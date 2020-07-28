
#include <io/driver/drv_analog.h>

#include "io/driver/drv_analog.private.h"

drv_analog_t Drv_Analog;

port_driver_ops_t DriverAnalog = {
    .Init = Drv_Analog_RefAdd,
    .Exit = Drv_Analog_RefDel,
    .Tick = NULL,
    .SetCallbacks = Drv_Analog_PnpSetCallbacks,
    .DeviceStart = Drv_Analog_PnpStart,
    .DeviceStop = Drv_Analog_PnpStop
};

bool Drv_Analog_RefAdd(void) {
    if (Drv_Analog.refCount > 0) {
        Drv_Analog.refCount++;
        return true;
    }

    if (!Drv_Dcm_RefAdd())
        return false;

    if (!Kdev_RefAdd(&DeviceAnalog)) {
        Drv_Dcm_RefDel();
        return false;
    }

    Drv_Analog.refCount++;
    return true;
}

bool Drv_Analog_RefDel(void) {
    if (Drv_Analog.refCount == 0)
        return false;
    if (Drv_Analog.refCount == 1) {
        Kdev_RefDel(&DeviceAnalog);
        Drv_Dcm_RefDel();
    }
    Drv_Analog.refCount--;
    return true;
}

void Drv_Analog_PnpStart(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev) {
    if ((port & DCM_TYPE_MASK) != DCM_TYPE_INPUT || link != PNP_LINK_ANALOG) {
        Drv_Analog.idCalls->failure(port);
        return;
    }
    uint8_t  sPort = port & DCM_PORT_MASK;
    uint16_t mV    = Drv_Dcm_GetIdPinVoltage(port);
    if (AUTOID_IS(EV3_TOUCH, mV)) {
        Drv_Analog.ports[port].device = PNP_DEVICE_SENSOR_EV3_TOUCH;
        Drv_Dcm_SetupPins(port, 'f');
        Drv_Analog.ports[sPort].rawMin  = 1000.0f;
        Drv_Analog.ports[sPort].rawMax  = 2000.0f;
        Drv_Analog.ports[sPort].pctMin  = 0.0f;
        Drv_Analog.ports[sPort].pctMax  = 100.0f;
        Drv_Analog.ports[sPort].siMin   = 0.0f;
        Drv_Analog.ports[sPort].siMax   = 1.0f;
        Drv_Analog.ports[sPort].present = true;
        Drv_Analog.ports[sPort].pin6    = true;
        Drv_Analog.idCalls->success(port,
                                    PNP_LINK_ANALOG,
                                    PNP_DEVICE_SENSOR_EV3_TOUCH,
                                    PNP_MODE_EV3_TOUCH_PRESS);
        return;
    }
    Drv_Analog.idCalls->failure(port);
}

void Drv_Analog_PnpStop(dcm_port_id_t port) {
    if ((port & DCM_TYPE_MASK) != DCM_TYPE_INPUT)
        return;
    uint8_t sPort = port & DCM_PORT_MASK;
    Drv_Analog.ports[sPort].present = false;
    Drv_Dcm_SetupPins(port, 'f');
}

void Drv_Analog_PnpSetCallbacks(identify_callback_t *id, modeswitch_callback_t *mode) {
    Drv_Analog.idCalls   = id;
    Drv_Analog.modeCalls = mode;
}

bool Drv_Analog_AdcValue(uint8_t sPort, uint16_t *pValue) {
    if (sPort >= 4 || !Drv_Analog.ports[sPort].present)
        return false;

    uint16_t idx = DeviceAnalog.mmap->Buffer_LastPtr[sPort];
    if (Drv_Analog.ports[sPort].pin6)
        *pValue = DeviceAnalog.mmap->Buffer_Pin6_S[sPort][idx];
    else
        *pValue = DeviceAnalog.mmap->Buffer_Pin1_S[sPort][idx];
    return true;
}

bool Drv_Analog_RawRange(uint8_t sPort, float *pValue) {
    uint16_t adc = 0;
    if (!Drv_Analog_AdcValue(sPort, &adc))
        return false;

    float full = (adc - Drv_Analog.ports[sPort].rawMin) /
                 (Drv_Analog.ports[sPort].rawMax - Drv_Analog.ports[sPort].rawMin);
    if (full > 1.0f)
        *pValue = 1.0f;
    else if (full < 0.0f)
        *pValue = 0.0f;
    else
        *pValue = full;

    return true;
}

bool Drv_Analog_PctValue(uint8_t sPort, float *pValue) {
    float range = 0.0f;
    if (!Drv_Analog_RawRange(sPort, &range))
        return false;

    *pValue = range * (Drv_Analog.ports[sPort].pctMax - Drv_Analog.ports[sPort].pctMin) +
              Drv_Analog.ports[sPort].pctMin;

    return true;
}

bool Drv_Analog_SiValue(uint8_t sPort, float *pValue) {
    float range = 0.0f;
    if (!Drv_Analog_RawRange(sPort, &range))
        return false;

    *pValue = range * (Drv_Analog.ports[sPort].siMax - Drv_Analog.ports[sPort].siMin) +
              Drv_Analog.ports[sPort].siMin;

    return true;
}
