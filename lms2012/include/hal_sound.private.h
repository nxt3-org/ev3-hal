#ifndef HAL_SOUND_PRIVATE
#define HAL_SOUND_PRIVATE

#include "hal_sound.h"

#define CMD_PLAY 2
typedef struct {
    uint8_t cmd;
    uint8_t volume;
} sound_req_play;

#define CMD_BREAK 0
typedef struct {
    uint8_t cmd;
} sound_req_break;

#define CMD_TONE 1
typedef struct {
    uint8_t  cmd;
    uint8_t  volume;
    uint16_t frequency;
    uint16_t duration;
} sound_req_tone;

#define CMD_DATA 4
typedef struct {
    uint8_t cmd;
    uint8_t samples[SOUNDBUFFERSIZE];
} sound_req_data;


typedef enum __attribute__((packed)) {
    FIFO_EMPTY      = 0,
    FIFO_PROCESSING = 1,
} sound_kstate_t;

typedef struct {
    sound_kstate_t fifo_state;
    uint8_t        padding[3];
} sound_mmap_t;

typedef enum {
    SOUND_STATE_UNKNOWN = -1,
    SOUND_STATE_STOPPED = 0,
    SOUND_STATE_PCM,
    SOUND_STATE_MELODY,
    SOUND_STATE_TONE,
} sound_state_t;

typedef struct {
    int                   refCount;
    int                   fd;
    volatile sound_mmap_t *mmap;
    sound_state_t         state;
    uint16_t              melodyPtr;
    uint16_t              melodyLength;
    uint8_t               melodyVolume;
    uint8_t               melodyBuffer[SOUNDBUFFERSIZE];
} mod_sound_t;

extern mod_sound_t Mod_Sound;

extern bool initPcm(uint16_t samplerate, uint8_t volume);
extern bool initMelody(uint8_t volume);
extern void resetState(void);

extern bool writeCommand(void *buffer, uint32_t size, bool busy);
extern int writePCM(void *samples, uint32_t size);
extern bool writeTone(uint16_t freqHZ, uint16_t durMS, uint8_t volume);

#endif //HAL_SOUND_PRIVATE
