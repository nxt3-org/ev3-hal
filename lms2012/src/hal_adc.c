#include "hal_adc.private.h"
#include <stddef.h>

mod_adc_t Mod_Adc;

bool Hal_AdcMgr_RefAdd(void) {
    if (Mod_Adc.refCount > 0) {
        Mod_Adc.refCount++;
        return true;
    }

    Mod_Adc.sensor = createLight();
    if (!Mod_Adc.sensor)
        return false;
    if (!Hal_AdcHost_Attach(&Mod_Adc.sensor->link, VICTIM_PORT))
        return false;

    Mod_Adc.refCount++;
    return true;
}

bool Hal_AdcMgr_RefDel(void) {
    if (Mod_Adc.refCount == 0)
        return false;
    if (Mod_Adc.refCount == 1) {
        deleteLight(Mod_Adc.sensor);
        Mod_Adc.sensor = NULL;
    }
    Mod_Adc.refCount--;
    return true;
}

void Hal_AdcMgr_Tick(void) {
}
