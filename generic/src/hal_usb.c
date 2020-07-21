#include "hal_usb.h"
#include "hal_filesystem.h"

static bool usbHandles[MAX_HANDLES];

bool Hal_Usb_AddHandle(handle_t hnd) {
    if (hnd >= MAX_HANDLES)
        return false;
    bool ok = usbHandles[hnd] == false;
    usbHandles[hnd] = true;
    return ok;
}

bool Hal_Usb_RemoveHandle(handle_t hnd) {
    if (hnd >= MAX_HANDLES)
        return false;
    bool ok = usbHandles[hnd] == true;
    usbHandles[hnd] = false;
    return ok;
}

handle_t Hal_Usb_GetNextHandle(uint32_t *pPosition) {
    for (; (*pPosition) < MAX_HANDLES; (*pPosition)++) {
        if (usbHandles[*pPosition]) {
            (*pPosition)++;
            return *pPosition;
        }
    }
    return MAX_HANDLES;
}
