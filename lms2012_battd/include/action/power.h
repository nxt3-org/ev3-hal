#ifndef POWER
#define POWER

#include <stdint.h>
#include <stdbool.h>

extern bool power_open(void);
extern void power_close(void);
extern bool power_is_rechargeable(void);
extern void power_shutdown(void);

#endif //POWER
