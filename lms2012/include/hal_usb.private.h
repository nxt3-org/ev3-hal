#ifndef HAL_USB_PRIVATE
#define HAL_USB_PRIVATE

#include "hal_usb.h"

typedef struct {
    int refCount;
    bool addrSet;
} mod_usb_t;

extern mod_usb_t Mod_Usb;

#endif //HAL_USB_PRIVATE
