#ifndef NXT3_HAL_BUTTON_LMS2012
#define NXT3_HAL_BUTTON_LMS2012

#include "hal_button.h"

typedef enum {
    EV3_UP,
    EV3_ENTER,
    EV3_DOWN,
    EV3_RIGHT,
    EV3_LEFT,
    EV3_BACK,
    EV3_NO_BUTTONS
} ev3_button_t;

extern const brick_button_t map_native_to_hal[EV3_NO_BUTTONS];

typedef struct {
    int refCount;
} mod_button_t;

extern mod_button_t Mod_Button;

#endif //NXT3_HAL_BUTTON_LMS2012
