#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "hal_sound.private.h"

mod_sound_t Mod_Sound;

bool Hal_Sound_RefAdd(void) {
    if (Mod_Sound.refCount > 0) {
        Mod_Sound.refCount++;
        return true;
    }

    Mod_Sound.state        = SOUND_STATE_STOPPED;
    Mod_Sound.melodyVolume = 8;
    Mod_Sound.melodyPtr    = 0;
    Mod_Sound.melodyLength = 0;

    Mod_Sound.fd = open("/dev/lms_sound", O_RDWR);
    if (Mod_Sound.fd < 0) {
        perror("EV3 HAL: cannot open sound device");
        return false;
    }

    Mod_Sound.mmap = mmap(0, sizeof(sound_mmap_t),
                          PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED,
                          Mod_Sound.fd, 0);
    if (Mod_Sound.mmap == MAP_FAILED) {
        Mod_Sound.mmap = NULL;
        perror("EV3 HAL: cannot mmap sound device");

        close(Mod_Sound.fd);
        Mod_Sound.fd = -1;
        return false;
    }

    Mod_Sound.refCount++;
    return true;
}

bool Hal_Sound_RefDel(void) {
    if (Mod_Sound.refCount == 0)
        return false;
    if (Mod_Sound.refCount == 1) {
        if (Mod_Sound.mmap) {
            if (munmap((void *) Mod_Sound.mmap, sizeof(sound_mmap_t)) < 0) {
                perror("EV3 HAL: cannot munmap sound device");
            }
            Mod_Sound.mmap = NULL;
        }

        if (Mod_Sound.fd >= 0) {
            Hal_Sound_Stop();
            if (close(Mod_Sound.fd) < 0) {
                perror("EV3 HAL: cannot close sound device");
            }
            Mod_Sound.fd = -1;
        }
    }
    Mod_Sound.refCount--;
    return false;
}

bool Hal_Sound_Stop(void) {
    if (Mod_Sound.refCount <= 0)
        return false;

    resetState();

    sound_req_break req = {.cmd = CMD_BREAK};
    return writeCommand(&req, sizeof(req), false);
}

bool writeCommand(void *buffer, uint32_t size, bool busy) {
    int written = write(Mod_Sound.fd, buffer, size);
    if (written < 0) {
        perror("EV3 HAL: cannot submit sound command");
        return false;
    }
    if (busy)
        Mod_Sound.mmap->fifo_state = FIFO_PROCESSING;
    return true;
}

void resetState(void) {
    Mod_Sound.state = SOUND_STATE_STOPPED;
    Mod_Sound.melodyPtr    = 0;
    Mod_Sound.melodyLength = 0;
}

bool Hal_Sound_Supports(sound_feature_t feature) {
    switch(feature) {
    case BRICK_FEATURE_PCM_8KHZ:
    case BRICK_FEATURE_TONE:
    case BRICK_FEATURE_MELODY:
        return true;
    case BRICK_FEATURE_PCM_ANY:
    default:
        return false;
    }
}
