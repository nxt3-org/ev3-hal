#include <io/core/interface.h>
#include "common/kdevices.h"
#include "io/core/hal_pnp.local.h"
#include "io/driver/if_motor.private.h"

drv_motor_t Drv_Motor;

static bool RefAdd(void) {
    if (Drv_Motor.refCount > 0) {
        Drv_Motor.refCount++;
        return true;
    }

    for (int port = 0; port < 4; port++) {
        Drv_Motor.types[port]   = NULL;
        Drv_Motor.running[port] = false;
    }

    if (!Kdev_RefAdd(&DevicePwm))
        goto fail_pwm;

    if (!Kdev_RefAdd(&DeviceTacho))
        goto fail_tacho;

    if (!Drv_Motor_KernelPowerOn())
        goto fail_end;
    if (!Drv_Motor_KernelSetPolarity()) // beware! this activates the motors -> dcm breaks
        goto fail_end;
    if (!Drv_Motor_KernelStopAll()) // this shuts them down again -> dcm works again
        goto fail_end;
    if (!Drv_Motor_KernelSetTypes())
        goto fail_end;

    Drv_Motor.refCount++;
    return true;

fail_end:
    Kdev_RefDel(&DeviceAnalog);
fail_tacho:
    Kdev_RefDel(&DevicePwm);
fail_pwm:
    return false;
}

static bool RefDel(void) {
    if (Drv_Motor.refCount == 0)
        return false;
    if (Drv_Motor.refCount == 1) {
        Drv_Motor_KernelStopAll();
        Drv_Motor_KernelPowerOff();
        Kdev_RefDel(&DeviceTacho);
        Kdev_RefDel(&DevicePwm);
    }
    Drv_Motor.refCount--;
    return true;
}

static bool PnpStart(int port, dcm_link_t link, dcm_type_t device) {
    if (port >= 4 || link != DCM_LINK_OUTPUT_TACHO)
        return false;

    if (device == DCM_DEV_LARGE_MOTOR_2)
        device = DCM_DEV_LARGE_MOTOR;

    const typedb_entry_t *typeDb = HwDb_FindDefaultForType(link, device);
    if (!typeDb)
        return false;

    Drv_Motor.types[port]   = typeDb;
    Drv_Motor.running[port] = false;
    Drv_Motor_KernelSetTypes();
    Hal_Pnp_HandshakeFinished(port, true, IdentifyDevice(typeDb->Main.Device));
    return true;
}

static void PnpStop(int port) {
    if (port >= 4) return;

    Drv_Motor_KernelStop(port, STOP_MODE_COAST);
    Drv_Motor.types[port]   = NULL;
    Drv_Motor.running[port] = false;
    Drv_Motor_KernelSetTypes();
}

static bool Present(int port) {
    if (port >= 4) return false;

    return Drv_Motor.types[port] != NULL;
}

static bool SetPwm(int port, int pwm, motor_stop_t stop) {
    if (!Present(port)) return false;

    if (pwm > +100) pwm = +100;
    if (pwm < -100) pwm = -100;

    bool was = Drv_Motor.running[port];
    bool is  = pwm != 0;
    Drv_Motor.running[port] = is;

    if (!is) {
        if (was)
            return Drv_Motor_KernelStop(port, stop);
        return true;
    }

    if (!was) {
        if (!Drv_Motor_KernelStart(port))
            return false;
    }

    return Drv_Motor_KernelSetPwm(port, pwm);
}

static bool TachoGet(int port, int *pTacho) {
    if (!Present(port)) return false;

    *pTacho = DeviceTacho.mmap->motors[port].senseDegrees;
    return true;
}

static bool TachoReset(int port) {
    if (!Present(port)) return false;

    return Drv_Motor_KernelTachoReset(port);
}

static pnp_type_t GetDevice(int port) {
    if (!Present(port)) return PNP_DEVICE_NONE;

    return IdentifyDevice(Drv_Motor.types[port]->Main.Device);
}

static const char *GetDeviceName(int port) {
    if (!Present(port)) return NULL;

    return Drv_Motor.types[port]->Main.Name;
}


interface_t DriverMotor = {
    .Init = RefAdd,
    .Exit = RefDel,
    .Tick = NULL,

    .Present = Present,
    .Ready = Present,

    .GetDevice = GetDevice,
    .GetDeviceName = GetDeviceName,

    .Start = PnpStart,
    .Stop = PnpStop,

    .Sensor = {},
    .Motor = {
        .SetPwm = SetPwm,
        .GetTacho = TachoGet,
        .ResetTacho = TachoReset,
    },
};
