#ifndef HAL_SOUND
#define HAL_SOUND

#include <stdbool.h>

typedef enum {
    BRICK_FEATURE_PCM,
    BRICK_FEATURE_TONE,
} sound_feature_t;

extern bool Hal_Sound_RefAdd(void);
extern bool Hal_Sound_RefDel(void);


#endif //HAL_SOUND
