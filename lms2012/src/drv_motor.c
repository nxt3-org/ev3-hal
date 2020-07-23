#include <memory.h>
#include "drv_motor.h"
#include "drv_motor.private.h"

drv_motor_t Drv_Motor;

bool Drv_Motor_RefAdd(void) {
    if (Drv_Motor.refCount > 0) {
        Drv_Motor.refCount++;
        return true;
    }

    for (int port = 0; port < 4; port++) {
        Drv_Motor.types[port] = DCM_DEV_NONE;
    }

    if (!Kdev_RefAdd(&DevicePwm))
        goto fail_pwm;

    if (!Kdev_RefAdd(&DeviceTacho))
        goto fail_tacho;

    if (!Drv_Motor_PowerOn())
        goto fail_end;

    if (!Drv_Motor_SetPolarity()) // beware! this activates the motors -> dcm breaks
        goto fail_end;

    if (!Drv_Motor_StopAll()) // this shuts them down again -> dcm works again
        goto fail_end;

    if (!Drv_Motor_SetTypes())
        goto fail_end;

    Drv_Motor.idCalls = NULL;

    Drv_Motor.refCount++;
    return true;

fail_end:
    Kdev_RefDel(&DeviceAnalog);
fail_tacho:
    Kdev_RefDel(&DevicePwm);
fail_pwm:
    return false;
}

bool Drv_Motor_RefDel(void) {
    if (Drv_Motor.refCount == 0)
        return false;
    if (Drv_Motor.refCount == 1) {
        Drv_Motor_StopAll();
        Drv_Motor_PowerOff();
        Kdev_RefDel(&DeviceTacho);
        Kdev_RefDel(&DevicePwm);
    }
    Drv_Motor.refCount--;
    return true;
}

bool Drv_Motor_SetType(uint8_t mPort, pnp_device_t type) {
    dcm_dev_t device;
    if (type == PNP_DEVICE_MOTOR_LARGE)
        device = DCM_DEV_LARGE_MOTOR;
    else if (type == PNP_DEVICE_MOTOR_MEDIUM)
        device = DCM_DEV_MEDIUM_MOTOR;
    else
        device = DCM_DEV_NONE;

    Drv_Motor.types[mPort] = device;
    return Drv_Motor_SetTypes();
}

bool Drv_Motor_SetTypes(void) {
    pwm_req_set_motor_type req = {
        .cmd   = CMD_SET_MOTOR_TYPE,
        .types = {Drv_Motor.types[0],
                  Drv_Motor.types[1],
                  Drv_Motor.types[2],
                  Drv_Motor.types[3]}
    };
    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_SetPwm(uint8_t mPort, motor_stop_t stop, int8_t pwm, bool wasRunning) {
    if (pwm == 0) {
        if (wasRunning)
            return Drv_Motor_Stop(mPort, stop);
        return true;
    }

    if (!wasRunning) {
        if (!Drv_Motor_Start(mPort))
            return false;
    }

    pwm_req_set_duty_cycle req = {
        .cmd   = CMD_SET_DUTY_CYCLE,
        .mask  = 1 << mPort,
        .power = pwm,
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_Start(uint8_t mPort) {
    pwm_req_output_start req = {
        .cmd     = CMD_OUTPUT_START,
        .mask    = 1 << mPort,
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_Stop(uint8_t mPort, motor_stop_t stop) {
    pwm_req_output_stop req = {
        .cmd     = CMD_OUTPUT_STOP,
        .mask    = 1 << mPort,
        .doBrake = stop == STOP_MODE_BRAKE
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_TachoReset(uint8_t mPort) {
    pwm_req_reset_tacho req = {
        .cmd   = CMD_RESET_TACHO,
        .mask  = 1 << mPort,
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_TachoGet(uint8_t mPort, int32_t *pTacho) {
    *pTacho = DeviceTacho.mmap->motors[mPort].senseDegrees;
    return true;
}

bool Drv_Motor_StopAll(void) {
    pwm_req_set_duty_cycle req1 = {
        .cmd   = CMD_SET_DUTY_CYCLE,
        .mask  = 0x0F,
        .power = 0,
    };
    if (Kdev_Write(&DevicePwm, &req1, sizeof(req1), 0) < 0)
        return false;

    pwm_req_output_stop req2 = {
        .cmd        = CMD_OUTPUT_STOP,
        .mask       = 0x0F,
        .doBrake    = false
    };
    return Kdev_Write(&DevicePwm, &req2, sizeof(req2), 0) >= 0;
}

bool Drv_Motor_SetPolarity(void) {
    pwm_req_set_polarity req2 = {
        .cmd        = CMD_SET_POLARITY,
        .mask       = 0x0F,
        .multiplier = POLARITY_FORWARD
    };
    return Kdev_Write(&DevicePwm, &req2, sizeof(req2), 0) >= 0;
}

bool Drv_Motor_PowerOn(void) {
    pwm_req_program_start req1 = {
        .cmd = CMD_PROGRAM_START
    };
    return Kdev_Write(&DevicePwm, &req1, sizeof(req1), 0) >= 0;
}

bool Drv_Motor_PowerOff(void) {
    pwm_req_program_stop exitReq = {
        .cmd = CMD_PROGRAM_STOP
    };
    return Kdev_Write(&DevicePwm, &exitReq, sizeof(exitReq), 0) >= 0;
}

void Drv_Motor_PnpStart(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev) {
    if ((port & DCM_TYPE_MASK) != DCM_TYPE_OUTPUT) {
        Drv_Motor.idCalls->failure(port);
        return;
    }
    uint8_t mPort = port & DCM_PORT_MASK;
    Drv_Motor_SetType(mPort, dev);
    if (Drv_Motor.types[mPort] != DCM_DEV_NONE) {
        Drv_Motor.idCalls->success(port, link, dev, PNP_MODE_MOTOR_PWM);
    } else {
        Drv_Motor.idCalls->failure(port);
    }
}

void Drv_Motor_PnpStop(dcm_port_id_t port) {
    if ((port & DCM_TYPE_MASK) != DCM_TYPE_OUTPUT)
        return;

    uint8_t mPort = port & DCM_PORT_MASK;
    Drv_Motor_Stop(mPort, STOP_MODE_COAST);
    Drv_Motor_SetType(mPort, PNP_DEVICE_NONE);
}

void Drv_Motor_PnpSetCallbacks(identify_callback_t *id, modeswitch_callback_t *mode) {
    Drv_Motor.idCalls = id;
    (void) mode;
}

port_driver_ops_t DriverMotor = {
    .Init = Drv_Motor_RefAdd,
    .Exit = Drv_Motor_RefDel,
    .Tick = NULL,
    .SetCallbacks = Drv_Motor_PnpSetCallbacks,
    .DeviceStart = Drv_Motor_PnpStart,
    .DeviceStop = Drv_Motor_PnpStop,
};
