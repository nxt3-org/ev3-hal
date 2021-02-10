// SPDX-License-Identifier: MIT
// Sequenced tone sender //
#include "ui/hal_sound.private.h"
#include <memory.h>
#include <hal_sound.h>

#include "common/kdevices.h"

int Hal_Sound_SendMelody(uint8_t *notes, uint32_t length, uint8_t volume) {
    if (Mod_Sound.refCount <= 0)
        return SOUND_RESULT_ERROR;

    // skip small payloads
    if (length < 4)
        return 0;

    bool kickstart = false;
    if (Mod_Sound.state != SOUND_STATE_MELODY) {
        resetState();
        Mod_Sound.state = SOUND_STATE_MELODY;
        kickstart = true;
    }

    // get next avail buffer
    int             nextBufferNo = (Mod_Sound.melody.lastWrittenBuffer + 1) % MELODY_BUFFERS;
    melody_buffer_t *nextBuffer  = &Mod_Sound.melody.buffers[nextBufferNo];

    // we hit an old write -> bye
    if (nextBuffer->remaining != 0) {
        return SOUND_RESULT_BUSY;
    }

    // we can copy the data
    size_t realLength = (length > SOUND_BUFFER_SIZE ? SOUND_BUFFER_SIZE : length) & ~0x03; // multiple of 4

    memcpy(nextBuffer->samples, notes, realLength);
    nextBuffer->readPtr                = 0;
    nextBuffer->remaining              = realLength / 4;
    nextBuffer->volume                 = convertVolume(volume);
    Mod_Sound.melody.lastWrittenBuffer = nextBufferNo;
    if (kickstart) {
        Mod_Sound.melody.readBuffer = nextBufferNo;
    }
    return realLength;
}

void pushMelody(void) {
    if (DeviceSound.mmap->fifo_state == FIFO_EMPTY) {
        melody_buffer_t *currentBuffer = &Mod_Sound.melody.buffers[Mod_Sound.melody.readBuffer];

        // find next non-empty buffer
        while (currentBuffer->remaining == 0) {
            // is this the last buffer?
            if (Mod_Sound.melody.readBuffer == Mod_Sound.melody.lastWrittenBuffer) {
                // yes, bye!
                Mod_Sound.state = SOUND_STATE_STOPPED;
                return;
            } else {
                // no, advance forward
                Mod_Sound.melody.readBuffer = (Mod_Sound.melody.readBuffer + 1) % MELODY_BUFFERS;
                currentBuffer = &Mod_Sound.melody.buffers[Mod_Sound.melody.readBuffer];
            }
        }

        // decode sample
        uint8_t  *data = currentBuffer->samples + currentBuffer->readPtr;
        uint16_t freq  = ((uint16_t) data[0] << 8) + ((uint16_t) data[1]);
        uint16_t ms    = ((uint16_t) data[2] << 8) + ((uint16_t) data[3]);
        writeTone(freq, ms, currentBuffer->volume);

        // advance state
        currentBuffer->readPtr += 4;
        currentBuffer->remaining -= 1;
    }
}

bool hasAnyMelodyData(void) {
    for (int i = 0; i < MELODY_BUFFERS; i++) {
        if (Mod_Sound.melody.buffers[i].remaining > 0)
            return true;
    }
    return false;
}
