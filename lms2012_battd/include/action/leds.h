#ifndef LEDS
#define LEDS

#include <stdbool.h>
#include <stdint.h>

extern bool leds_open(void);
extern void leds_close(void);
extern void leds_alert(uint16_t warning);

#endif //LEDS
