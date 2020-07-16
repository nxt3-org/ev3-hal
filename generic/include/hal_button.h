#ifndef NXT3_HAL_BUTTON
#define NXT3_HAL_BUTTON

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BRICK_BUTTON_EXIT = 0,
    BRICK_BUTTON_RIGHT = 1,
    BRICK_BUTTON_LEFT = 2,
    BRICK_BUTTON_ENTER = 3,
    BRICK_NO_BUTTONS = 4,

    BRICK_EXTRA_BUTTON_UP = 4,
    BRICK_EXTRA_BUTTON_DOWN = 5,
    BRICK_NO_EXTRA_BUTTONS = 6,
} brick_button_t;

typedef enum {
    BRICK_FEATURE_BUTTONS,
    BRICK_FEATURE_EXTRA_BUTTONS,
} button_feature_t;

extern bool Hal_Button_RefAdd(void);
extern bool Hal_Button_RefDel(void);
extern bool Hal_Button_Read(uint32_t *pPressedMask);
extern bool Hal_Button_Supports(button_feature_t feature);

#endif
