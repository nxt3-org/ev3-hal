#ifndef SOUND
#define SOUND

#include <stdbool.h>
#include <stdint.h>

bool sound_open(void);
void sound_close(void);
void sound_alert(uint16_t warning, uint16_t msec);

#endif //SOUND
