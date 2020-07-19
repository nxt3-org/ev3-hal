#include "hal_iic.private.h"
#include <stdio.h>

mod_i2c_t Mod_I2C;

bool Hal_IicMgr_RefAdd(void) {
    if (Mod_I2C.refCount > 0) {
        Mod_I2C.refCount++;
        return true;
    }

    Mod_I2C.pSonic = createSonic();
    if (Mod_I2C.pSonic == NULL) {
        perror("Cannot allocate ultrasonic sensor data");
        return false;
    }
    if (!Hal_IicHost_Attach(&Mod_I2C.pSonic->link, VICTIM_PORT))
        return false;

    Mod_I2C.refCount++;
    return true;
}

bool Hal_IicMgr_RefDel(void) {
    if (Mod_I2C.refCount == 0)
        return false;
    if (Mod_I2C.refCount == 1) {
        deleteSonic(Mod_I2C.pSonic);
        Mod_I2C.pSonic     = NULL;
    }
    return true;
}

void Hal_IicMgr_Tick(void) {
}
