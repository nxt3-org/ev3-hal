#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <hal_timer.h>
#include <sys/mman.h>
#include "hal_display.h"
#include "hal_display.private.h"

mod_display_t Mod_Display;

bool Hal_Display_RefAdd(void) {
    if (Mod_Display.refCount > 0) {
        Mod_Display.refCount++;
        return true;
    }

    Mod_Display.fd = open("/dev/fb0", O_RDWR);
    if (Mod_Display.fd < 0) {
        perror("EV3 HAL: cannot open framebuffer");
        return false;
    }

    Mod_Display.kernelMemory = mmap(NULL, EV3_DISPLAY_SIZE,
                                    PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED,
                                    Mod_Display.fd, 0);
    if (Mod_Display.kernelMemory == MAP_FAILED) {
        Mod_Display.kernelMemory = NULL;
        perror("EV3 HAL: cannot mmap framebuffer device");

        close(Mod_Display.fd);
        Mod_Display.fd = -1;
        return false;
    }

    Mod_Display.powerOn     = false;
    Mod_Display.scalingMode = DISPLAY_SCALE_CENTER;
    Mod_Display.refCount++;

    Hal_Display_SetPower(true);
    return true;
}

bool Hal_Display_RefDel(void) {
    if (Mod_Display.refCount == 0)
        return false;
    Mod_Display.refCount--;
    if (Mod_Display.refCount == 0) {
        if (Mod_Display.fd >= 0) {
            if (close(Mod_Display.fd) < 0)
                perror("EV3 HAL: cannot close framebuffer");
            Mod_Display.fd = -1;
        }
        if (Mod_Display.kernelMemory) {
            if (munmap((void *) Mod_Display.kernelMemory, EV3_DISPLAY_SIZE) < 0) {
                perror("EV3 HAL: cannot munmap LMS UI device");
            }
            Mod_Display.kernelMemory = NULL;
        }
    }
    return true;
}

bool Hal_Display_SetPower(bool on) {
    if (Mod_Display.refCount <= 0)
        return false;

    if (on != Mod_Display.powerOn) {
        memset(Mod_Display.kernelMemory, 0, EV3_DISPLAY_SIZE);
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
        memset(Mod_Display.kernelMemory, 0, EV3_DISPLAY_SIZE);
    return true;
}

bool Hal_Display_Supports(display_feature_t feature) {
    switch (feature) {
    case BRICK_FEATURE_DISPLAY_POWER:
    case BRICK_FEATURE_DISPLAY_WRITE:
    case BRICK_FEATURE_DISPLAY_SCALING_CORNER:
    case BRICK_FEATURE_DISPLAY_SCALING_CENTER:
    case BRICK_FEATURE_DISPLAY_SCALING_STRETCH:
    case BRICK_FEATURE_DISPLAY_SCALING_CROP:
        return true;
    case BRICK_FEATURE_DISPLAY_CONTRAST:
        return false;
    default:
        return false;
    }
}
