#include "kdev_ui.h"
#include "hal_button.h"
#include "hal_button.private.h"

// button mapping
const brick_button_t map_native_to_hal[6] = {
    [EV3_ENTER] = BRICK_BUTTON_ENTER,
    [EV3_UP]    = BRICK_EXTRA_BUTTON_UP,
    [EV3_DOWN]  = BRICK_EXTRA_BUTTON_DOWN,
    [EV3_LEFT]  = BRICK_BUTTON_LEFT,
    [EV3_RIGHT] = BRICK_BUTTON_RIGHT,
    [EV3_BACK]  = BRICK_BUTTON_EXIT,
};

// module data
mod_button_t Mod_Button;

// functions
extern bool Hal_Button_RefAdd(void) {
    if (Mod_Button.refCount == 0) {
        if (!Kdev_UI_RefAdd())
            return false;
    }
    Mod_Button.refCount++;
    return true;
}

extern bool Hal_Button_RefDel(void) {
    if (Mod_Button.refCount == 0)
        return false;
    Mod_Button.refCount--;
    if (Mod_Button.refCount == 0) {
        Kdev_UI_RefDel();
    }
    return true;
}

extern bool Hal_Button_Read(uint32_t *pPressedMask) {
    if (Mod_Button.refCount <= 0)
        return false;

    int result = 0;

    kdev_ui_mmap_t copy = *Kdev_UI.mmap;

    for (ev3_button_t btn = 0; btn < EV3_NO_BUTTONS; btn++) {
        int shift = map_native_to_hal[btn];

        if (copy.isPressed[btn]) {
            result |= 1 << shift;
        }
    }

    *pPressedMask = result;
    return true;
}

extern bool Hal_Button_Supports(button_feature_t feature) {
    switch (feature) {
    case BRICK_FEATURE_BUTTONS:
    case BRICK_FEATURE_EXTRA_BUTTONS:
        return true;
    default:
        return false;
    }
}
