#include "hal_motor.private.h"
#include "kdevices.h"

mod_motor_t Mod_Motor;

bool Hal_Motor_RefAdd(void) {
    if (Mod_Motor.refCount > 0) {
        Mod_Motor.refCount++;
        return true;
    }

    for (int port = 0; port < 4; port++) {
        Mod_Motor.ports[port].device    = MOTOR_TYPE_ERROR;
        Mod_Motor.ports[port].stopMode  = STOP_MODE_COAST;
        Mod_Motor.ports[port].dutyCycle = 0;
        Mod_Motor.ports[port].isActive  = false;
    }

    if (!Kdev_RefAdd(&DevicePwm))
        goto fail_pwm;

    if (!Kdev_RefAdd(&DeviceTacho))
        goto fail_tacho;

    if (!Kdev_RefAdd(&DeviceAnalog))
        goto fail_analog;

    pwm_req_program_start req1 = {.cmd = CMD_PROGRAM_START};
    if (Kdev_Write(&DevicePwm, &req1, sizeof(req1), 0) < 0)
        goto fail_end;

    pwm_req_set_polarity req2 = {
        .cmd        = CMD_SET_POLARITY,
        .mask       = 0x0F,
        .multiplier = POLARITY_FORWARD
    };
    if (Kdev_Write(&DevicePwm, &req2, sizeof(req2), 0) < 0)
        goto fail_end;

    Mod_Motor.refCount++;
    Hal_Motor_Tick();
    return true;

fail_end:
    Kdev_RefDel(&DeviceAnalog);
fail_analog:
    Kdev_RefDel(&DeviceTacho);
fail_tacho:
    Kdev_RefDel(&DevicePwm);
fail_pwm:
    return false;
}

bool Hal_Motor_RefDel(void) {
    if (Mod_Motor.refCount == 0)
        return false;
    if (Mod_Motor.refCount == 1) {
        pwm_req_program_stop exitReq = {
            .cmd = CMD_PROGRAM_STOP
        };
        Kdev_Write(&DevicePwm, &exitReq, sizeof(exitReq), 0);

        Kdev_RefDel(&DeviceAnalog);
        Kdev_RefDel(&DeviceTacho);
        Kdev_RefDel(&DevicePwm);
    }
    Mod_Motor.refCount--;
    return true;
}

void Hal_Motor_Tick(void) {
    uint8_t uploadType[4] = { MOTOR_TYPE_NONE, MOTOR_TYPE_NONE, MOTOR_TYPE_NONE, MOTOR_TYPE_NONE };
    uint8_t uploadFlag = 0;

    doScanDCM(uploadType, &uploadFlag);

    if (uploadFlag) {
        pwm_req_set_motor_type req = {
            .cmd   = CMD_SET_MOTOR_TYPE,
            .types = { uploadType[0], uploadType[1], uploadType[2], uploadType[3] }
        };

        Kdev_Write(&DevicePwm, &req, sizeof(req), 0);

        for (int motor = 0; motor < 4; motor++) {
            if ((uploadFlag & (1 << motor)) == 0)
                continue;

            Mod_Motor.ports[motor].changed = true;
            Mod_Motor.ports[motor].stopMode = STOP_MODE_COAST;
            Mod_Motor.ports[motor].isActive = false;
            // restart movement if needed
            Hal_Motor_PushPwm(motor, Mod_Motor.ports[motor].dutyCycle);
        }
    }
}

void doScanDCM(uint8_t *motorTypes, uint8_t *pUploadMask) {
    for (int motor = 0; motor < 4; motor++) {
        uint8_t newConn = DeviceAnalog.mmap->Dcm_InLink[motor];
        uint8_t newType = DeviceAnalog.mmap->Dcm_InType[motor];
        motor_type_t newCombo = MOTOR_TYPE_NONE;

        if (newConn == MOTOR_DCM_CONN_TACHO) {
            if (newType == MOTOR_DCM_TYPE_NXT) {
                newCombo = MOTOR_TYPE_NXT;

            } else if (newType == MOTOR_DCM_TYPE_MEDIUM) {
                newCombo = MOTOR_TYPE_EV3_MEDIUM;

            } else if (newType == MOTOR_DCM_TYPE_LARGE) {
                newCombo = MOTOR_TYPE_EV3_LARGE;
            }
        }

        if (newCombo != MOTOR_TYPE_NONE) {
            motorTypes[motor] = newType;
        }

        if (newCombo != Mod_Motor.ports[motor].device) {
            *pUploadMask |= 1 << motor;
            Mod_Motor.ports[motor].device = newCombo;
        }
    }
}

bool Hal_Motor_SetStopMode(motor_port_t port, motor_stop_t mode) {
    if (Mod_Motor.refCount <= 0)
        return false;
    if (port >= 4 || port < 0)
        return false;
    if (mode != STOP_MODE_BRAKE && mode != STOP_MODE_COAST)
        return false;

    Mod_Motor.ports[port].stopMode = mode;

    if (!Mod_Motor.ports[port].isActive) {
        doStop(port);
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

    Mod_Motor.ports[port].dutyCycle = percent;

    if (percent == 0) {
        if (Mod_Motor.ports[port].isActive)
            return doStop(port);
        return true;
    }

    if (!Mod_Motor.ports[port].isActive) {
        if (!doStart(port))
            return false;
    }

    pwm_req_set_duty_cycle req = {
        .cmd   = CMD_SET_DUTY_CYCLE,
        .mask  = 1 << port,
        .power = percent
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool doStart(motor_port_t port) {
    Mod_Motor.ports[port].isActive = true;
    pwm_req_output_start req       = {
        .cmd     = CMD_OUTPUT_START,
        .mask    = 1 << port,
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool doStop(motor_port_t port) {
    Mod_Motor.ports[port].isActive = false;
    pwm_req_output_stop req        = {
        .cmd     = CMD_OUTPUT_STOP,
        .mask    = 1 << port,
        .doBrake = Mod_Motor.ports[port].stopMode == STOP_MODE_BRAKE
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Hal_Motor_GetTacho(motor_port_t port, int32_t *pDegrees) {
    if (Mod_Motor.refCount <= 0)
        return false;
    if (port >= 4 || port < 0)
        return false;

    if (pDegrees)
        *pDegrees = DeviceTacho.mmap->motors[port].senseDegrees;
    return true;
}

bool Hal_Motor_ResetTacho(motor_port_t port) {
    if (Mod_Motor.refCount <= 0)
        return false;
    if (port >= 4 || port < 0)
        return false;

    pwm_req_reset_tacho req = {
        .cmd   = CMD_RESET_TACHO,
        .mask  = 1 << port,
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

motor_type_t Hal_Motor_Detect(motor_port_t port) {
    if (Mod_Motor.refCount <= 0)
        return MOTOR_TYPE_ERROR;
    if (port >= 4 || port < 0)
        return MOTOR_TYPE_ERROR;

    return Mod_Motor.ports[port].device;
}

bool Hal_Motor_GetChangeMask(uint8_t *pMask) {
    if (Mod_Motor.refCount <= 0)
        return false;

    for (int port = 0; port < 4; port++) {
        if (Mod_Motor.ports[port].changed)
            *pMask |= 1 << port;
    }
}

bool Hal_Motor_ClearChangeMask(uint8_t processed) {
    if (Mod_Motor.refCount <= 0)
        return false;

    for (int port = 0; port < 4; port++) {
        if (processed & (1 << port))
            Mod_Motor.ports[port].changed = false;
    }
}
