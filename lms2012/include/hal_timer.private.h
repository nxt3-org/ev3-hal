#ifndef HAL_TIMER_PRIVATE
#define HAL_TIMER_PRIVATE

#include "hal_timer.h"

typedef struct {
    int             refCount;
    hal_timestamp_t start;
} mod_timer_t;

extern mod_timer_t Mod_Timer;

#endif //HAL_TIMER_PRIVATE
