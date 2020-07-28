#include "io/driver/drv_uart.private.h"
#include <memory.h>
#include <sys/ioctl.h>
#include <io/driver/drv_uart.h>
#include <stdio.h>
#include <unistd.h>

drv_uart_t Drv_Uart;

port_driver_ops_t DriverUart = {
    .Init = Drv_Uart_RefAdd,
    .Exit = Drv_Uart_RefDel,
    .Tick = Drv_Uart_Tick,
    .SetCallbacks = Drv_Uart_PnpSetCallbacks,
    .DeviceStart = Drv_Uart_PnpStart,
    .DeviceStop = Drv_Uart_PnpStop
};

bool Drv_Uart_RefAdd(void) {
    if (Drv_Uart.refCount > 0) {
        Drv_Uart.refCount++;
        return true;
    }

    Drv_Uart.idCalls   = NULL;
    Drv_Uart.modeCalls = NULL;

    if (!Kdev_RefAdd(&DeviceAnalog))
        return false;
    for (int no = 0; no < 4; no++) {
        memset(Drv_Uart.ports[no].types, 0, 8 * sizeof(devinfo_t));
        Drv_Uart.ports[no].state = UART_OFF;
        Drv_Uart.devmap.link[no] = DeviceAnalog.mmap->Dcm_InLink[no];
        Drv_Uart.devmap.type[no] = DCM_DEV_UNKNOWN;
        Drv_Uart.devmap.mode[no] = 0;
    }
    if (!Kdev_RefDel(&DeviceAnalog))
        return false;
    if (!Kdev_RefAdd(&DeviceUart))
        return false;

    if (!Drv_Uart_KernelUpload())
        return false;
    for (int no = 0; no < 4; no++)
        DeviceUart.mmap->Flags[no] = 0;

    Drv_Uart.refCount++;
    return true;
}

bool Drv_Uart_RefDel(void) {
    if (Drv_Uart.refCount == 0)
        return false;
    if (Drv_Uart.refCount == 1) {
        Kdev_RefDel(&DeviceUart);
    }
    Drv_Uart.refCount--;
    return true;
}

void Drv_Uart_Tick(void) {
    static int oldState = -100;
    for (int   sPort    = 0; sPort < 4; sPort++) {
        uint8_t flags = DeviceUart.mmap->Flags[sPort];
        switch (Drv_Uart.ports[sPort].state) {
        case UART_OFF:
            break;
        case UART_KERNEL_HANDSHAKE:
            if (flags & UART_FLAG_PNP_CHANGE) {
                Drv_Uart_ClearChanged(sPort);
                Drv_Uart.ports[sPort].state = UART_READING_INFO;
                break;
            }
            if (flags & UART_FLAG_DATA_READY) {
                Drv_Uart.ports[sPort].state = UART_READING_INFO;
                break;
            }
            if (--Drv_Uart.ports[sPort].timer == 0) {
                Drv_Uart.ports[sPort].state = UART_OFF;
                Drv_Uart.idCalls->failure(sPort | DCM_TYPE_INPUT);
                break;
            }
            break;
        case UART_READING_INFO: {
            for (int mode = 0; mode < MAX_MODES; mode++) {
                Drv_Uart_ReadType(sPort, mode);
            }

            dcm_dev_t    dev    = Drv_Uart.ports[sPort].types[0].Device;
            pnp_device_t pnpDev = dcm2pnp_dev(dev);
            if (pnpDev == PNP_DEVICE_NONE || pnpDev == PNP_DEVICE_UNKNOWN) {
                Drv_Uart.ports[sPort].state = UART_OFF;
                Drv_Uart.idCalls->failure(sPort | DCM_TYPE_INPUT);
            } else if (pnpDev == PNP_DEVICE_TTY) {
                Drv_Uart.ports[sPort].state = UART_TTY;
                Drv_Uart.idCalls->success(sPort | DCM_TYPE_INPUT, PNP_LINK_UART, pnpDev, 0);
            } else {
                Drv_Uart.ports[sPort].state = UART_WAITING_FOR_READY;
            }
            break;
        }
        case UART_WAITING_FOR_READY:
            if (--Drv_Uart.ports[sPort].timer == 0) {
                Drv_Uart.ports[sPort].state = UART_OFF;
                Drv_Uart.idCalls->failure(sPort | DCM_TYPE_INPUT);
                break;
            }
            if (flags & UART_FLAG_PNP_CHANGE) {
                Drv_Uart_ClearChanged(sPort);
                break;
            }
            if (flags & UART_FLAG_DATA_READY) {
                dcm_dev_t    dev    = Drv_Uart.ports[sPort].types[0].Device;
                pnp_device_t pnpDev = dcm2pnp_dev(dev);

                Drv_Uart.devmap.type[sPort] = dev;
                Drv_Uart.ports[sPort].state = UART_READY;
                Drv_Uart.idCalls->success(sPort | DCM_TYPE_INPUT, PNP_LINK_UART, pnpDev, 0);
                break;
            }
            break;
        case UART_READY:
            if (flags & UART_FLAG_PNP_CHANGE) {
                Drv_Uart_ClearChanged(sPort);
                // this seems very bad, the sensor probably reset itself, let's go mode 0
                Drv_Uart.modeCalls->started(sPort | DCM_TYPE_INPUT);
                Drv_Uart.modeCalls->finished(sPort | DCM_TYPE_INPUT, 0);
            }
            break;
        case UART_TTY:
            break;
        case UART_WRITING:
            if (!(flags & UART_FLAG_SENDING)) {
                Drv_Uart.ports[sPort].state = UART_READY;
            }
            break;
        case UART_MODESWITCHING:
            if (flags & UART_FLAG_DATA_READY) {
                Drv_Uart.modeCalls->finished(sPort | DCM_TYPE_INPUT, Drv_Uart.devmap.mode[sPort]);
                Drv_Uart.ports[sPort].state = UART_READY;
            }
            if (flags & UART_FLAG_PNP_CHANGE) {
                Drv_Uart_ClearChanged(sPort);
                Drv_Uart.devmap.mode[sPort] = 0;
            }
            break;
        }
    }
}

void Drv_Uart_PnpStart(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev) {
    if ((port & DCM_TYPE_MASK) != DCM_TYPE_INPUT) {
        Drv_Uart.idCalls->failure(port);
        return;
    }
    uint8_t sPort = port & DCM_PORT_MASK;
    Drv_Uart.ports[sPort].timer = MAX_HANDSHAKE_TIME;
    Drv_Uart.ports[sPort].state = UART_KERNEL_HANDSHAKE;
    DeviceUart.mmap->Flags[sPort] &= ~UART_FLAG_DATA_READY;
    Drv_Uart.devmap.link[sPort] = DCM_LINK_INPUT_UART;
    Drv_Uart.devmap.type[sPort] = DCM_DEV_UNKNOWN;
    Drv_Uart.devmap.mode[sPort] = 0;
    Drv_Uart_KernelUpload();
}

void Drv_Uart_PnpStop(dcm_port_id_t port) {
    if ((port & DCM_TYPE_MASK) != DCM_TYPE_INPUT)
        return;

    uint8_t sPort = port & DCM_PORT_MASK;
    Drv_Uart.ports[sPort].state = UART_OFF;
    Drv_Uart.devmap.link[sPort] = DCM_LINK_NONE;
    Drv_Uart.devmap.type[sPort] = DCM_DEV_NONE;
    Drv_Uart.devmap.mode[sPort] = 0;
    Drv_Uart_KernelUpload();
    DeviceUart.mmap->Flags[sPort] = 0;
}

void Drv_Uart_PnpSetCallbacks(identify_callback_t *id, modeswitch_callback_t *mode) {
    Drv_Uart.idCalls   = id;
    Drv_Uart.modeCalls = mode;
}

bool Drv_Uart_SwitchTo(uint8_t port, uint8_t mode) {
    if (port >= 4)
        return false;
    if (Drv_Uart.ports[port].state != UART_READY)
        return false;

    Drv_Uart.devmap.mode[port] = mode;
    Drv_Uart_KernelUpload();
    DeviceUart.mmap->Flags[port] &= ~UART_FLAG_DATA_READY;
    Drv_Uart.ports[port].state = UART_MODESWITCHING;
    Drv_Uart.modeCalls->started(port | DCM_TYPE_INPUT);
    return true;
}

bool Drv_Uart_Enqueue(uint8_t port, const uint8_t *command, uint8_t length) {
    if (port >= 4)
        return false;
    if (Drv_Uart.ports[port].state != UART_READY)
        return false;

    Drv_Uart_Write(port, command, length);
    Drv_Uart.ports[port].state = UART_WRITING;
    return true;
}

bool Drv_Uart_ReadType(uint8_t sPort, uint8_t mode) {
    uartinfo_t info = {
        .typedata = {},
        .port = sPort,
        .mode = mode
    };

    if (Kdev_Ioctl(&DeviceUart, KERNEL_ENABLE_INFO, &info) < 0)
        return false;
    if (Kdev_Ioctl(&DeviceUart, KERNEL_READ_INFO, &info) < 0)
        return false;
    memcpy(&Drv_Uart.ports[sPort].types[mode], &info.typedata, sizeof(devinfo_t));
    return true;
}

bool Drv_Uart_KernelUpload(void) {
    return Kdev_Ioctl(&DeviceUart, KERNEL_UPLOAD, &Drv_Uart.devmap) >= 0;
}

bool Drv_Uart_ClearChanged(uint8_t sPort) {
    uartinfo_t info = {
        .typedata = {},
        .port = sPort,
        .mode = 0
    };

    DeviceUart.mmap->Flags[sPort] &= ~UART_FLAG_PNP_CHANGE;
    return Kdev_Ioctl(&DeviceUart, KERNEL_CLEAR_CHANGED, &info);
}

bool Drv_Uart_Write(uint8_t sPort, const uint8_t *buffer, uint8_t length) {
    if (DeviceUart.mmap->Flags[sPort] & UART_FLAG_SENDING)
        return false;

    if (length > MAX_UART_MSGLEN)
        length = MAX_UART_MSGLEN;

    uint8_t command[1 + MAX_UART_MSGLEN];
    command[0] = sPort;
    memcpy(command + 1, buffer, length);
    if (Kdev_Write(&DeviceUart, command, 1 + length, 0) >= 0) {
        DeviceUart.mmap->Flags[sPort] |= UART_FLAG_SENDING;
        return true;
    }
    return false;
}

bool Drv_Uart_DirectValue(uint8_t port, uint8_t slot, float *pValue) {
    if (port >= 4 || Drv_Uart.ports[port].state != UART_READY)
        return false;
    if (slot >= 8)
        return false;

    uint8_t       mode = Drv_Uart.devmap.mode[port];
    data_format_t fmt  = Drv_Uart.ports[port].types[mode].InterpretAs;

    uint16_t current = DeviceUart.mmap->Buffer_LastPtr[port];
    uint8_t  *data   = DeviceUart.mmap->Buffer_Data[port][current];

    if (fmt == FORMAT_S8) {
        *pValue = *((int8_t *) data + slot);
    } else if (fmt == FORMAT_S16) {
        *pValue = *((int16_t *) data + slot);
    } else if (fmt == FORMAT_S32) {
        *pValue = *((int32_t *) data + slot);
    } else if (fmt == FORMAT_FLT) {
        *pValue = *((float *) data + slot);
    }
    return true;
}

bool Drv_Uart_RawRange(uint8_t port, uint8_t slot, float *pValue) {
    float direct = 0.0f;
    if (!Drv_Uart_DirectValue(port, slot, &direct))
        return false;

    uint8_t mode = Drv_Uart.devmap.mode[port];
    float   rMin = Drv_Uart.ports[port].types[mode].RawMin;
    float   rMax = Drv_Uart.ports[port].types[mode].RawMax;
    float   full = (direct - rMin) / (rMax - rMin);
    if (full > 1.0f)
        *pValue = 1.0f;
    else if (full < 0.0f)
        *pValue = 0.0f;
    else
        *pValue = full;
    return true;
}

bool Drv_Uart_PctValue(uint8_t port, uint8_t slot, float *pValue) {
    float range = 0.0f;
    if (!Drv_Uart_RawRange(port, slot, &range))
        return false;

    uint8_t mode = Drv_Uart.devmap.mode[port];
    float   pMin = Drv_Uart.ports[port].types[mode].PercentMin;
    float   pMax = Drv_Uart.ports[port].types[mode].PercentMax;

    *pValue = range * (pMax - pMin) + pMin;
    return true;
}

bool Drv_Uart_SiValue(uint8_t port, uint8_t slot, float *pValue) {
    float range = 0.0f;
    if (!Drv_Uart_RawRange(port, slot, &range))
        return false;

    uint8_t mode = Drv_Uart.devmap.mode[port];
    float   sMin = Drv_Uart.ports[port].types[mode].SiMin;
    float   sMax = Drv_Uart.ports[port].types[mode].SiMax;

    *pValue = range * (sMax - sMin) + sMin;
    return true;
}
