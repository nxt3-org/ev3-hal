#ifndef NXT3_HAL_LED
#define NXT3_HAL_LED

#include <stdbool.h>

typedef enum {
    BRICK_LED_OFF,
    BRICK_LED_ON_GREEN,
    BRICK_LED_ON_RED,
    BRICK_LED_ON_ORANGE,
    BRICK_LED_BLINK_GREEN,
    BRICK_LED_BLINK_RED,
    BRICK_LED_BLINK_ORANGE,
    BRICK_LED_BEAT_GREEN,
    BRICK_LED_BEAT_RED,
    BRICK_LED_BEAT_ORANGE,
} brick_ledmode_t;

extern bool Hal_Led_RefAdd(void);
extern bool Hal_Led_RefDel(void);
extern bool Hal_Led_SetMode(brick_ledmode_t mode);

#endif //NXT3_HAL_LED
