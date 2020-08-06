#include <malloc.h>
#include <io/adapters/nxt_light.private.h>
#include <io/core/interface.h>
#include "hal_general.h"

#define adc_to_light(lnk) container_of(lnk, sensor_nxt_light_t, adc)
#define sen_to_light(sen) container_of(sen, sensor_nxt_light_t, dev)

static uint16_t ReadAdc(hal_adc_dev_t *dev) {
    sensor_nxt_light_t *this = adc_to_light(dev);

    float value;
    if (!this->iface->Sensor.Measure(this->port, &value, 1, UNIT_RAW, true))
        return 0;

    // do not decode the values, this is already in NXT core
    value /= 4.0f;
    if (value >= 1023.0f)
        return 1023;
    if (value < 0.0f)
        return 0;
    return (uint16_t) value;
}

static bool IsValid(adapter_t *dev) {
    sensor_nxt_light_t *this = sen_to_light(dev);
    return this->iface->Ready(this->port);
}

static bool Attach(adapter_t *dev) {
    sensor_nxt_light_t *this = sen_to_light(dev);
    return Hal_AdcHost_Attach(&this->adc, this->port);
}

static void Detach(adapter_t *dev) {
    sensor_nxt_light_t *this = sen_to_light(dev);
    Hal_AdcHost_Detach(this->port);
    Hal_IicHost_Detach(this->port);
}

static void Destroy(adapter_t *dev) {
    sensor_nxt_light_t *this = sen_to_light(dev);
    free(this);
}

static bool StorePins(adapter_t *dev, struct hal_pins pins) {
    sensor_nxt_light_t *this = sen_to_light(dev);
    this->pins = pins;
    if (pins.d0_dir == DIR_OUT && pins.d0_out == PIN_HIGH) {
        this->iface->Sensor.StartSwitch(this->port, PNP_MODE_NXT_LIGHT_REFLECT);
    } else {
        this->iface->Sensor.StartSwitch(this->port, PNP_MODE_NXT_LIGHT_AMBIENT);
    }
    return true;
}

static hal_adc_ops_t adc_ops = {
    .read_adc = ReadAdc,
};

static adapter_ops_t sensor_ops = {
    .Attach = Attach,
    .Detach = Detach,
    .Destroy = Destroy,
    .IsReady = IsValid,
    .SetPins = StorePins
};

adapter_t *Create_NxtLight(int port, const interface_t *iface) {
    sensor_nxt_light_t *dev = malloc(sizeof(sensor_nxt_light_t));
    if (dev) {
        dev->adc.ops = &adc_ops;
        dev->dev.ops = &sensor_ops;
        dev->iface   = iface;
        dev->port    = port;
        return &dev->dev;
    }
    return NULL;
}
