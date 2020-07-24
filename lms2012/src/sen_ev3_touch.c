#include "sen_ev3_touch.private.h"
#include <malloc.h>
#include "hal_general.h"
#include "drv_analog.h"

#define iic_to_touch(lnk) container_of(lnk, sensor_ev3_touch_t, iic)
#define adc_to_touch(lnk) container_of(lnk, sensor_ev3_touch_t, adc)
#define sen_to_touch(sen) container_of(sen, sensor_ev3_touch_t, dev)

static uint16_t readAdc(hal_adc_dev_t *dev) {
    sensor_ev3_touch_t *this = adc_to_touch(dev);

    float value;
    if (!Drv_Analog_RawRange(this->port, &value))
        return 0;

    int16_t val = 1023 * (1.0f - value);
    if (val > 1023)
        return 1023;
    if (val < 0)
        return 0;
    return val;
}

static bool isValid(hal_adc_dev_t *dev) {
    return true;
}

bool attach(sensor_dev_t *dev) {
    sensor_ev3_touch_t *this = sen_to_touch(dev);
    return Hal_AdcHost_Attach(&this->adc, this->port) &&
           Hal_IicHost_Attach(&this->iic, this->port);
}

void detach(sensor_dev_t *dev) {
    sensor_ev3_touch_t *this = sen_to_touch(dev);
    Hal_AdcHost_Detach(this->port);
    Hal_IicHost_Detach(this->port);
}

void destroy(sensor_dev_t *dev) {
    sensor_ev3_touch_t *this = sen_to_touch(dev);
    free(this);
}

static hal_iic_result_t iic_reject(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    return HAL_IIC_RESULT_ERROR;
}

static hal_iic_ops_t iic_ops = {
    .preread = iic_reject,
    .prewrite = iic_reject,
    .read = NULL,
    .write = NULL,
    .cancel = NULL,
    .tick = NULL
};

static hal_adc_ops_t adc_ops = {
    .is_valid = isValid,
    .read_adc = readAdc,
    .color_active = NULL,
    .read_color_adc1    = NULL,
    .read_color_adc4    = NULL,
    .read_color_caldata = NULL,
    .set_type   = NULL,
    .load_pins  = NULL,
    .store_pins = NULL,
};

static sensor_ops_t sensor_ops = {
    .attach = attach,
    .detach = detach,
    .destroy = destroy,
};

sensor_dev_t *Sensor_EV3Touch_Create(int port) {
    sensor_ev3_touch_t *dev = malloc(sizeof(sensor_ev3_touch_t));
    if (dev) {
        dev->adc.ops = &adc_ops;
        dev->iic.ops = &iic_ops;
        dev->dev.ops = &sensor_ops;
        dev->port     = port;
        return &dev->dev;
    }
    return NULL;
}
