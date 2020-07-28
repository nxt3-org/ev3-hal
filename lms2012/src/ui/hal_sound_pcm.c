#include "ui/hal_sound.private.h"
#include "common/kdevices.h"
#include <stdio.h>
#include <memory.h>

int Hal_Sound_StartPcm(uint8_t *samples, uint32_t length, uint16_t samplerate, uint8_t volume) {
    if (Mod_Sound.refCount <= 0)
        return SOUND_RESULT_ERROR;

    // start & underrun
    if (DeviceSound.mmap->fifo_state == FIFO_EMPTY) {
        if (!initPcm(samplerate, volume))
            return SOUND_RESULT_ERROR;
    }

    resetState();
    Mod_Sound.state = SOUND_STATE_PCM;

    if (length == 0)
        return SOUND_RESULT_SENT;

    return writePCM(samples, length);
}

bool initPcm(uint16_t samplerate, uint8_t volume) {
    // nope, only 8000 Hz supported
    if (samplerate != SAMPLERATE_DEFAULT) {
        fprintf(stderr, "EV3 HAL: invalid sound sample rate: %d\n", samplerate);
        return false;
    }

    if (volume > SOUNDVOLUMESTEPS)
        volume = SOUNDVOLUMESTEPS;

    sound_req_play req = {
        .cmd = CMD_PLAY,
        .volume = 2 * volume
    };

    return writeCommand(&req, sizeof(req), true);
}

int writePCM(void *samples, size_t size) {
    if (size > SOUNDBUFFERSIZE)
        size = SOUNDBUFFERSIZE;

    sound_req_data req = {.cmd = CMD_DATA};
    memcpy(req.samples, samples, size);

    int written = Kdev_Write(&DeviceSound, &req, size + 1, 0);

    return written < 0 ? SOUND_RESULT_ERROR :
           (written == 0 ? SOUND_RESULT_BUSY : SOUND_RESULT_SENT);
}
