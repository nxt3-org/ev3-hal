#ifndef HAL_TIMER
#define HAL_TIMER

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t seconds;
    uint32_t nanos;
} hal_timestamp_t;

extern bool Hal_Timer_RefAdd(void);
extern bool Hal_Timer_RefDel(void);

extern hal_timestamp_t  Hal_Timer_StartTime(void);
extern hal_timestamp_t  Hal_Timer_Now(void);
extern hal_timestamp_t  Hal_Timer_Delta(hal_timestamp_t start, hal_timestamp_t end);
extern uint32_t         Hal_Timer_MillisSince(hal_timestamp_t last);
extern uint32_t         Hal_Timer_MicrosSince(hal_timestamp_t last);
extern bool             Hal_Timer_MillisecondElapsed(hal_timestamp_t pStart);
extern void             Hal_Timer_WaitForMillisecond(hal_timestamp_t since);

extern uint32_t         Hal_Timer_ElapsedMillis(void);

#endif //HAL_TIMER
