#include "io/hal/hal_motor.private.h"
#include "io/driver/drv_motor.h"

mod_motor_t Mod_Motor;

bool Hal_Motor_RefAdd(void) {
    if (Mod_Motor.refCount > 0) {
        Mod_Motor.refCount++;
        return true;
    }

    for (int port = 0; port < 4; port++) {
        Mod_Motor.ports[port].stopMode  = STOP_MODE_COAST;
        Mod_Motor.ports[port].dutyCycle = 0;
        Mod_Motor.ports[port].isActive  = false;
        Mod_Motor.ports[port].isPresent = false;
    }

    if (!Drv_Motor_RefAdd())
        return false;

    Mod_Motor.refCount++;
    return true;
}

bool Hal_Motor_RefDel(void) {
    if (Mod_Motor.refCount == 0)
        return false;
    if (Mod_Motor.refCount == 1) {
        Drv_Motor_RefDel();
    }
    Mod_Motor.refCount--;
    return true;
}

bool Hal_Motor_SetStopMode(motor_port_t port, motor_stop_t mode) {
    if (Mod_Motor.refCount <= 0)
        return false;
    if (port >= 4 || port < 0)
        return false;
    if (mode != STOP_MODE_BRAKE && mode != STOP_MODE_COAST)
        return false;

    Mod_Motor.ports[port].stopMode = mode;

    if (Mod_Motor.ports[port].isPresent && !Mod_Motor.ports[port].isActive) {
        return Drv_Motor_SetPwm(port, mode, 0, true);
    }
    return true;
}

bool Hal_Motor_PushPwm(motor_port_t port, int8_t percent) {
    if (Mod_Motor.refCount <= 0)
        return false;
    if (port >= 4 || port < 0)
        return false;

    if (percent < -100)
        percent = -100;
    if (percent > +100)
        percent = +100;

    motor_stop_t stop = Mod_Motor.ports[port].stopMode;
    bool wasActive    = Mod_Motor.ports[port].isActive;

    if (!Mod_Motor.ports[port].isPresent) {
        Mod_Motor.ports[port].dutyCycle = percent;
        return true;
    }

    if (true == Drv_Motor_SetPwm(port, stop, percent, wasActive)) {
        Mod_Motor.ports[port].dutyCycle = percent;
        Mod_Motor.ports[port].isActive  = percent != 0;
        return true;
    }
    return false;
}

bool Hal_Motor_GetTacho(motor_port_t port, int32_t *pDegrees) {
    if (Mod_Motor.refCount <= 0)
        return false;
    if (port >= 4 || port < 0)
        return false;
    if (!Mod_Motor.ports[port].isPresent)
        return false;

    return Drv_Motor_TachoGet(port, pDegrees);
}

bool Hal_Motor_ResetTacho(motor_port_t port) {
    if (Mod_Motor.refCount <= 0)
        return false;
    if (port >= 4 || port < 0)
        return false;
    if (!Mod_Motor.ports[port].isPresent)
        return false;
    return Drv_Motor_TachoReset(port);
}

bool Hal_Motor_DeviceAttached(uint8_t mPort, pnp_link_t link, pnp_device_t device, uint8_t hwMode) {
    Mod_Motor.ports[mPort].isPresent = true;
    Hal_Motor_PushPwm(mPort, Mod_Motor.ports[mPort].dutyCycle);
    return true;
}

bool Hal_Motor_DeviceDetached(uint8_t mPort) {
    Mod_Motor.ports[mPort].isPresent = false;
    return true;
}
