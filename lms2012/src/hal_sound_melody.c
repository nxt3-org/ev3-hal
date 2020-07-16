#include "hal_sound.private.h"
#include <stdio.h>
#include <memory.h>
#include "kdevices.h"


int Hal_Sound_StartMelody(uint8_t *stream, uint32_t length, uint8_t volume) {
    if (Mod_Sound.refCount <= 0)
        return false;

    if (Mod_Sound.melodyLength == 0) {
        size_t realLen = length > SOUNDBUFFERSIZE ? SOUNDBUFFERSIZE : length;
        memcpy(Mod_Sound.melodyBuffer, stream, realLen);

        resetState();
        Mod_Sound.state        = SOUND_STATE_MELODY;
        Mod_Sound.melodyLength = realLen;
        Mod_Sound.melodyVolume = volume;
        Mod_Sound.melodyPtr    = 0;

        return SOUND_RESULT_SENT;
    } else {
        return SOUND_RESULT_BUSY;
    }
}

void Hal_Sound_Tick(void) {
    if (Mod_Sound.state != SOUND_STATE_MELODY)
        return;

    if (DeviceSound.mmap->fifo_state == FIFO_EMPTY) {
        if (Mod_Sound.melodyPtr <= (Mod_Sound.melodyLength - 4)) {
            uint16_t freq, ms;
            freq  = (uint16_t) Mod_Sound.melodyBuffer[Mod_Sound.melodyPtr++] << 8;
            freq += (uint16_t) Mod_Sound.melodyBuffer[Mod_Sound.melodyPtr++];
            ms    = (uint16_t) Mod_Sound.melodyBuffer[Mod_Sound.melodyPtr++] << 8;
            ms   += (uint16_t) Mod_Sound.melodyBuffer[Mod_Sound.melodyPtr++];
            writeTone(freq, ms, Mod_Sound.melodyVolume);
        } else {
            Mod_Sound.melodyLength = 0;
            Mod_Sound.melodyPtr    = 0;
        }
    }
}
