#ifndef HAL_RS485_PRIVATE
#define HAL_RS485_PRIVATE

#include "hal_rs485.h"

typedef struct {
    int refCount;
} mod_rs485_t;

extern mod_rs485_t Mod_Rs485;

#endif //HAL_RS485_PRIVATE
