#include <hal_general.h>
#include <io/driver/drv_dcm.h>

#include "io/driver/drv_dcm.private.h"

drv_dcm_t Drv_Dcm;

bool Drv_Dcm_RefAdd(void) {
    if (Drv_Dcm.refCount > 0) {
        Drv_Dcm.refCount++;
        return true;
    }
    if (!Kdev_RefAdd(&DeviceAnalog))
        return false;
    if (!Kdev_RefAdd(&DeviceConnMgr))
        return false;

    for (int no = 0; no < 8; no++) {
        Drv_Dcm.ports[no].lastDevice = DCM_DEV_NONE;
        Drv_Dcm.ports[no].lastLink   = DCM_LINK_UNKNOWN;
    }
    Drv_Dcm.mgr = NULL;
    Drv_Dcm.refCount++;
    return true;
}

bool Drv_Dcm_RefDel(void) {
    if (Drv_Dcm.refCount == 0)
        return false;
    if (Drv_Dcm.refCount == 1) {
        if (!Kdev_RefDel(&DeviceConnMgr))
            Hal_General_AbnormalExit("ERROR: refcounting error on dcm device");
        if (!Kdev_RefDel(&DeviceAnalog))
            Hal_General_AbnormalExit("ERROR: refcounting error on analog device");
    }
    Drv_Dcm.refCount--;
    return true;
}

void Drv_Dcm_SetCallback(dcm_callback_t *mgr) {
        Drv_Dcm.mgr = mgr;
}

void Drv_Dcm_Tick() {
    for (int no = 0; no < 8; no++) {
        dcm_link_t newDcmLink = getNewLink(no);
        dcm_dev_t  newDcmDev  = getNewDevice(no);
        dcm_link_t oldDcmLink = getLastLink(no);

        pnp_link_t   newPnpLink = dcm2pnp_link(newDcmLink);
        pnp_device_t newPnpDev  = dcm2pnp_dev(newDcmDev);
        pnp_link_t   oldPnpLink = dcm2pnp_link(oldDcmLink);

        Drv_Dcm.ports[no].lastLink   = newDcmLink;
        Drv_Dcm.ports[no].lastDevice = newDcmDev;

        // no transition -> not interesting
        if (newPnpLink == oldPnpLink)
            continue;

        if (oldPnpLink != PNP_LINK_NONE)
            Drv_Dcm.mgr->linkLost(no);
        if (newPnpLink != PNP_LINK_NONE)
            Drv_Dcm.mgr->linkFound(no, newPnpLink, newPnpDev);
    }
}

bool Drv_Dcm_SetupPins(dcm_port_id_t port, char code) {
    if ((port & DCM_TYPE_MASK) == DCM_TYPE_OUTPUT)
        return false;

    int pNum = port & DCM_PORT_MASK;
    if (pNum >= 4)
        return false;

    char string[] = "----";
    string[pNum] = code;

    return Kdev_Write(&DeviceConnMgr, string, sizeof(string), 0) >= 0;
}

uint16_t Drv_Dcm_GetIdPinVoltage(dcm_port_id_t port) {
    uint8_t portN = port & DCM_PORT_MASK;
    if ((port & DCM_TYPE_MASK) == DCM_TYPE_INPUT) {
        return ADC_VOLTS(DeviceAnalog.mmap->Adc_Pin1_S[portN]);
    } else {
        return ADC_VOLTS(DeviceAnalog.mmap->Adc_Pin5_M[portN]);
    }
}

dcm_link_t getNewLink(dcm_port_id_t port) {
    if ((port & DCM_TYPE_MASK) == DCM_TYPE_INPUT)
        return DeviceAnalog.mmap->Dcm_InLink[port & DCM_PORT_MASK];
    else
        return DeviceAnalog.mmap->Dcm_OutLink[port & DCM_PORT_MASK];
}

dcm_dev_t getNewDevice(dcm_port_id_t port) {
    if ((port & DCM_TYPE_MASK) == DCM_TYPE_INPUT)
        return DeviceAnalog.mmap->Dcm_InType[port & DCM_PORT_MASK];
    else
        return DeviceAnalog.mmap->Dcm_OutType[port & DCM_PORT_MASK];
}

dcm_link_t getLastLink(dcm_port_id_t port) {
    return Drv_Dcm.ports[port].lastLink;
}

dcm_dev_t getLastDevice(dcm_port_id_t port) {
    return Drv_Dcm.ports[port].lastDevice;
}

pnp_link_t dcm2pnp_link(dcm_link_t dcm) {
    switch (dcm) {

    case DCM_LINK_NXT_COLOR:
        return PNP_LINK_NXTCOLOR;

    case DCM_LINK_NXT_DUMB:
    case DCM_LINK_INPUT_DUMB:
        return PNP_LINK_ANALOG;

    case DCM_LINK_NXT_IIC:
        return PNP_LINK_IIC;

    case DCM_LINK_INPUT_UART:
        return PNP_LINK_UART;

    case DCM_LINK_OUTPUT_TACHO:
        return PNP_LINK_MOTOR;

    default:
    case DCM_LINK_UNKNOWN:
    case DCM_LINK_NONE:
    case DCM_LINK_ERROR:
    case DCM_LINK_OUTPUT_DUMB:
    case DCM_LINK_OUTPUT_SMART:
        return PNP_LINK_NONE;
    }
}

pnp_device_t dcm2pnp_dev(dcm_dev_t dcm) {
    switch (dcm) {
    case DCM_DEV_NXT_TOUCH:
        return PNP_DEVICE_SENSOR_NXT_TOUCH;
    case DCM_DEV_NXT_LIGHT:
        return PNP_DEVICE_SENSOR_NXT_LIGHT;
    case DCM_DEV_NXT_SOUND:
        return PNP_DEVICE_SENSOR_NXT_SOUND;
    case DCM_DEV_NXT_COLOR:
        return PNP_DEVICE_SENSOR_NXT_COLOR;
    case DCM_DEV_NXT_SONIC:
        return PNP_DEVICE_SENSOR_NXT_SONIC;
    case DCM_DEV_NXT_TEMP:
        return PNP_DEVICE_SENSOR_NXT_TEMP;
    case DCM_DEV_LARGE_MOTOR:
    case DCM_DEV_LARGE_MOTOR_2:
        return PNP_DEVICE_MOTOR_LARGE;
    case DCM_DEV_MEDIUM_MOTOR:
        return PNP_DEVICE_MOTOR_MEDIUM;
    case DCM_DEV_EV3_TOUCH:
        return PNP_DEVICE_SENSOR_EV3_TOUCH;
    case DCM_DEV_EV3_COLOR:
        return PNP_DEVICE_SENSOR_EV3_COLOR;
    case DCM_DEV_EV3_SONIC:
        return PNP_DEVICE_SENSOR_EV3_SONIC;
    case DCM_DEV_EV3_GYRO:
        return PNP_DEVICE_SENSOR_EV3_GYRO;
    case DCM_DEV_EV3_IR:
        return PNP_DEVICE_SENSOR_EV3_IR;
    case DCM_DEV_ENERGYMETER:
        return PNP_DEVICE_SENSOR_NXT_ENERGY;
    case DCM_DEV_GENERIC_IIC:
    case DCM_DEV_UNKNOWN:
    default:
        return PNP_DEVICE_UNKNOWN;
    case DCM_DEV_TTY:
        return PNP_DEVICE_TTY;
    case DCM_DEV_NONE:
    case DCM_DEV_ERROR:
        return PNP_DEVICE_NONE;
    }
}
