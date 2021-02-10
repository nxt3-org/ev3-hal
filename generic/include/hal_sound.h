// SPDX-License-Identifier: MIT
/**
 * NXT3 sound output HAL
 * =====================
 * - tone playback, tone sequence playback, waveform playback
 * - push-mode architecture
 * - currently only supports a single fixed sample rate (8 kHz for lms2012)
 * - currently only supports a single fixed format (u8 pcm for everything)
 */

#ifndef HAL_SOUND
#define HAL_SOUND

#include <stdbool.h>
#include <stdint.h>

//! Maximum number of bytes in a single sound buffer.
#define SOUND_BUFFER_SIZE 240 // [bytes]

//! Special value returned when an error occurred.
#define SOUND_RESULT_ERROR (-1)

//! Special value returned when there's no room for new samples yet.
#define SOUND_RESULT_BUSY (-2)

/**
 * Initialize the sound subsystem, if needed, and add one to its ref counter.
 * @return True if the operation was OK, false otherwise.
 */
extern bool Hal_Sound_RefAdd(void);

/**
 * Subtract one from sound subsystem refcounter and deinitialize it, if we're the last user.
 * @return True if operation was OK, false otherwise.
 */
extern bool Hal_Sound_RefDel(void);

/**
 * Update internal module state. Call this often if possible (~1 call/ms).
 */
extern void Hal_Sound_Tick(void);

/**
 * Stop currently playing sound.
 * @return True if the sound was stopped successfully, false otherwise.
 */
extern bool Hal_Sound_Stop(void);

/**
 * Check what sample rate is supported by this HAL.
 * @return Single supported sample rate in Hz.
 */
extern int Hal_Sound_SupportedSampleRate(void);

/**
 * Begin waveform playback, if not already running, and supply new data.
 * @param samples Sample buffer (u8 pcm mono @ supported sample rate).
 * @param length Number of bytes/samples in the sample buffer.
 * @param volume Sound volume in percent.
 * @return Number of written samples if successful, SOUND_RESULT_BUSY if no space is available yet and
 *         SOUND_RESULT_ERROR if an error occurs.
 * @remarks Previously started playbacks will be interrupted.
 */
extern int Hal_Sound_SendPcm(uint8_t *samples, uint32_t length, uint8_t volume);

/**
 * Begin single tone playback.
 * @param freqHZ Tone frequency in hertz.
 * @param durMS Tone duration in milliseconds.
 * @param volume Tone volume in percent.
 * @return True if tone was started successfully, false otherwise.
 * @remarks Previously started playbacks will be interrupted.
 */
extern bool Hal_Sound_SendTone(uint16_t freqHZ, uint16_t durMS, uint8_t volume);

/**
 * Begin tone sequence playback, if not already running, and supply new data.
 * @param notes Note buffer (u16 freq + u16 ms sequence).
 * @param length Number of bytes in the note buffer.
 * @param volume Sound volume in percent.
 * @return Number of written bytes if successful, SOUND_RESULT_BUSY if no space is available yet and
 *         SOUND_RESULT_ERROR if an error occurs.
 * @remarks Previously started playbacks will be interrupted.
 */
extern int Hal_Sound_SendMelody(uint8_t *notes, uint32_t length, uint8_t volume);

/**
 * Test whether tone playback was finished.
 * @return True if the tone is over, false otherwise.
 */
extern bool Hal_Sound_IsFinished(void);

#endif //HAL_SOUND
