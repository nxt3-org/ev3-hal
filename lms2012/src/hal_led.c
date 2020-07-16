#include <unistd.h>
#include <stdio.h>
#include "kdev_ui.h"
#include "hal_led.h"
#include "hal_led.private.h"

mod_led_t Mod_Led;

bool Hal_Led_RefAdd(void) {
    if (Mod_Led.refCount == 0) {
        if (!Kdev_UI_RefAdd())
            return false;
        Mod_Led.refCount++;
        Hal_Led_SetMode(BRICK_LED_ON_GREEN);
        return true;
    }
    Mod_Led.refCount++;
    return true;
}

bool Hal_Led_RefDel(void) {
    if (Mod_Led.refCount == 0)
        return false;
    Mod_Led.refCount--;
    if (Mod_Led.refCount == 0) {
        Hal_Led_SetMode(BRICK_LED_OFF);
        Kdev_UI_RefDel();
    }
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

    int written = pwrite(Kdev_UI.fd, &req, sizeof(req), 0);
    if (written < 0) {
        perror("EV3 HAL: cannot set LED color");
        return false;
    }
    return true;
}

bool Hal_Led_Supports(led_feature_t feature) {
    switch (feature) {
    case BRICK_FEATURE_LED_MODE:
        return true;
    default:
        return false;
    }
}
