#include "hal_motor.h"

bool Hal_MotorDev_SetPwm(hal_motor_dev_t *dev, int percent, motor_stop_t mode) {
    if (!dev || !dev->ops->SetPwm)
        return false;
    return dev->ops->SetPwm(dev, percent, mode);
}

bool Hal_MotorDev_GetTacho(hal_motor_dev_t *dev, int *pDegrees) {
    if (!dev || !dev->ops->GetTacho)
        return false;
    return dev->ops->GetTacho(dev, pDegrees);
}

bool Hal_MotorDev_ResetTacho(hal_motor_dev_t *dev) {
    if (!dev || !dev->ops->ResetTacho)
        return false;
    return dev->ops->ResetTacho(dev);
}

