#include "hal_timer.h"
#include "hal_timer.private.h"

#include <time.h>

mod_timer_t Mod_Timer;

bool Hal_Timer_RefAdd(void) {
    if (Mod_Timer.refCount == 0) {
        Mod_Timer.start = Hal_Timer_Now();
    }
    Mod_Timer.refCount++;
    return true;
}

bool Hal_Timer_RefDel(void) {
    if (Mod_Timer.refCount == 0)
        return false;
    Mod_Timer.refCount--;
    return true;
}

hal_timestamp_t Hal_Timer_StartTime(void) {
    return Mod_Timer.start;
}

hal_timestamp_t Hal_Timer_Now(void) {
    struct timespec time0;
    clock_gettime(CLOCK_MONOTONIC, &time0);

    return (hal_timestamp_t) {
        .nanos   = time0.tv_nsec,
        .seconds = time0.tv_sec
    };
}

void Hal_Timer_WaitForMillisecond(hal_timestamp_t since) {
    hal_timestamp_t now = Hal_Timer_Now();
    struct timespec sleepT;
    sleepT.tv_sec = 0;

    if (now.seconds > since.seconds) {
        if (now.seconds > since.seconds + 1)
            return;
        else
            sleepT.tv_nsec = 1000000 - (1000000000 - since.nanos + now.nanos);
    } else {
        sleepT.tv_nsec = 1000000 - (now.nanos - since.nanos);
    }
    if (sleepT.tv_nsec < 0)
        return;

    while(nanosleep(&sleepT, &sleepT) < 0)
        ;
}
