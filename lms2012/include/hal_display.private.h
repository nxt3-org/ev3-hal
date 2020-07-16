#ifndef HAL_DISPLAY_PRIVATE
#define HAL_DISPLAY_PRIVATE

#include <stdint.h>
#include "hal_display.h"

#define EV3_DISPLAY_WIDTH 178
#define EV3_DISPLAY_HEIGHT 128
#define EV3_DISPLAY_STRIDE ((EV3_DISPLAY_WIDTH+2)/3)
#define EV3_DISPLAY_SIZE (EV3_DISPLAY_STRIDE*EV3_DISPLAY_HEIGHT)

typedef struct {
    int               refCount;
    display_scaling_t scalingMode;
    bool powerOn;
} mod_display_t;

extern mod_display_t Mod_Display;

extern void writeDirect(const uint8_t *buffer, bool centered);
extern void writeCrop(const uint8_t *buffer);
extern void writeStretch(const uint8_t *buffer);

#endif //HAL_DISPLAY_PRIVATE
