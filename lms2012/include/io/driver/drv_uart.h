#ifndef DRV_UART
#define DRV_UART

#include "drv_port.h"

extern bool Drv_Uart_SwitchTo(uint8_t port, uint8_t mode);
extern bool Drv_Uart_Enqueue(uint8_t port, const uint8_t *command, uint8_t length);
extern bool Drv_Uart_DirectValue(uint8_t port, uint8_t slot, float *pValue);
extern bool Drv_Uart_RawRange(uint8_t port, uint8_t slot, float *pValue);
extern bool Drv_Uart_PctValue(uint8_t port, uint8_t slot, float *pValue);
extern bool Drv_Uart_SiValue(uint8_t port, uint8_t slot, float *pValue);

extern port_driver_ops_t DriverUart;

#endif //DRV_UART
