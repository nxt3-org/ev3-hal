#include <io/driver/drv_dcm.h>
#include <hal_general.h>
#include <io/driver/drv_port.h>
#include <io/driver/drv_motor.h>
#include <io/hal/hal_motor.private.h>
#include <io/driver/drv_dummy.h>
#include "io/core/hal_pnp.private.h"
#include <stdio.h>
#include <io/driver/drv_analog.h>
#include <io/hal/sen_ev3_touch.h>
#include <io/driver/drv_uart.h>
#include <io/hal/sen_ev3_us.h>
#include <io/hal/sen_ev3_color.h>
#include <io/hal/sen_tty.h>
#include <hal_pnp.h>


mod_pnp_t Mod_Pnp;

static dcm_callback_t DcmCallback = {
    .linkFound = dcmLinkFound,
    .linkLost = dcmLinkLost,
};

static identify_callback_t IdCallback = {
    .success = portHandshakeSuccess,
    .failure = portHandshakeFailure,
};

static modeswitch_callback_t ModeCallback = {
    .started = switchStarted,
    .finished = switchFinished,
};

static port_driver_ops_t *drivers[PNP_LINK_COUNT] = {
    [PNP_LINK_MOTOR]    = &DriverMotor,
    [PNP_LINK_UART]     = &DriverUart,
    [PNP_LINK_IIC]      = &DriverDummy,
    [PNP_LINK_ANALOG]   = &DriverAnalog,
    [PNP_LINK_NXTCOLOR] = &DriverDummy,
};

bool Hal_Pnp_RefAdd(void) {
    if (Mod_Pnp.refCount > 0) {
        Mod_Pnp.refCount++;
        return true;
    }
    if (!Drv_Dcm_RefAdd())
        return false;
    Drv_Dcm_SetCallback(&DcmCallback);

    int drv = 0;
    for (; drv < PNP_LINK_COUNT; drv++) {
        if (!drivers[drv]->Init())
            goto cleanup;
        drivers[drv]->SetCallbacks(&IdCallback, &ModeCallback);
    }

    for (int i = 0; i < 8; i++) {
        Mod_Pnp.ports[i].link     = PNP_LINK_NONE;
        Mod_Pnp.ports[i].device   = PNP_DEVICE_NONE;
        Mod_Pnp.ports[i].hwMode   = 0;
        Mod_Pnp.ports[i].emulMode = 0;
        Mod_Pnp.ports[i].state    = PNP_STATE_OFF;
    }

    Mod_Pnp.refCount++;
    return true;

cleanup:
    drv--;
    for (; drv >= 0; drv--) {
        drivers[drv]->Exit();
    }
    Drv_Dcm_RefDel();
    return false;
}

bool Hal_Pnp_RefDel(void) {
    if (Mod_Pnp.refCount == 0)
        return false;
    if (Mod_Pnp.refCount == 1) {
        for (int drv = 0; drv < PNP_LINK_COUNT; drv++) {
            if (!drivers[drv]->Exit())
                Hal_General_AbnormalExit("ERROR: cannot deinitialize one of sensor links");
        }

        if (!Drv_Dcm_RefAdd())
            Hal_General_AbnormalExit("ERROR: cannot deinitialize DCM");
    }
    Mod_Pnp.refCount--;
    return true;
}

void Hal_Pnp_Tick(void) {
    Drv_Dcm_Tick();
    for (int drv = 0; drv < PNP_LINK_COUNT; drv++) {
        if (drivers[drv]->Tick)
            drivers[drv]->Tick();
    }
}

bool Hal_Pnp_GetLink(uint8_t port, bool output, pnp_link_t *pLink) {
    if (port >= 4)
        return false;

    int dcmPort = port | (output ? DCM_TYPE_OUTPUT : DCM_TYPE_INPUT);
    *pLink = Mod_Pnp.ports[dcmPort].link;
    return true;
}

bool Hal_Pnp_GetDevice(uint8_t port, bool output, pnp_device_t *pDevice) {
    if (port >= 4)
        return false;

    int dcmPort = port | (output ? DCM_TYPE_OUTPUT : DCM_TYPE_INPUT);
    *pDevice = Mod_Pnp.ports[dcmPort].device;
    return true;
}

bool Hal_Pnp_GetHwMode(uint8_t port, bool output, uint8_t *pMode) {
    if (port >= 4)
        return false;

    int dcmPort = port | (output ? DCM_TYPE_OUTPUT : DCM_TYPE_INPUT);
    if (Mod_Pnp.ports[dcmPort].state == PNP_STATE_OFF ||
        Mod_Pnp.ports[dcmPort].state == PNP_STATE_HANDSHAKING)
        return false;

    *pMode = Mod_Pnp.ports[dcmPort].hwMode;
    return true;
}

bool Hal_Pnp_GetEmulatedMode(uint8_t port, bool output, uint8_t *pMode) {
    if (port >= 4)
        return false;

    int dcmPort = port | (output ? DCM_TYPE_OUTPUT : DCM_TYPE_INPUT);
    if (Mod_Pnp.ports[dcmPort].state == PNP_STATE_OFF ||
        Mod_Pnp.ports[dcmPort].state == PNP_STATE_HANDSHAKING)
        return false;

    *pMode = Mod_Pnp.ports[dcmPort].emulMode;
    return true;
}

bool Hal_Pnp_IsReady(uint8_t port, bool output) {
    if (port >= 4)
        return false;

    int dcmPort = port | (output ? DCM_TYPE_OUTPUT : DCM_TYPE_INPUT);
    return Mod_Pnp.ports[dcmPort].state == PNP_STATE_RUNNING;
}

bool Hal_Pnp_IsSwitching(uint8_t port, bool output) {
    if (port >= 4)
        return false;

    int dcmPort = port | (output ? DCM_TYPE_OUTPUT : DCM_TYPE_INPUT);
    return Mod_Pnp.ports[dcmPort].state == PNP_STATE_SWITCHING;
}

void dcmLinkFound(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev) {
    Mod_Pnp.ports[port].link     = link;
    Mod_Pnp.ports[port].device   = PNP_DEVICE_UNKNOWN;
    Mod_Pnp.ports[port].hwMode   = 0;
    Mod_Pnp.ports[port].emulMode = 0;
    Mod_Pnp.ports[port].state    = PNP_STATE_HANDSHAKING;
    drivers[link]->DeviceStart(port, link, dev);
}

void dcmLinkLost(dcm_port_id_t port) {
    if (Mod_Pnp.ports[port].state != PNP_STATE_OFF) {
        stopEmulation(port);
        drivers[Mod_Pnp.ports[port].link]->DeviceStop(port);
        Mod_Pnp.ports[port].link     = PNP_LINK_NONE;
        Mod_Pnp.ports[port].device   = PNP_DEVICE_NONE;
        Mod_Pnp.ports[port].hwMode   = 0;
        Mod_Pnp.ports[port].emulMode = 0;
        Mod_Pnp.ports[port].state    = PNP_STATE_OFF;
    }
}

void portHandshakeSuccess(dcm_port_id_t port, pnp_link_t link, pnp_device_t device, uint8_t hwMode) {
    Mod_Pnp.ports[port].state  = PNP_STATE_RUNNING;
    Mod_Pnp.ports[port].link   = link;
    Mod_Pnp.ports[port].device = device;
    Mod_Pnp.ports[port].hwMode = hwMode;
    startEmulation(port);
}

void portHandshakeFailure(dcm_port_id_t port) {
    drivers[Mod_Pnp.ports[port].link]->DeviceStop(port);
    Mod_Pnp.ports[port].state  = PNP_STATE_OFF;
    Mod_Pnp.ports[port].link   = PNP_LINK_NONE;
    Mod_Pnp.ports[port].device = PNP_DEVICE_NONE;
}

void switchStarted(dcm_port_id_t port) {
    Mod_Pnp.ports[port].state = PNP_STATE_SWITCHING;
}

void switchFinished(dcm_port_id_t port, uint8_t hwMode) {
    Mod_Pnp.ports[port].state  = PNP_STATE_RUNNING;
    Mod_Pnp.ports[port].hwMode = hwMode;
}

void startEmulation(dcm_port_id_t port) {
    sensor_dev_t *sen = NULL;
    if (Mod_Pnp.ports[port].link == PNP_LINK_MOTOR) {
        Hal_Motor_DeviceAttached(port & DCM_PORT_MASK,
                                 Mod_Pnp.ports[port].link,
                                 Mod_Pnp.ports[port].device,
                                 Mod_Pnp.ports[port].hwMode);
        return;
    }
    if (Mod_Pnp.ports[port].device == PNP_DEVICE_SENSOR_EV3_TOUCH)
        sen = Sensor_EV3Touch_Create(port & DCM_PORT_MASK);
    if (Mod_Pnp.ports[port].device == PNP_DEVICE_SENSOR_EV3_SONIC)
        sen = Sensor_EV3Sonic_Create(port & DCM_PORT_MASK);
    if (Mod_Pnp.ports[port].device == PNP_DEVICE_SENSOR_EV3_COLOR)
        sen = Sensor_EV3Color_Create(port & DCM_PORT_MASK);
    if (Mod_Pnp.ports[port].device == PNP_DEVICE_TTY)
        sen = Sensor_TTY_Create(port & DCM_PORT_MASK);

    if (sen != NULL) {
        Mod_Pnp.ports[port].sensor = sen;
        Sensor_Attach(sen);
    }
}

void stopEmulation(dcm_port_id_t port) {
    if (Mod_Pnp.ports[port].link == PNP_LINK_MOTOR) {
        Hal_Motor_DeviceDetached(port & DCM_PORT_MASK);

    } else {
        sensor_dev_t *sen = Mod_Pnp.ports[port].sensor;
        if (sen) {
            Sensor_Detach(sen);
            Sensor_Destroy(sen);
        }
    }
}
