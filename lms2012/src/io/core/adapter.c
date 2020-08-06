#include "io/core/adapter.h"

bool Adapter_Attach(adapter_t *dev) {
    if (!dev || !dev->ops || !dev->ops->Attach)
        return false;
    return dev->ops->Attach(dev);
}

bool Adapter_Detach(adapter_t *dev) {
    if (!dev || !dev->ops || !dev->ops->Detach)
        return false;
    dev->ops->Detach(dev);
    return true;
}

bool Adapter_Destroy(adapter_t *dev) {
    if (!dev || !dev->ops || !dev->ops->Destroy)
        return false;
    dev->ops->Destroy(dev);
    return true;
}

ready_status_t Adapter_IsReady(adapter_t *dev) {
    if (!dev)
        return READY_DEVICE_NOT_PRESENT;
    if (!dev->ops || !dev->ops->IsReady)
        return READY_NOT_SIGNALLED;
    if (dev->ops->IsReady(dev)) {
        return READY_SIGNALLED_YES;
    } else {
        return READY_SIGNALLED_NO;
    }
}

bool Adapter_GetPins(adapter_t *dev, struct hal_pins *pins) {
    if (!dev || !dev->ops || !dev->ops->GetPins)
        return false;
    return dev->ops->GetPins(dev, pins);
}

bool Adapter_SetPins(adapter_t *dev, struct hal_pins pins) {
    if (!dev || !dev->ops || !dev->ops->SetPins)
        return false;
    return dev->ops->SetPins(dev, pins);
}

void Adapter_Tick(adapter_t *dev) {
    if (!dev || !dev->ops || !dev->ops->Tick)
        return;
    return dev->ops->Tick(dev);
}
