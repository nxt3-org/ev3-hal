// SPDX-License-Identifier: MIT
// PCM waveform sender //
#include "ui/hal_sound.private.h"
#include "common/kdevices.h"
#include <memory.h>

int Hal_Sound_SendPcm(uint8_t *samples, uint32_t length, uint8_t volume) {
    if (Mod_Sound.refCount <= 0)
        return SOUND_RESULT_ERROR;

    // start & underrun
    if (DeviceSound.mmap->fifo_state == FIFO_EMPTY) {
        if (!initPcm(convertVolume(volume)))
            return SOUND_RESULT_ERROR;
    }

    resetState();
    Mod_Sound.state = SOUND_STATE_PCM;

    if (length == 0)
        return 0; // just initialize, do not send data

    return writePCM(samples, length);
}

int Hal_Sound_SupportedSampleRate(void) {
    return LMS2012_SAMPLERATE;
}

bool initPcm(uint8_t volume) {
    sound_req_play req = {
        .cmd = CMD_PLAY,
        .volume = volume
    };

    return writeCommand(&req, sizeof(req), true);
}

int writePCM(void *samples, uint32_t size) {
    if (size > SOUND_BUFFER_SIZE)
        size = SOUND_BUFFER_SIZE;

    sound_req_data req = {.cmd = CMD_DATA};
    memcpy(req.samples, samples, size);

    int written = Kdev_Pwrite(&DeviceSound, &req, size + 1, 0);

    return written < 0 ? SOUND_RESULT_ERROR :
           (written == 0 ? SOUND_RESULT_BUSY : written);
}
