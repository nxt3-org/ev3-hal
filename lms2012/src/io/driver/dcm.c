#include "hal_general.h"
#include "common/kdevices.h"
#include "io/core/units.h"
#include "io/driver/dcm.private.h"

drv_dcm_t Dcm;

bool Dcm_RefAdd(void) {
    if (Dcm.refCount > 0) {
        Dcm.refCount++;
        return true;
    }
    if (!Kdev_RefAdd(&DeviceAnalog))
        return false;
    if (!Kdev_RefAdd(&DeviceConnMgr))
        return false;

    for (int no = 0; no < 8; no++) {
        Dcm.Ports[no].Device = DCM_DEV_NONE;
        Dcm.Ports[no].Link   = DCM_LINK_UNKNOWN;
    }
    Dcm.refCount++;
    return true;
}

bool Dcm_RefDel(void) {
    if (Dcm.refCount == 0)
        return false;
    if (Dcm.refCount == 1) {
        if (!Kdev_RefDel(&DeviceConnMgr))
            Hal_General_AbnormalExit("ERROR: refcounting error on dcm device");
        if (!Kdev_RefDel(&DeviceAnalog))
            Hal_General_AbnormalExit("ERROR: refcounting error on analog device");
    }
    Dcm.refCount--;
    return true;
}

void Dcm_Tick() {
    for (int idx = 0; idx < 8; idx++) {
        bool output = idx >= 4;
        int  port   = output ? idx - 4 : idx;

        dcm_link_t newDcmLink = newLink(port, output);
        dcm_type_t newDcmDev  = newDevice(port, output);
        dcm_link_t oldDcmLink = oldLink(port, output);
        dcm_type_t oldDcmDev  = oldDevice(port, output);

        Dcm.Ports[idx].Link   = newDcmLink;
        Dcm.Ports[idx].Device = newDcmDev;

        // no transition -> not interesting
        if (newDcmLink == oldDcmLink && newDcmDev == oldDcmDev)
            continue;

        pnp_link_t newPnpLink = IdentifyLink(newDcmLink);
        pnp_link_t oldPnpLink = IdentifyLink(oldDcmLink);
        if (oldPnpLink != PNP_LINK_NONE)
            Hal_Pnp_LinkLost(port, output);
        if (newPnpLink != PNP_LINK_NONE)
            Hal_Pnp_LinkFound(port, output, newDcmLink, newDcmDev);
    }
}

bool Dcm_SetPins(int port, char code) {
    if (port >= 4)
        return false;

    char string[] = "----";
    string[port] = code;

    return Kdev_Pwrite(&DeviceConnMgr, string, sizeof(string), 0) >= 0;
}

int16_t Dcm_MeasureAutoID(int port, bool output) {
    if (output) {
        return AdcVolts(DeviceAnalog.mmap->Adc_Pin5_M[port]);
    } else {
        return AdcVolts(DeviceAnalog.mmap->Adc_Pin1_S[port]);
    }
}

dcm_link_t newLink(int port, bool output) {
    if (output)
        return DeviceAnalog.mmap->Dcm_OutLink[port];
    else
        return DeviceAnalog.mmap->Dcm_InLink[port];
}

dcm_type_t newDevice(int port, bool output) {
    if (output)
        return DeviceAnalog.mmap->Dcm_OutType[port];
    else
        return DeviceAnalog.mmap->Dcm_InType[port];
}

dcm_link_t oldLink(int port, bool output) {
    int portN = port + (output ? 4 : 0);
    return Dcm.Ports[portN].Link;
}

dcm_type_t oldDevice(int port, bool output) {
    int portN = port + (output ? 4 : 0);
    return Dcm.Ports[portN].Device;
}
