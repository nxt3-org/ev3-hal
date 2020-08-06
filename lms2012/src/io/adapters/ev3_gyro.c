#include "io/adapters/ev3_gyro.private.h"
#include "hal_general.h"
#include <malloc.h>
#include <memory.h>

#define adc_to_gyro(sns)  container_of(sns, ev3_gyro_t, adc)
#define sen_to_gyro(sns)  container_of(sns, ev3_gyro_t, dev)

static bool Attach(adapter_t *dev) {
    ev3_gyro_t *this = sen_to_gyro(dev);

    this->iface->Sensor.StartSwitch(this->port, PNP_MODE_EV3_GYRO_RATE);
    return Hal_AdcHost_Attach(&this->adc, this->port);
}

static void Detach(adapter_t *dev) {
    ev3_gyro_t *this = sen_to_gyro(dev);
    Hal_AdcHost_Detach(this->port);
}

static void Destroy(adapter_t *dev) {
    ev3_gyro_t *this = sen_to_gyro(dev);
    free(this);
}

static bool IsValid(adapter_t *dev) {
    ev3_gyro_t *this = sen_to_gyro(dev);
    return this->iface->Ready(this->port);
}

static uint16_t ReadAdc(hal_adc_dev_t *dev) {
    ev3_gyro_t *this = adc_to_gyro(dev);

    float rate;
    if (!this->iface->Sensor.Measure(this->port, &rate, 1, UNIT_SI, false))
        rate = 0.0f;

    float result = 600.0f + rate;
    if (result > 1023.0f)
        return 1023;
    else if (result < 0.0f)
        return 0;
    else
        return (uint16_t) result;
}

static adapter_ops_t sensor_ops = {
    .Attach = Attach,
    .Detach = Detach,
    .Destroy = Destroy,
    .IsReady = IsValid,
};

static hal_adc_ops_t adc_ops = {
    .read_adc = ReadAdc,
};

adapter_t *Create_EV3Gyro(int port, const interface_t *iface) {
    ev3_gyro_t *this = malloc(sizeof(ev3_gyro_t));
    if (this) {
        this->adc.ops = &adc_ops;
        this->dev.ops = &sensor_ops;
        this->iface   = iface;
        this->port    = port;
        return &this->dev;
    }
    return NULL;
}
