#include <hal_battery.h>
#include <math.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdio.h>
#include <spawn.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <pthread.h>

#include "hal_battery.private.h"

mod_battery_t Mod_Battery;

bool Hal_Battery_RefAdd(void) {
    if (Mod_Battery.refCount > 0) {
        Mod_Battery.refCount++;
        return true;
    }

    bool battd_running = system("pidof battd.elf") == 0;
    if (!battd_running && !spawn_battd())
        return false;

    int error;
    Mod_Battery.memFd = -1;
    for (int i = 0; i < 100; i++) {
        Mod_Battery.memFd = open("/tmp/battd", O_RDWR);
        if (Mod_Battery.memFd >= 0)
            break;
        error = errno;
        struct timespec ts = {.tv_sec = 0, .tv_nsec = 10000000};
        while (nanosleep(&ts, &ts));
    }
    if (Mod_Battery.memFd < 0) {
        errno = error;
        perror("Cannot open BattD shm channel");
        return false;
    }
    Mod_Battery.mem = mmap(NULL, sizeof(battd_memory_t),
                           PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED,
                           Mod_Battery.memFd, 0);
    if (Mod_Battery.mem == MAP_FAILED) {
        perror("Cannot mmap BattD shm channel");
        close(Mod_Battery.memFd);
        Mod_Battery.memFd = -1;
        return false;
    }

    Mod_Battery.lastRx = (struct timespec) {0, 0};

    pthread_mutex_lock(&Mod_Battery.mem->Mutex);
    Mod_Battery.state          = Mod_Battery.mem->Message;
    Mod_Battery.rxCounter      = Mod_Battery.mem->CounterTx;
    Mod_Battery.mem->CounterRx = Mod_Battery.rxCounter;
    pthread_mutex_unlock(&Mod_Battery.mem->Mutex);

    if (Mod_Battery.state.BattD_Version != BATTD_VERSION) {
        system("killall battd.elf");
        munmap(Mod_Battery.mem, sizeof(battd_memory_t));
        close(Mod_Battery.memFd);
        Mod_Battery.mem   = NULL;
        Mod_Battery.memFd = -1;
        fputs("BattD version mismatch!\n", stderr);
        return false;
    }

    Mod_Battery.refCount++;
    return true;
}

bool Hal_Battery_RefDel(void) {
    if (Mod_Battery.refCount == 0)
        return false;
    if (Mod_Battery.refCount == 1) {
        if (Mod_Battery.mem) {
            munmap(Mod_Battery.mem, sizeof(battd_memory_t));
            Mod_Battery.mem = NULL;
        }
        if (Mod_Battery.memFd >= 0) {
            close(Mod_Battery.memFd);
            Mod_Battery.memFd = 0;
        }
    }
    Mod_Battery.refCount--;
    return true;
}

void Hal_Battery_Tick(void) {
    bool changed = false;
    pthread_mutex_lock(&Mod_Battery.mem->Mutex);
    if (Mod_Battery.mem->CounterTx != Mod_Battery.rxCounter) {
        changed = true;
        Mod_Battery.rxCounter      = Mod_Battery.mem->CounterTx;
        Mod_Battery.mem->CounterRx = Mod_Battery.rxCounter;;
        Mod_Battery.state = Mod_Battery.mem->Message;
    }
    pthread_mutex_unlock(&Mod_Battery.mem->Mutex);

    if (changed) {
        clock_gettime(CLOCK_MONOTONIC, &Mod_Battery.lastRx);
    }
}

bool Hal_Battery_IsRechargeable(bool *pReally) {
    if (Mod_Battery.refCount <= 0 || !isOnline())
        return false;
    *pReally = Mod_Battery.state.Events & IS_RECHARGEABLE;
    return true;
}

bool Hal_Battery_GetVoltage(float *pV) {
    if (Mod_Battery.refCount <= 0 || !isOnline())
        return false;
    *pV = Mod_Battery.state.Battery_Voltage;
    return true;
}

bool Hal_Battery_GetCurrent(float *pA) {
    if (Mod_Battery.refCount <= 0 || !isOnline())
        return false;
    *pA = Mod_Battery.state.Battery_Current;
    return true;
}

bool Hal_Battery_GetPercentRemaining(float *pPercent) {
    if (Mod_Battery.refCount <= 0 || !isOnline())
        return false;
    *pPercent = Mod_Battery.state.Battery_Percent;
    return true;
}

bool Hal_Battery_GetTemperature(float *pC) {
    if (Mod_Battery.refCount <= 0 || !isOnline())
        return false;
    float temp = Mod_Battery.state.Battery_Temperature;
    if (isnan(temp))
        return false;
    else
        *pC = temp;
    return true;
}

bool Hal_Battery_CheckBatteryWarning(bool *pWarning, bool *pCritical) {
    if (Mod_Battery.refCount <= 0 || !isOnline())
        return false;
    *pWarning = Mod_Battery.state.Events & LOW_BATTERY_WARN;
    *pCritical = Mod_Battery.state.Events & LOW_BATTERY_CRIT;
    return true;
}

bool Hal_Battery_CheckTempWarning(bool *pReally) {
    if (Mod_Battery.refCount <= 0 || !isOnline())
        return false;
    *pReally = Mod_Battery.state.Events & OVERHEAT_WARN;
    return true;
}

bool spawn_battd(void) {
    bool success = true;
    char *self   = NULL,
         *battd  = NULL;

    if ((self = realpath("/proc/self/exe", NULL)) == NULL) {
        perror("Cannot get self path");
        goto fail;
    }
    if (asprintf(&battd, "%s/battd.elf", dirname(self)) < 0) {
        perror("Cannot get battd path");
        goto fail;
    }
    char *argv[] = {battd, NULL};
    if (posix_spawn(NULL, battd, NULL, NULL, argv, environ) < 0) {
        perror("Cannot spawn battd");
        goto fail;
    }

exit:
    if (battd) free(battd);
    if (self) free(self);
    return success;
fail:
    success = false;
    goto exit;
}

bool isOnline(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (now.tv_sec < Mod_Battery.lastRx.tv_sec)
        return true; // should not happen though
    if (now.tv_sec == Mod_Battery.lastRx.tv_sec)
        return true; // within a second
    if (now.tv_sec == Mod_Battery.lastRx.tv_sec + 1)
        return now.tv_nsec <= Mod_Battery.lastRx.tv_nsec; // second boundary
    return false; // more than a second
}
