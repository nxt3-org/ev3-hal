#ifndef PWM
#define PWM

#include <stdbool.h>

bool pwm_open(void);
void pwm_close(void);
void pwm_stop(void);

#endif //PWM
