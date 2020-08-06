#ifndef HAL_ADC
#define HAL_ADC

#include "hal_adc_defs.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct hal_adc_ops hal_adc_ops_t;
typedef struct hal_adc_dev hal_adc_dev_t;
typedef struct hal_pins    hal_pins_t;

struct hal_adc_ops {
    uint16_t (*read_adc)(hal_adc_dev_t *dev);
    bool (*color_active)(hal_adc_dev_t *dev);
    bool (*read_color_adc1)(hal_adc_dev_t *dev, uint16_t *rawAD);
    bool (*read_color_adc4)(hal_adc_dev_t *dev, uint16_t *rawAD);
    bool (*read_color_caldata)(hal_adc_dev_t *dev, CALDATA *out);
};

struct hal_adc_dev {
    const hal_adc_ops_t *ops;
};

// ADC "controller"
// to be implemented by NXT3 core
extern bool Hal_AdcHost_Attach(hal_adc_dev_t *device, int port);
extern bool Hal_AdcHost_Detach(int port);

// ADC "device"
// implemented locally in HAL core (proxy for struct hal_adc_ops)
extern uint16_t Hal_AdcDev_ReadAdc(hal_adc_dev_t *dev);
extern bool Hal_AdcDev_IsColorPresent(hal_adc_dev_t *dev);
extern bool Hal_AdcDev_ReadColorAdc1(hal_adc_dev_t *dev, uint16_t *rawAD);
extern bool Hal_AdcDev_ReadColorAdc4(hal_adc_dev_t *dev, uint16_t *rawAD);
extern bool Hal_AdcDev_ReadColorCalib(hal_adc_dev_t *dev, CALDATA *out);

#endif //HAL_ADC
