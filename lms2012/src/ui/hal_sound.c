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

    Mod_Sound.state        = SOUND_STATE_STOPPED;
    Mod_Sound.melodyVolume = 8;
    Mod_Sound.melodyPtr    = 0;
    Mod_Sound.melodyLength = 0;
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

bool writeCommand(void *buffer, uint32_t size, bool busy) {
    int written = Kdev_Write(&DeviceSound, buffer, size, 0);
    if (written >= 0 && busy)
        DeviceSound.mmap->fifo_state = FIFO_PROCESSING;
    return written >= 0;
}

void resetState(void) {
    Mod_Sound.state        = SOUND_STATE_STOPPED;
    Mod_Sound.melodyPtr    = 0;
    Mod_Sound.melodyLength = 0;
}
