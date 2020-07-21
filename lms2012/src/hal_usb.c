#include "hal_filesystem.h"
#include "hal_usb.private.h"

mod_usb_t Mod_Usb;

bool Hal_Usb_RefAdd(void) {
    if (Mod_Usb.refCount > 1) {
        Mod_Usb.refCount++;
        return true;
    }

    Mod_Usb.addrSet = false;
    for (int hnd = 0; hnd < MAX_HANDLES; hnd++) {
        Hal_Usb_RemoveHandle(hnd);
    }

    // do some init

    Mod_Usb.refCount++;
    return true;
}

bool Hal_Usb_RefDel(void) {
    if (Mod_Usb.refCount == 0)
        return false;
    if (Mod_Usb.refCount == 1) {
        // do some deinit
    }
    Mod_Usb.refCount--;
    return true;
}

void Hal_Usb_StoreBtAddress(const uint8_t *raw) {
    Mod_Usb.addrSet = true;
}

void Hal_Usb_ResetState(void) {

}

bool Hal_Usb_IsPresent(void) {
    return false;
}

bool Hal_Usb_IsReady(void) {
    return false;
}

uint32_t Hal_Usb_RxFrame(uint8_t *buffer, uint32_t maxLength) {
    return 0;
}

void Hal_Usb_TxFrame(const uint8_t *buffer, uint32_t maxLength) {
    return;
}
