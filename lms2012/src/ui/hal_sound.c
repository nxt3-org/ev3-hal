// SPDX-License-Identifier: MIT
// Common sound functions //
#include <string.h>
#include "common/kdevices.h"
#include "ui/hal_sound.private.h"

mod_sound_t Mod_Sound;

bool Hal_Sound_RefAdd(void) {
    if (Mod_Sound.refCount > 0) {
        Mod_Sound.refCount++;
        return true;
    }

    if (!Kdev_RefAdd(&DeviceSound))
        return false;

    Mod_Sound.state = SOUND_STATE_STOPPED;
    memset(&Mod_Sound.melody, 0, sizeof(Mod_Sound.melody));
    Mod_Sound.refCount++;
    Hal_Sound_Stop();
    return true;
}

bool Hal_Sound_RefDel(void) {
    if (Mod_Sound.refCount == 0)
        return false;
    if (Mod_Sound.refCount == 1) {
        Kdev_RefDel(&DeviceSound);
    }
    Mod_Sound.refCount--;
    return true;
}

bool Hal_Sound_Stop(void) {
    if (Mod_Sound.refCount <= 0)
        return false;

    resetState();

    sound_req_break req = {.cmd = CMD_BREAK};
    return writeCommand(&req, sizeof(req), false);
}

void Hal_Sound_Tick(void) {
    if (Mod_Sound.state == SOUND_STATE_MELODY) {
        pushMelody();
    }
}

bool writeCommand(void *buffer, uint32_t size, bool busy) {
    int written = Kdev_Pwrite(&DeviceSound, buffer, size, 0);
    if (written >= 0 && busy)
        DeviceSound.mmap->fifo_state = FIFO_PROCESSING;
    return written >= 0;
}

void resetState(void) {
    Mod_Sound.state = SOUND_STATE_STOPPED;

    Mod_Sound.melody.readBuffer        = MELODY_BUFFERS - 1;
    Mod_Sound.melody.lastWrittenBuffer = MELODY_BUFFERS - 1;
    for (int i = 0; i < MELODY_BUFFERS; i++) {
        Mod_Sound.melody.buffers[i].remaining = 0;
        Mod_Sound.melody.buffers[i].readPtr   = 0;
    }
}

uint8_t convertVolume(uint8_t percent) {
    if (percent == 0) {
        return 0;
    }
    if (percent <= 50) {
        if (percent <= 25) {
            if (percent <= 12) {
                return 1;
            } else {
                return 2;
            }
        } else {
            if (percent <= 37) {
                return 3;
            } else {
                return 4;
            }
        }
    } else {
        if (percent <= 75) {
            if (percent <= 63) {
                return 5;
            } else {
                return 6;
            }
        } else {
            if (percent <= 88) {
                return 7;
            } else {
                return 8;
            }
        }
    }
}
