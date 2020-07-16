#ifndef HAL_SOUND
#define HAL_SOUND

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BRICK_FEATURE_PCM_8KHZ,
    BRICK_FEATURE_PCM_ANY,
    BRICK_FEATURE_TONE,
    BRICK_FEATURE_MELODY
} sound_feature_t;

#define   SOUNDVOLUMESTEPS              4

#define   DURATION_MIN                  10        // [mS]
#define   FREQUENCY_MIN                 250       // [Hz]
#define   FREQUENCY_MAX                 10000     // [Hz]

#define   SAMPLERATE_MIN                2000      // Min sample rate [sps]
#define   SAMPLERATE_DEFAULT            8000      // Default sample rate [sps]
#define   SAMPLERATE_MAX                16000     // Max sample rate [sps]

#define   SOUNDBUFFERSIZE_ADPCM         120
#define   SOUNDBUFFERSIZE               240
#define   SOUNDBUFFERS                  3


#define SOUND_RESULT_SENT 0
#define SOUND_RESULT_BUSY (1)
#define SOUND_RESULT_ERROR (-1)

extern bool Hal_Sound_RefAdd(void);
extern bool Hal_Sound_RefDel(void);

extern void Hal_Sound_Tick(void);
extern bool Hal_Sound_Stop(void);

extern int  Hal_Sound_StartPcm(uint8_t *samples, uint32_t length, uint16_t samplerate, uint8_t volume);
extern int  Hal_Sound_StartMelody(uint8_t *notes, uint32_t length, uint8_t volume);
extern bool Hal_Sound_StartTone(uint16_t freqHZ, uint16_t durMS, uint8_t volume);
extern bool Hal_Sound_ToneFinished(void);

extern bool Hal_Sound_Supports(sound_feature_t feature);

#endif //HAL_SOUND
