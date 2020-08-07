#include <memory.h>
#include "io/driver/if_uart.private.h"

bool Drv_Uart_KernelReadType(uint8_t sPort, uint8_t mode) {
    uartinfo_t info = {
        .typedata = {},
        .port = sPort,
        .mode = mode
    };

    if (Kdev_Ioctl(&DeviceUart, KERNEL_ENABLE_INFO, &info) < 0)
        return false;
    if (Kdev_Ioctl(&DeviceUart, KERNEL_READ_INFO, &info) < 0)
        return false;
    memcpy(&Drv_Uart.types[sPort][mode].Main, &info.typedata, sizeof(typedata_t));
    return true;
}

bool Drv_Uart_KernelUpload(int invalidateMask) {
    bool     ok = Kdev_Ioctl(&DeviceUart, KERNEL_UPLOAD, &Drv_Uart.devmap) >= 0;
    for (int i  = 0; i < 4; i++) {
        if (invalidateMask & (1 << i)) {
            DeviceUart.mmap->Flags[i] &= ~UART_FLAG_DATA_READY;
        }
    }
    return ok;
}

bool Drv_Uart_KernelClearChanged(uint8_t sPort) {
    uartinfo_t info = {
        .typedata = {},
        .port = sPort,
        .mode = 0
    };

    DeviceUart.mmap->Flags[sPort] &= ~UART_FLAG_PNP_CHANGE;
    return Kdev_Ioctl(&DeviceUart, KERNEL_CLEAR_CHANGED, &info);
}

bool Drv_Uart_KernelWrite(uint8_t sPort, const uint8_t *buffer, uint8_t length) {
    if (DeviceUart.mmap->Flags[sPort] & UART_FLAG_SENDING)
        return false;

    if (length > MAX_UART_MSGLEN)
        length = MAX_UART_MSGLEN;

    uint8_t command[1 + MAX_UART_MSGLEN];
    command[0] = sPort;
    memcpy(command + 1, buffer, length);
    if (Kdev_Pwrite(&DeviceUart, command, 1 + length, 0) >= 0) {
        DeviceUart.mmap->Flags[sPort] |= UART_FLAG_SENDING;
        return true;
    }
    return false;
}
