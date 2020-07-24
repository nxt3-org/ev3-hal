#ifndef DRV_UART
#define DRV_UART

#include "drv_port.h"

extern bool Drv_Uart_SwitchTo(uint8_t port, uint8_t mode);
extern bool Drv_Uart_Enqueue(uint8_t port, const uint8_t *command, uint8_t length);
extern bool Drv_Uart_DirectValue(uint8_t port, uint8_t slot, float *pValue);
extern bool Drv_Uart_RawRange(uint8_t port, uint8_t slot, float *pValue);
extern bool Drv_Uart_PctValue(uint8_t port, uint8_t slot, float *pValue);
extern bool Drv_Uart_SiValue(uint8_t port, uint8_t slot, float *pValue);


typedef struct {
    uint16_t readPtr;
    uint16_t lastAge;
} segread_t;

extern bool Drv_Uart_StartCalRead(uint8_t port, segread_t *pRead);
extern bool Drv_Uart_ContinueCalRead(uint8_t port, segread_t *pRead, uint16_t *row, bool *pReady, bool *pWrite);


extern port_driver_ops_t DriverUart;

#endif //DRV_UART
