#include <stdio.h>
#include <memory.h>
#include "hal_timer.h"
#include "common/kdevices.h"
#include "hal_display.h"
#include "ui/hal_display.private.h"

mod_display_t Mod_Display;

bool Hal_Display_RefAdd(void) {
    if (Mod_Display.refCount > 0) {
        Mod_Display.refCount++;
        return true;
    }

    if (!Kdev_RefAdd(&DeviceDisplay))
        return false;

    Mod_Display.powerOn     = false;
    Mod_Display.scalingMode = DISPLAY_SCALE_CENTER;
    Mod_Display.refCount++;

    Hal_Display_SetPower(true);
    return true;
}

bool Hal_Display_RefDel(void) {
    if (Mod_Display.refCount == 0)
        return false;
    if (Mod_Display.refCount == 1) {
        Hal_Display_SetPower(false);
        Kdev_RefDel(&DeviceDisplay);
    }
    Mod_Display.refCount--;
    return true;
}

bool Hal_Display_SetPower(bool on) {
    if (Mod_Display.refCount <= 0)
        return false;

    if (on != Mod_Display.powerOn) {
        memset(DeviceDisplay.mmap, 0, EV3_DISPLAY_SIZE);
        Mod_Display.powerOn = on;
    }

    return true;
}

bool Hal_Display_Write(const uint8_t *data) {
    if (Mod_Display.refCount <= 0)
        return false;

    if (!Mod_Display.powerOn)
        return true;

    if (Mod_Display.scalingMode == DISPLAY_SCALE_CENTER) {
        writeDirect(data, true);
    } else if (Mod_Display.scalingMode == DISPLAY_SCALE_CORNER) {
        writeDirect(data, false);
    } else if (Mod_Display.scalingMode == DISPLAY_SCALE_STRETCH) {
        writeStretch(data);
    } else if (Mod_Display.scalingMode == DISPLAY_SCALE_CROP) {
        writeCrop(data);
    }
    return true;
}

bool Hal_Display_SetContrast(int contrast) {
    (void) contrast;
    return false;
}

bool Hal_Display_SetScaling(display_scaling_t mode) {
    if (Mod_Display.refCount <= 0)
        return false;

    display_scaling_t oldMode = Mod_Display.scalingMode;

    switch (mode) {
    case DISPLAY_SCALE_CENTER:
    case DISPLAY_SCALE_CORNER:
    case DISPLAY_SCALE_CROP:
    case DISPLAY_SCALE_STRETCH:
        Mod_Display.scalingMode = mode;
        break;
    default:
        return false;
    }

    if (oldMode != mode)
        memset(DeviceDisplay.mmap, 0, EV3_DISPLAY_SIZE);
    return true;
}
