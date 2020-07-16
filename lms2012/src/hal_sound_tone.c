#include "hal_sound.private.h"
#include "kdevices.h"

bool Hal_Sound_StartTone(uint16_t freqHZ, uint16_t durMS, uint8_t volume) {
    if (Mod_Sound.refCount <= 0)
        return false;

    resetState();
    Mod_Sound.state = SOUND_STATE_TONE;

    return writeTone(freqHZ, durMS, volume);
}

bool Hal_Sound_ToneFinished(void) {
    if (Mod_Sound.refCount <= 0)
        return true;

    if (Mod_Sound.state == SOUND_STATE_TONE) {
        return DeviceSound.mmap->fifo_state == FIFO_EMPTY;
    } else {
        return true;
    }
}

bool writeTone(uint16_t freqHZ, uint16_t durMS, uint8_t volume) {
    if (volume > SOUNDVOLUMESTEPS)
        volume = SOUNDVOLUMESTEPS;

    if (durMS < DURATION_MIN)
        durMS = DURATION_MIN;

    if (freqHZ > FREQUENCY_MAX)
        freqHZ = FREQUENCY_MAX;
    else if (freqHZ < FREQUENCY_MIN)
        freqHZ = FREQUENCY_MIN;

    sound_req_tone req = {
        .cmd = CMD_TONE,
        .volume = 2 * volume,
        .duration = durMS,
        .frequency = freqHZ
    };

    return writeCommand(&req, sizeof(req), true);
}
