#include <malloc.h>
#include <io/adapters/touch.private.h>
#include <io/core/interface.h>
#include "hal_general.h"

#define adc_to_touch(lnk) container_of(lnk, sensor_ev3_touch_t, adc)
#define sen_to_touch(sen) container_of(sen, sensor_ev3_touch_t, dev)

static uint16_t ReadAdc(hal_adc_dev_t *dev) {
    sensor_ev3_touch_t *this = adc_to_touch(dev);

    float value;
    if (!this->iface->Sensor.Measure(this->port, &value, 1, UNIT_RAW_RANGE, true))
        return 1023;

    return 1023 * (1.0f - value);
}

static bool IsValid(hal_adc_dev_t *dev) {
    sensor_ev3_touch_t *this = adc_to_touch(dev);
    return this->iface->Ready(this->port);
}

static bool Attach(adapter_t *dev) {
    sensor_ev3_touch_t *this = sen_to_touch(dev);
    return Hal_AdcHost_Attach(&this->adc, this->port);
}

static void Detach(adapter_t *dev) {
    sensor_ev3_touch_t *this = sen_to_touch(dev);
    Hal_AdcHost_Detach(this->port);
}

static void Destroy(adapter_t *dev) {
    sensor_ev3_touch_t *this = sen_to_touch(dev);
    free(this);
}

static hal_adc_ops_t adc_ops = {
    .read_adc = ReadAdc,
};

static adapter_ops_t sensor_ops = {
    .Attach = Attach,
    .Detach = Detach,
    .Destroy = Destroy,
};

adapter_t *Create_Touch(int port, const interface_t *iface) {
    sensor_ev3_touch_t *dev = malloc(sizeof(sensor_ev3_touch_t));
    if (dev) {
        dev->adc.ops = &adc_ops;
        dev->dev.ops = &sensor_ops;
        dev->iface   = iface;
        dev->port    = port;
        return &dev->dev;
    }
    return NULL;
}
