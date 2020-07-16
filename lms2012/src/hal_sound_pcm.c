#include "hal_sound.private.h"
#include <stdio.h>
#include <memory.h>
#include <unistd.h>

int Hal_Sound_StartPcm(uint8_t *samples, uint32_t length, uint16_t samplerate, uint8_t volume) {
    if (Mod_Sound.refCount <= 0)
        return SOUND_RESULT_ERROR;

    // start & underrun
    if (Mod_Sound.mmap->fifo_state == FIFO_EMPTY) {
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

    int written = pwrite(Mod_Sound.fd, &req, size + 1, 0);
    if (written < 0) {
        perror("EV3 HAL: cannot submit sound data");
        return SOUND_RESULT_ERROR;
    } else if (written == 0) {
        return SOUND_RESULT_BUSY;
    } else {
        return SOUND_RESULT_SENT;
    }
}
