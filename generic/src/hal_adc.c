#include "hal_adc.h"
#include <memory.h>

bool Hal_AdcDev_IsValid(hal_adc_dev_t *dev) {
    if (!dev || !dev->ops->is_valid)
        return false;
    return dev->ops->is_valid(dev);
}

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

void Hal_AdcDev_SetType(hal_adc_dev_t *dev, hal_adc_type_t type) {
    if (!dev || !dev->ops->set_type)
        return;
    return dev->ops->set_type(dev, type);
}

void Hal_AdcDev_LoadPins(hal_adc_dev_t *dev) {
    if (!dev || !dev->ops->load_pins)
        return;
    return dev->ops->load_pins(dev);
}

void Hal_AdcDev_StorePins(hal_adc_dev_t *dev) {
    if (!dev || !dev->ops->store_pins)
        return;
    return dev->ops->store_pins(dev);
}

void Hal_AdcDev_Tick(hal_adc_dev_t *dev) {
    if (!dev || !dev->ops->tick)
        return;
    return dev->ops->tick(dev);
}

bool Hal_AdcDev_JustAttached(hal_adc_dev_t *dev, int port) {
    if (!dev) return false;
    if (dev->port != -1) return false;
    dev->port = port;
    memset(&dev->pins, 0, sizeof(hal_pins_t));
    return true;
}

bool Hal_AdcDev_JustDetached(hal_adc_dev_t *dev) {
    if (!dev) return false;
    if (dev->port == -1) return false;
    dev->port = -1;
    return true;
}

void Hal_AdcDev_SetDigiDir(hal_adc_dev_t *dev, pin_t num, bool out) {
    if (!dev) return;
    if (num & DIGI0) {
        dev->pins.d0_dir = out ? DIR_OUT : DIR_IN;
    }
    if (num & DIGI1) {
        dev->pins.d1_dir = out ? DIR_OUT : DIR_IN;
    }
}

void Hal_AdcDev_SetDigiOut(hal_adc_dev_t *dev, pin_t num, bool high) {
    if (!dev) return;
    Hal_AdcDev_SetDigiDir(dev, num, DIR_OUT);
    if (num & DIGI0) {
        dev->pins.d0_out = high ? PIN_HIGH : PIN_LOW;
    }
    if (num & DIGI1) {
        dev->pins.d1_out = high ? PIN_HIGH : PIN_LOW;
    }
}

void Hal_AdcDev_GetDigiIn(hal_adc_dev_t *dev, pin_t num, uint8_t *pMask) {
    if (!dev) return;
    if (num & DIGI0) {
        if (dev->pins.d0_in) {
            *pMask |= 0x01;
        } else {
            *pMask &= ~0x01;
        }
    }
    if (num & DIGI1) {
        if (dev->pins.d1_in) {
            *pMask |= 0x02;
        } else {
            *pMask &= ~0x02;
        }
    }
}

void Hal_AdcDev_SetPower(hal_adc_dev_t *dev, pwr_aux_t mode) {
    if (!dev) return;
    dev->pins.pwr_mode = mode;
}
