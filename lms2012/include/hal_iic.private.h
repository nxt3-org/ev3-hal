#ifndef HAL_IIC_PRIVATE
#define HAL_IIC_PRIVATE

#include "sen_dummy_us.h"

#define VICTIM_PORT 0

typedef struct {
    int           refCount;
    dummy_sonic_t *pSonic;
} mod_i2c_t;

extern mod_i2c_t Mod_I2C;

#endif //HAL_IIC_PRIVATE
