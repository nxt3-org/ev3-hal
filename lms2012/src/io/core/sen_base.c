#include "io/core/sen_base.h"

bool Sensor_Attach(sensor_dev_t *dev) {
    if (!dev || !dev->ops || !dev->ops->attach)
        return false;
    return dev->ops->attach(dev);
}

bool Sensor_Detach(sensor_dev_t *dev) {
    if (!dev || !dev->ops || !dev->ops->detach)
        return false;
    dev->ops->detach(dev);
    return true;
}

bool Sensor_Destroy(sensor_dev_t *dev) {
    if (!dev || !dev->ops || !dev->ops->destroy)
        return false;
    dev->ops->destroy(dev);
    return true;
}
