#include "hal_timer.h"

hal_timestamp_t Hal_Timer_Delta(hal_timestamp_t start, hal_timestamp_t end) {
    hal_timestamp_t delta;

    if (end.nanos > start.nanos) {
        delta.seconds = end.seconds - start.seconds;
        delta.nanos   = end.nanos - start.nanos;
    } else {
        delta.seconds = end.seconds - start.seconds - 1;
        delta.nanos   = 1000000000 - start.nanos + end.nanos;
    }

    return delta;
}

uint32_t Hal_Timer_MillisSince(hal_timestamp_t last) {
    hal_timestamp_t delta = Hal_Timer_Delta(last, Hal_Timer_Now());
    return delta.seconds * 1000 + delta.nanos / 1000000;
}

uint32_t Hal_Timer_MicrosSince(hal_timestamp_t last) {
    hal_timestamp_t delta = Hal_Timer_Delta(last, Hal_Timer_Now());
    return delta.seconds * 1000000 + delta.nanos / 1000;
}

bool Hal_Timer_MillisecondElapsed(hal_timestamp_t start) {
    hal_timestamp_t now = Hal_Timer_Now();

    if (now.seconds > start.seconds) {
        if (now.seconds > start.seconds + 1)
            return true;
        else
            return (1000000000 - start.nanos + now.nanos) >= 1000000;
    } else {
        return (now.nanos - start.nanos) >= 1000000;
    }
}

uint32_t Hal_Timer_ElapsedMillis(void) {
    return Hal_Timer_MillisSince(Hal_Timer_StartTime());
}
