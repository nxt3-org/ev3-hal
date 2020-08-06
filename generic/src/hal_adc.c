#include "hal_adc.h"
#include <memory.h>

uint16_t Hal_AdcDev_ReadAdc(hal_adc_dev_t *dev) {
    if (!dev || !dev->ops->read_adc)
        return 0;
    return dev->ops->read_adc(dev);
}

bool Hal_AdcDev_IsColorPresent(hal_adc_dev_t *dev) {
    if (!dev || !dev->ops->color_active)
        return false;
    return dev->ops->color_active(dev);
}

bool Hal_AdcDev_ReadColorAdc1(hal_adc_dev_t *dev, uint16_t *rawAD) {
    if (!rawAD || !dev || !dev->ops->read_color_adc1)
        return false;
    return dev->ops->read_color_adc1(dev, rawAD);
}

bool Hal_AdcDev_ReadColorAdc4(hal_adc_dev_t *dev, uint16_t *rawAD) {
    if (!rawAD || !dev || !dev->ops->read_color_adc4)
        return false;
    return dev->ops->read_color_adc4(dev, rawAD);
}

bool Hal_AdcDev_ReadColorCalib(hal_adc_dev_t *dev, CALDATA *out) {
    if (!out || !dev || !dev->ops->read_color_caldata)
        return false;
    return dev->ops->read_color_caldata(dev, out);
}
