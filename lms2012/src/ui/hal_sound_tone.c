// SPDX-License-Identifier: MIT
// Single tone sender //
#include "ui/hal_sound.private.h"
#include "common/kdevices.h"

bool Hal_Sound_SendTone(uint16_t freqHZ, uint16_t durMS, uint8_t volume) {
    if (Mod_Sound.refCount <= 0)
        return false;

    resetState();
    Mod_Sound.state = SOUND_STATE_TONE;

    return writeTone(freqHZ, durMS, convertVolume(volume));
}

bool Hal_Sound_IsFinished(void) {
    if (Mod_Sound.refCount <= 0)
        return true;

    if (Mod_Sound.state == SOUND_STATE_TONE || Mod_Sound.state == SOUND_STATE_PCM) {
        return DeviceSound.mmap->fifo_state == FIFO_EMPTY;
    } else if (Mod_Sound.state == SOUND_STATE_MELODY) {
        return !hasAnyMelodyData() && DeviceSound.mmap->fifo_state == FIFO_EMPTY;
    } else {
        return true;
    }
}

bool writeTone(uint16_t freqHZ, uint16_t durMS, uint8_t volume) {
    if (volume > LMS2012_MAX_VOLUME)
        volume = LMS2012_MAX_VOLUME;

    if (durMS < TONE_DURATION_MIN)
        durMS = TONE_DURATION_MIN;

    if (freqHZ > TONE_FREQUENCY_MAX)
        freqHZ = TONE_FREQUENCY_MAX;
    else if (freqHZ < TONE_FREQUENCY_MIN)
        freqHZ = TONE_FREQUENCY_MIN;

    sound_req_tone req = {
        .cmd = CMD_TONE,
        .volume = volume,
        .duration = durMS,
        .frequency = freqHZ
    };

    return writeCommand(&req, sizeof(req), true);
}
