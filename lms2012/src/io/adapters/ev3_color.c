#include "io/adapters/ev3_color.private.h"
#include "hal_general.h"
#include <malloc.h>
#include <memory.h>

#define   VccSensor                     5000L
#define   AdRes                         1023L
#define   LightMin                      (800L/(VccSensor/AdRes))
#define   LightMax                      ((AdRes * 4400L)/VccSensor)

#define adc_to_color(sns)  container_of(sns, ev3_color_t, adc)
#define sen_to_color(sns)  container_of(sns, ev3_color_t, dev)

static bool Attach(adapter_t *dev) {
    ev3_color_t *this = sen_to_color(dev);
    return Hal_AdcHost_Attach(&this->adc, this->port);
}

static void Detach(adapter_t *dev) {
    ev3_color_t *this = sen_to_color(dev);
    Hal_AdcHost_Detach(this->port);
}

static void Destroy(adapter_t *dev) {
    ev3_color_t *this = sen_to_color(dev);
    free(this);
}

static bool IsValid(adapter_t *dev) {
    ev3_color_t *this = sen_to_color(dev);
    return this->iface->Ready(this->port);
}

static bool StorePins(adapter_t *dev, struct hal_pins pins) {
    ev3_color_t *this = sen_to_color(dev);
    this->pins = pins;

    if (pins.d0_dir == DIR_OUT && pins.d0_out == PIN_HIGH) {
        this->iface->Sensor.StartSwitch(this->port, PNP_MODE_EV3_COLOR_REFLECT);
    } else {
        this->iface->Sensor.StartSwitch(this->port, PNP_MODE_EV3_COLOR_AMBIENT);
    }
    return true;
}

static uint16_t ReadAdc(hal_adc_dev_t *dev) {
    ev3_color_t *this = adc_to_color(dev);

    float percent;
    if (this->iface->Sensor.Measure(this->port, &percent, 1, UNIT_PERCENT, true)) {
        return (100.0f - percent) * (LightMax - LightMin) / 100.0f + LightMin;
    } else {
        return LightMax;
    }
}

static adapter_ops_t sensor_ops = {
    .Attach = Attach,
    .Detach = Detach,
    .Destroy = Destroy,
    .SetPins = StorePins,
    .IsReady = IsValid,
};

static hal_adc_ops_t adc_ops = {
    .read_adc = ReadAdc,
};

adapter_t *Create_EV3Color(int port, const interface_t *iface) {
    ev3_color_t *this = malloc(sizeof(ev3_color_t));
    if (this) {
        this->adc.ops = &adc_ops;
        this->dev.ops = &sensor_ops;
        this->iface   = iface;
        this->port    = port;
        memset(&this->pins, 0, sizeof(this->pins));
        return &this->dev;
    }
    return NULL;
}
