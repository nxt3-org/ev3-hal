#ifndef HAL_DISPLAY
#define HAL_DISPLAY

#include <stdint.h>
#include <stdbool.h>

#define HAL_DISPLAY_CONTRAST_MAX 100
#define HAL_DISPLAY_CONTRAST_MIN 0
#define HAL_DISPLAY_WIDTH 100
#define HAL_DISPLAY_HEIGHT 64

typedef enum {
    DISPLAY_SCALE_CENTER,
    DISPLAY_SCALE_CORNER,
    DISPLAY_SCALE_STRETCH,
    DISPLAY_SCALE_CROP,
} display_scaling_t;

extern bool Hal_Display_RefAdd(void);
extern bool Hal_Display_RefDel(void);
extern bool Hal_Display_SetPower(bool on);
extern bool Hal_Display_SetContrast(int contrast);
extern bool Hal_Display_Write(const uint8_t *data);
extern bool Hal_Display_SetScaling(display_scaling_t mode);

#endif //HAL_DISPLAY
