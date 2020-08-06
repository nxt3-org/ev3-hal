#include "common/kdevices.h"
#include "io/core/hal_pnp.local.h"
#include "io/driver/if_motor.private.h"

bool Drv_Motor_KernelSetTypes(void) {
    dcm_type_t types[4] = {DCM_DEV_NONE, DCM_DEV_NONE, DCM_DEV_NONE, DCM_DEV_NONE};

    for (int i = 0; i < 4; i++) {
        if (Drv_Motor.types[i])
            types[i] = Drv_Motor.types[i]->Main.Device;
    }

    pwm_req_set_motor_type req = {
        .cmd   = CMD_SET_MOTOR_TYPE,
        .types = {types[0], types[1], types[2], types[3]}
    };
    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_KernelStart(int mPort) {
    pwm_req_output_start req = {
        .cmd     = CMD_OUTPUT_START,
        .mask    = 1 << mPort,
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_KernelStop(int mPort, motor_stop_t stop) {
    pwm_req_output_stop req = {
        .cmd     = CMD_OUTPUT_STOP,
        .mask    = 1 << mPort,
        .doBrake = stop == STOP_MODE_BRAKE
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_KernelStopAll(void) {
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

bool Drv_Motor_KernelSetPolarity(void) {
    pwm_req_set_polarity req2 = {
        .cmd        = CMD_SET_POLARITY,
        .mask       = 0x0F,
        .multiplier = POLARITY_FORWARD
    };
    return Kdev_Write(&DevicePwm, &req2, sizeof(req2), 0) >= 0;
}

bool Drv_Motor_KernelPowerOn(void) {
    pwm_req_program_start req1 = {
        .cmd = CMD_PROGRAM_START
    };
    return Kdev_Write(&DevicePwm, &req1, sizeof(req1), 0) >= 0;
}

bool Drv_Motor_KernelPowerOff(void) {
    pwm_req_program_stop exitReq = {
        .cmd = CMD_PROGRAM_STOP
    };
    return Kdev_Write(&DevicePwm, &exitReq, sizeof(exitReq), 0) >= 0;
}

bool Drv_Motor_KernelTachoReset(int port) {
    pwm_req_reset_tacho req = {
        .cmd   = CMD_RESET_TACHO,
        .mask  = 1 << port,
    };

    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}

bool Drv_Motor_KernelSetPwm(int port, int pwm) {
    pwm_req_set_duty_cycle req = {
        .cmd   = CMD_SET_DUTY_CYCLE,
        .mask  = 1 << port,
        .power = pwm,
    };
    return Kdev_Write(&DevicePwm, &req, sizeof(req), 0) >= 0;
}
