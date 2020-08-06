#include <hal_ev3sensor.h>
#include <stddef.h>

const char *Hal_Sensor2_GetName(hal_sensor2_dev_t *dev) {
    if (!dev || !dev->ops->GetName)
        return NULL;
    return dev->ops->GetName(dev);
}

int Hal_Sensor2_GetMode(hal_sensor2_dev_t *dev) {
    if (!dev || !dev->ops->GetMode)
        return -1;
    return dev->ops->GetMode(dev);
}

bool Hal_Sensor2_SetMode(hal_sensor2_dev_t *dev, int mode) {
    if (!dev || !dev->ops->SetMode)
        return false;
    return dev->ops->SetMode(dev, mode);
}

int Hal_Sensor2_GetModes(hal_sensor2_dev_t *dev) {
    if (!dev || !dev->ops->GetModes)
        return -1;
    return dev->ops->GetModes(dev);
}

const char *Hal_Sensor2_GetModeName(hal_sensor2_dev_t *dev, int mode) {
    if (!dev || !dev->ops->GetModeName)
        return NULL;
    return dev->ops->GetModeName(dev, mode);
}

const char *Hal_Sensor2_GetModeUnits(hal_sensor2_dev_t *dev) {
    if (!dev || !dev->ops->GetModeUnits)
        return NULL;
    return dev->ops->GetModeUnits(dev);
}

int Hal_Sensor2_GetModeSlots(hal_sensor2_dev_t *dev) {
    if (!dev || !dev->ops->GetModeSlots)
        return -1;
    return dev->ops->GetModeSlots(dev);
}

bool Hal_Sensor2_Command(hal_sensor2_dev_t *dev, const uint8_t *buffer, uint32_t length) {
    if (!dev || !dev->ops->Command)
        return false;
    return dev->ops->Command(dev, buffer, length);
}

int Hal_Sensor2_Measure(hal_sensor2_dev_t *dev, float *si, float *pct, float *raw, int slots, bool clip) {
    if (!dev || !dev->ops->Measure)
        return -1;
    return dev->ops->Measure(dev, si, pct, raw, slots, clip);
}

bool Hal_Sensor2_ResetDatalog(hal_sensor2_dev_t *dev) {
    if (!dev || !dev->ops->ResetDatalog)
        return false;
    return dev->ops->ResetDatalog(dev);
}

int Hal_Sensor2_ReadDatalog(hal_sensor2_dev_t *dev,
                             int *pRepeat, float *si, float *pct, float *raw,
                             int slots, bool clip) {
    if (!dev || !dev->ops->ReadDatalog)
        return -1;
    return dev->ops->ReadDatalog(dev, pRepeat, si, pct, raw, slots, clip);
}
