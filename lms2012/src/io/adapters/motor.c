#include <hal_general.h>
#include <io/core/interface.h>
#include <io/adapters/motor.private.h>
#include <malloc.h>

#define dev_to_motor(x)     container_of(x, motor_device_t, motor)
#define adapter_to_motor(x) container_of(x, motor_device_t, dev)

static bool SetPwm(hal_motor_dev_t *dev, int pwm, motor_stop_t mode) {
    motor_device_t *this = dev_to_motor(dev);
    return this->iface->Motor.SetPwm(this->port, pwm, mode);
}

static bool GetTacho(hal_motor_dev_t *dev, int32_t *pDegrees) {
    motor_device_t *this = dev_to_motor(dev);
    return this->iface->Motor.GetTacho(this->port, pDegrees);
}

static bool ResetTacho(hal_motor_dev_t *dev) {
    motor_device_t *this = dev_to_motor(dev);
    return this->iface->Motor.ResetTacho(this->port);
}

static bool Attach(adapter_t *dev) {
    motor_device_t *this = adapter_to_motor(dev);
    return Hal_MotorHost_Attach(&this->motor, this->port);
}

static void Detach(adapter_t *dev) {
    motor_device_t *this = adapter_to_motor(dev);
    Hal_MotorHost_Detach(this->port);
}

static void Destroy(adapter_t *dev) {
    motor_device_t *this = adapter_to_motor(dev);
    free(this);
}

static hal_motor_ops_t motor_ops = {
    .SetPwm = SetPwm,
    .GetTacho = GetTacho,
    .ResetTacho = ResetTacho,
};

static adapter_ops_t dev_ops = {
    .Attach = Attach,
    .Detach = Detach,
    .Destroy = Destroy,
};

adapter_t *Create_TachoMotor(int port, const interface_t *iface) {
    motor_device_t *dev = malloc(sizeof(motor_device_t));
    if (dev) {
        dev->motor.ops = &motor_ops;
        dev->dev.ops   = &dev_ops;
        dev->iface     = iface;
        dev->port      = port;
        return &dev->dev;
    }
    return NULL;
}
