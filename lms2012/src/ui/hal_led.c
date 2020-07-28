#include "common/kdevices.h"
#include "hal_led.h"
#include "ui/hal_led.private.h"

mod_led_t Mod_Led;

bool Hal_Led_RefAdd(void) {
    if (Mod_Led.refCount > 0) {
        Mod_Led.refCount++;
        return true;
    }

    if (!Kdev_RefAdd(&DeviceUi))
        return false;

    Mod_Led.refCount++;
    Hal_Led_SetMode(BRICK_LED_ON_GREEN);
    return true;
}

bool Hal_Led_RefDel(void) {
    if (Mod_Led.refCount == 0)
        return false;
    if (Mod_Led.refCount == 1) {
        Hal_Led_SetMode(BRICK_LED_OFF);
        Kdev_RefDel(&DeviceUi);
    }
    Mod_Led.refCount--;
    return true;
}

bool Hal_Led_SetMode(brick_ledmode_t mode) {
    if (Mod_Led.refCount <= 0)
        return false;

    if (mode < 0 || mode > 9)
        return false;

    led_request_t req = {
        .mode = '0' + mode,
        .padding = '\0'
    };

    return Kdev_Write(&DeviceUi, &req, sizeof(req), 0) >= 0;
}
