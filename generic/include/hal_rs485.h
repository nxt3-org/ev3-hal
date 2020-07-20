#ifndef HAL_RS485
#define HAL_RS485

#include <stdbool.h>
#include <stdint.h>
#include "hal_rs485_defs.h"

extern bool Hal_Rs485_RefAdd(void);
extern bool Hal_Rs485_RefDel(void);

extern void Hal_Rs485_Setup(hs_speed_t speed, hs_params_t params, hs_uart_mode_t umode);
extern void Hal_Rs485_EnablePins(void);
extern void Hal_Rs485_EnableRx(void);
extern void Hal_Rs485_Disable(void);
extern void Hal_Rs485_Transmit(const uint8_t *buffer, uint32_t *pLength);
extern void Hal_Rs485_Receive(uint8_t *buffer, uint32_t *pLength);
extern void Hal_Rs485_TxStatus(uint32_t *pQueueLength);

#endif //HAL_RS485
