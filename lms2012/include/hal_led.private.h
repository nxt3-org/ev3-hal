#ifndef NXT3_HAL_LED_LMS2012
#define NXT3_HAL_LED_LMS2012

#include <stdint.h>

typedef struct {
    char mode;
    char padding;
} led_request_t;

typedef struct {
    int refCount;
} mod_led_t;

extern mod_led_t Mod_Led;

#endif //NXT3_HAL_LED_LMS2012
