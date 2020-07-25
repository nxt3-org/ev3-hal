#ifndef DISPLAY
#define DISPLAY

#include <stdbool.h>
#include <stdint.h>

bool display_open(void);
void display_close(void);
void display_clear(void);
void display_draw(uint16_t warning);

#endif //DISPLAY
