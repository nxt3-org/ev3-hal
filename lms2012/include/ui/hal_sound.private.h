// SPDX-License-Identifier: MIT
// LMS2012 sound implementation header //
#ifndef HAL_SOUND_PRIVATE
#define HAL_SOUND_PRIVATE

#include "hal_sound.h"

//! sample rate supported by kernel drivers
#define LMS2012_SAMPLERATE 8000  // [Hz]
//! span of the supported volume range
#define LMS2012_MAX_VOLUME 8     // [steps]
//! Number of allocated melody buffers.
#define MELODY_BUFFERS 2

//! Minimum duration of a single tone.
#define TONE_DURATION_MIN     10  // [mS]
//! Minimum supported frequency of a tone.
#define TONE_FREQUENCY_MIN   250  // [Hz]
//! Maximum supported frequency of a tone.
#define TONE_FREQUENCY_MAX 10000  // [Hz]


//! ioctl struct for waveform playback start request
#define CMD_PLAY 2
typedef struct {
    uint8_t cmd;    //!< Command byte (needs to be CMD_PLAY).
    uint8_t volume; //!< Playback volume in range 0-8.
} sound_req_play;

//! ioctl struct for stop request
#define CMD_BREAK 0
typedef struct {
    uint8_t cmd; //!< Command byte (needs to be CMD_BREAK).
} sound_req_break;

//! ioctl struct for tone playback request
#define CMD_TONE 1
typedef struct {
    uint8_t  cmd;       //!< Command byte (needs to be CMD_TONE).
    uint8_t  volume;    //!< Tone volume in range 0-8.
    uint16_t frequency; //!< Tone frequency in range 250-10000.
    uint16_t duration;  //!< Tone duration in range 10-65535.
} sound_req_tone;

//! ioctl struct for waveform data submission
#define CMD_DATA 4
typedef struct {
    uint8_t cmd;                        //!< Command byte (needs to be CMD_DATA).
    uint8_t samples[SOUND_BUFFER_SIZE]; //!< U8 PCM samples.
} sound_req_data;

//! Sound module state.
typedef enum {
    SOUND_STATE_STOPPED = 0, //!< No playback is going on.
    SOUND_STATE_PCM,         //!< Sending PCM to the kernel.
    SOUND_STATE_MELODY,      //!< Sending tones from a melody to the kernel.
    SOUND_STATE_TONE,        //!< Sent a single tone to the kernel.
} sound_state_t;

//! Submitted melody buffer.
typedef struct {
    uint8_t samples[SOUND_BUFFER_SIZE]; //!< Melody samples (u16 freq + u16 ms, big endian!)
    uint8_t readPtr;   //!< Read position in the sample buffer.
    uint8_t remaining; //!< How many samples are remaining in the buffer.
    uint8_t volume;    //!< Volume associated with this buffer.
} melody_buffer_t;

//! Melody supplier state.
typedef struct {
    melody_buffer_t buffers[MELODY_BUFFERS]; //!< Melody buffers.
    uint8_t         readBuffer;              //!< Current active buffer.
    uint8_t         lastWrittenBuffer;       //!< Last buffer written (seesentially end pointer).
} melody_state_t;

//! Sound module data.
typedef struct {
    int            refCount; //!< Module reference counter.
    sound_state_t  state;    //!< Current state.
    melody_state_t melody;   //!< Melody FIFO state.
} mod_sound_t;

//! Sound module data.
extern mod_sound_t Mod_Sound;

/**
 * Request PCM playback start.
 * @param volume Requested volume (0-8);
 * @return True if priming succeeded, false otherwise.
 */
extern bool initPcm(uint8_t volume);

/**
 * Reset internal module state.
 */
extern void resetState(void);

/**
 * Send an ioctl request.
 * @param buffer Command struct.
 * @param size   Length of the command struct.
 * @param busy   Whether to enable the operation running flag in the kernel driver.
 * @return True if the command succeeded, false otherwise.
 */
extern bool writeCommand(void *buffer, uint32_t size, bool busy);

/**
 * Write PCM waveform data to the kernel.
 * @param samples Sample buffer.
 * @param size    Number of bytes in the sample buffer.
 * @return Number of written samples if successful, SOUND_RESULT_BUSY if no space is available yet and
 *         SOUND_RESULT_ERROR if an error occurs.
 */
extern int writePCM(void *samples, uint32_t size);

/**
 * Write tone request to the kernel.
 * @param freqHZ Frequency of the tone (250-10000 Hz).
 * @param durMS  Duration of the tone (10 - 65535 ms).
 * @param volume Volume of the tone (0-8).
 * @return True if the request was successful, false otherwise.
 */
extern bool writeTone(uint16_t freqHZ, uint16_t durMS, uint8_t volume);

/**
 * Convert percent volume to range 0-8.
 * @param Volume in range 0-100 (but the full range is handled).
 * @return Volume in range 0-8 (mapped from 0-100).
 */
extern uint8_t convertVolume(uint8_t percent);

/**
 * Update playing melody, if there is any.
 */
extern void pushMelody(void);

#endif //HAL_SOUND_PRIVATE
