#include <hal_general.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <malloc.h>
#include "hal_filesystem.h"
#include "comm/hal_usb.private.h"
#include "common/kdevices.h"

mod_usb_t Mod_Usb;

bool Hal_Usb_RefAdd(void) {
    if (Mod_Usb.refCount > 0) {
        Mod_Usb.refCount++;
        return true;
    }

    for (int hnd = 0; hnd < MAX_HANDLES; hnd++) {
        Hal_Usb_RemoveHandle(hnd);
    }

    Mod_Usb.rxBuffer    = calloc(BUFFER_SIZE, 1);
    Mod_Usb.txBuffer    = calloc(BUFFER_SIZE, 1);
    Mod_Usb.nxtRxBuffer = calloc(NXT_BUFFER_SIZE, 1);
    Mod_Usb.nxtTxBuffer = calloc(NXT_BUFFER_SIZE, 1);
    if (!Mod_Usb.rxBuffer || !Mod_Usb.txBuffer ||
        !Mod_Usb.nxtRxBuffer || !Mod_Usb.nxtTxBuffer) {
        goto freeBuffers;
    }

    Mod_Usb.modeFd = open("/sys/devices/platform/musb_hdrc/mode", O_RDONLY);
    if (Mod_Usb.modeFd < 0)
        goto freeBuffers;

    if (!Ev3Proto_Init(&Mod_Usb.ev3,
                       (remotebuf_t) {Mod_Usb.rxBuffer, &Mod_Usb.rxCount},
                       (remotebuf_t) {Mod_Usb.txBuffer, &Mod_Usb.txCount},
                       BUFFER_SIZE)) {
        goto closeFd;
    }

    if (!Kdev_RefAdd(&DeviceUsbDev))
        goto deinitProto;

    char tmp;
    while (Kdev_Read(&DeviceUsbDev, &tmp, 1) > 0)
        /* clear the input buffer */;
    Mod_Usb.present    = false;
    Mod_Usb.ready      = false;
    Mod_Usb.addrSet    = false;
    Mod_Usb.rxCount    = 0;
    Mod_Usb.txCount    = 0;
    Mod_Usb.nxtRxCount = 0;
    Mod_Usb.nxtTxCount = 0;
    Mod_Usb.refCount++;
    return true;

deinitProto:
    Ev3Proto_RefDel(&Mod_Usb.ev3);
closeFd:
    close(Mod_Usb.modeFd);
    Mod_Usb.modeFd = -1;
freeBuffers:
    free(Mod_Usb.rxBuffer);
    free(Mod_Usb.txBuffer);
    free(Mod_Usb.nxtRxBuffer);
    free(Mod_Usb.nxtTxBuffer);
    Mod_Usb.rxBuffer    = NULL;
    Mod_Usb.txBuffer    = NULL;
    Mod_Usb.nxtRxBuffer = NULL;
    Mod_Usb.nxtTxBuffer = NULL;
    return false;
}

bool Hal_Usb_RefDel(void) {
    if (Mod_Usb.refCount == 0)
        return false;
    if (Mod_Usb.refCount == 1) {
        if (Mod_Usb.rxBuffer) {
            free(Mod_Usb.rxBuffer);
            Mod_Usb.rxBuffer = NULL;
        }
        if (Mod_Usb.txBuffer) {
            free(Mod_Usb.txBuffer);
            Mod_Usb.txBuffer = NULL;
        }
        if (Mod_Usb.nxtRxBuffer) {
            free(Mod_Usb.nxtRxBuffer);
            Mod_Usb.nxtRxBuffer = NULL;
        }
        if (Mod_Usb.nxtTxBuffer) {
            free(Mod_Usb.nxtTxBuffer);
            Mod_Usb.nxtTxBuffer = NULL;
        }
        if (Mod_Usb.modeFd >= 0) {
            close(Mod_Usb.modeFd);
            Mod_Usb.modeFd = -1;
        }
        if (!Ev3Proto_RefDel(&Mod_Usb.ev3))
            Hal_General_AbnormalExit("Cannot deinitialize EV3 protocol");
        if (!Kdev_RefDel(&DeviceUsbDev))
            Hal_General_AbnormalExit("Cannot deinitialize USB peripheral");
    }
    Mod_Usb.refCount--;
    return true;
}

bool Hal_Usb_IsPresent(void) {
    if (Mod_Usb.refCount <= 0) return false;
    return Mod_Usb.present;
}

bool Hal_Usb_IsReady(void) {
    if (Mod_Usb.refCount <= 0) return false;
    return Mod_Usb.ready;
}

void Hal_Usb_Tick(void) {
    if (Mod_Usb.refCount <= 0) return;

    Hal_Usb_ReloadPresence();
    if (!Mod_Usb.ready) return;

    if (Mod_Usb.nxtTxCount > 0) {
        if (Hal_Usb_HandleNxtTx())
            return; // no two consecutive transmits in one loop should occur
    }

    if (Hal_Usb_DoRead()) {
        int bytes   = (Mod_Usb.rxBuffer[1] << 8 | Mod_Usb.rxBuffer[0] << 0) + 2;
        int counter = Mod_Usb.rxBuffer[3] << 8 | Mod_Usb.rxBuffer[2] << 0;
        int type    = Mod_Usb.rxBuffer[4];
        switch (type) {
        case COMMAND_NXT3_HOST_TO_DEV:
            Hal_Usb_HandleNxtRx(bytes);
            break;
        case COMMAND_EV3_SYS_REQUEST:
        case COMMAND_EV3_SYS_REQUEST_QUIET:
            if (!Ev3Proto_SystemCommand(&Mod_Usb.ev3))
                Hal_Usb_RejectSysCommand(bytes, counter, type);
            break;
        case COMMAND_EV3_VM_REQUEST:
            Hal_Usb_RejectDirectCmd(counter);
            break;
        case COMMAND_NXT3_DEV_TO_HOST:
        case COMMAND_EV3_SYS_REPLY_OK:
        case COMMAND_EV3_SYS_REPLY_ERROR:
        case COMMAND_EV3_VM_REQUEST_QUIET:
        case COMMAND_EV3_VM_REPLY_OK:
        case COMMAND_EV3_VM_REPLY_ERROR:
            // quietly ignore
            break;
        }
    }
    Hal_Usb_DoWrite();
}

void Hal_Usb_ReloadPresence(void) {
    char buffer[16];
    int  bytes = pread(Mod_Usb.modeFd, &buffer, sizeof(buffer), 0);
    if (bytes >= 0 && DeviceUsbDev.mmap->usbSpeed == USB_SPEED_HIGH) {
        buffer[bytes - 1] = '\0';
        bool wasOK = Mod_Usb.ready;
        Mod_Usb.present = strstr(buffer, "b_idle") == NULL;
        Mod_Usb.ready   = strstr(buffer, "b_peripheral") != NULL;
        if (!wasOK && Mod_Usb.ready)
            Ev3Proto_ConnEstablished(&Mod_Usb.ev3);
    } else {
        if (Mod_Usb.ready)
            Ev3Proto_ConnLost(&Mod_Usb.ev3);
        Mod_Usb.present = false;
        Mod_Usb.ready   = false;
    }
}

bool Hal_Usb_DoRead(void) {
    Mod_Usb.rxCount = Kdev_Read(&DeviceUsbDev, Mod_Usb.rxBuffer, BUFFER_SIZE);
    return Mod_Usb.rxCount > 0;
}

bool Hal_Usb_DoWrite(void) {
    if (Mod_Usb.txCount <= 0) return true;
    if (Mod_Usb.txCount > BUFFER_SIZE)
        Mod_Usb.txCount = BUFFER_SIZE;

    memset(Mod_Usb.txBuffer + Mod_Usb.txCount, 0, BUFFER_SIZE - Mod_Usb.txCount);
    int done = Kdev_Write(&DeviceUsbDev, Mod_Usb.txBuffer, BUFFER_SIZE);
    if (done > 0)
        Mod_Usb.txCount = 0;
    return done > 0;
}

void Hal_Usb_RejectDirectCmd(int counter) {
    Mod_Usb.txBuffer[0] = 5 - 2;
    Mod_Usb.txBuffer[1] = 0;
    Mod_Usb.txBuffer[2] = (counter >> 0) & 0xFF;
    Mod_Usb.txBuffer[3] = (counter >> 8) & 0xFF;
    Mod_Usb.txBuffer[4] = COMMAND_EV3_VM_REPLY_ERROR;
    Mod_Usb.txCount = 5;
}

void Hal_Usb_RejectSysCommand(int bytes, int counter, int type) {
    if (type == COMMAND_EV3_SYS_REQUEST_QUIET)
        return;
    Mod_Usb.txBuffer[0] = 5 - 2;
    Mod_Usb.txBuffer[1] = 0;
    Mod_Usb.txBuffer[2] = (counter >> 0) & 0xFF;
    Mod_Usb.txBuffer[3] = (counter >> 8) & 0xFF;
    Mod_Usb.txBuffer[4] = COMMAND_EV3_SYS_REPLY_ERROR;
    Mod_Usb.txCount = 5;
    (void) bytes;
}

void Hal_Usb_StoreBtAddress(const uint8_t *raw) {
    (void) raw;
    Mod_Usb.addrSet = true;
}

void Hal_Usb_ResetState(void) {
    Mod_Usb.nxtTxCount = 0;
    Mod_Usb.nxtRxCount = 0;
    Mod_Usb.txCount    = 0;
    Mod_Usb.rxCount    = 0;
}

bool Hal_Usb_HandleNxtTx(void) {
    int size = 5 + Mod_Usb.nxtTxCount;
    Mod_Usb.txBuffer[0] = ((size - 2) >> 0) & 0xFF;
    Mod_Usb.txBuffer[1] = ((size - 2) >> 8) & 0xFF;
    Mod_Usb.txBuffer[2] = 0x00; // counter is always zero for now
    Mod_Usb.txBuffer[3] = 0x00;
    Mod_Usb.txBuffer[4] = COMMAND_NXT3_DEV_TO_HOST;
    memcpy(Mod_Usb.txBuffer + 5, Mod_Usb.nxtTxBuffer, Mod_Usb.nxtTxCount);
    Mod_Usb.txCount = size;
    if (Hal_Usb_DoWrite()) {
        Mod_Usb.nxtTxCount = 0;
        return true;
    }
    return false;
}

void Hal_Usb_HandleNxtRx(int bytes) {
    if (bytes <= 5) return;
    int count = bytes - 5;
    if (count > NXT_BUFFER_SIZE)
        count = NXT_BUFFER_SIZE;
    memcpy(Mod_Usb.nxtRxBuffer, Mod_Usb.rxBuffer + 5, count);
    Mod_Usb.nxtRxCount = count;
}

uint32_t Hal_Usb_RxFrame(uint8_t *buffer, uint32_t maxLength) {
    if (Mod_Usb.refCount <= 0) return 0;
    if (Mod_Usb.nxtRxCount > 0) {
        int count = maxLength < Mod_Usb.nxtRxCount ? maxLength : Mod_Usb.nxtRxCount;
        memcpy(buffer, Mod_Usb.nxtRxBuffer, count);
        Mod_Usb.nxtRxCount = 0;
        return count;
    }
    return 0;
}

void Hal_Usb_TxFrame(const uint8_t *buffer, uint32_t maxLength) {
    if (Mod_Usb.refCount <= 0) return;
    if (maxLength == 0) return;

    int count = maxLength < NXT_BUFFER_SIZE ? maxLength : NXT_BUFFER_SIZE;
    memcpy(Mod_Usb.nxtTxBuffer, buffer, count);
    Mod_Usb.nxtTxCount = count;
}
