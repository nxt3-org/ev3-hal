#ifndef HAL_ADC_PRIVATE
#define HAL_ADC_PRIVATE

#include "hal_adc.h"
#include "sen_dummy_light.h"

#define VICTIM_PORT 1

typedef struct {
    int refCount;
    dummy_light_t *sensor;
} mod_adc_t;

extern mod_adc_t Mod_Adc;

#endif //HAL_ADC_PRIVATE
