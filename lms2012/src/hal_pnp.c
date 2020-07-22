#include "hal_pnp.private.h"

mod_pnp_t Mod_Pnp;

bool Hal_Pnp_RefAdd(void) {
    if (Mod_Pnp.refCount > 0) {
        Mod_Pnp.refCount++;
        return true;
    }
    // init
    Mod_Pnp.refCount++;
    return true;
}

bool Hal_Pnp_RefDel(void) {
    if (Mod_Pnp.refCount == 0)
        return false;
    if (Mod_Pnp.refCount == 1) {
        // deinit
    }
    Mod_Pnp.refCount--;
    return true;
}

void Hal_Pnp_Tick(void) {

}
