#include <io/adapters/ev3_native.private.h>
#include <hal_general.h>
#include <malloc.h>

#define out_to_wrapper(sns)  container_of(sns, ev3_native_wrapper_t, out)
#define sen_to_wrapper(sns)  container_of(sns, ev3_native_wrapper_t, dev)

static bool Attach(adapter_t *dev) {
    ev3_native_wrapper_t *this = sen_to_wrapper(dev);

    return Hal_Sensor2Host_Attach(&this->out, this->port);
}

static void Detach(adapter_t *dev) {
    ev3_native_wrapper_t *this = sen_to_wrapper(dev);
    Hal_Sensor2Host_Detach(this->port);
}

static void Destroy(adapter_t *dev) {
    ev3_native_wrapper_t *this = sen_to_wrapper(dev);
    free(this);
}

static bool IsValid(adapter_t *dev) {
    ev3_native_wrapper_t *this = sen_to_wrapper(dev);
    return this->iface->Ready(this->port);
}

static adapter_ops_t sensor_ops = {
    .Attach = Attach,
    .Detach = Detach,
    .Destroy = Destroy,
    .IsReady = IsValid,
};

static const char *GetName(hal_sensor2_dev_t *dev) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->GetDeviceName(this->port);
}

static int GetMode(hal_sensor2_dev_t *dev) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.GetMode(this->port);
}

static bool SetMode(hal_sensor2_dev_t *dev, int mode) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.StartSwitch(this->port, mode);
}

static int GetModes(hal_sensor2_dev_t *dev) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.GetModes(this->port);
}

static const char *GetModeName(hal_sensor2_dev_t *dev, int port) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.GetModeName(this->port, port);
}

static const char *GetModeUnits(hal_sensor2_dev_t *dev) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.GetSiUnit(this->port);
}

static int GetModeSlots(hal_sensor2_dev_t *dev) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.GetModes(this->port);
}

static bool Command(hal_sensor2_dev_t *dev, const uint8_t *buffer, uint32_t length) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.StartCommand(this->port, buffer, length);
}

static int Measure(hal_sensor2_dev_t *dev, float *si, float *pct, float *raw, int slots, bool clip) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);

    int real = 0;
    if (si)
        real = this->iface->Sensor.Measure(this->port, si, slots, UNIT_SI, clip);
    if (pct)
        real = this->iface->Sensor.Measure(this->port, pct, slots, UNIT_PERCENT, clip);
    if (raw)
        real = this->iface->Sensor.Measure(this->port, raw, slots, UNIT_RAW, clip);
    return real;
}

static bool ResetDatalog(hal_sensor2_dev_t *dev) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.ResetDatalog(this->port);
}

static int ReadDatalog(hal_sensor2_dev_t *dev, int *pRepeat, float *si, float *pct, float *raw, int slots, bool clip) {
    ev3_native_wrapper_t *this = out_to_wrapper(dev);
    return this->iface->Sensor.ReadDatalog(this->port, pRepeat, si, pct, raw, slots, clip);
}

static hal_sensor2_ops_t ev3_ops = {
    .GetName      = GetName,
    .GetMode      = GetMode,
    .SetMode      = SetMode,
    .GetModes     = GetModes,
    .GetModeName  = GetModeName,
    .GetModeUnits = GetModeUnits,
    .GetModeSlots = GetModeSlots,
    .Command      = Command,
    .Measure      = Measure,
    .ResetDatalog = ResetDatalog,
    .ReadDatalog  = ReadDatalog,
};

adapter_t *Create_EV3Native(int port, const interface_t *iface) {
    ev3_native_wrapper_t *this = malloc(sizeof(ev3_native_wrapper_t));
    if (this) {
        this->out.ops = &ev3_ops;
        this->dev.ops = &sensor_ops;
        this->iface   = iface;
        this->port    = port;
        return &this->dev;
    }
    return NULL;
}
