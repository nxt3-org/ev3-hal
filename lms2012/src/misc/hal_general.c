#include "hal_general.h"
#include "common/kdevices.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>

static char VersionBuffer[8];

void Hal_General_AbnormalExit(const char *message) {
    fprintf(stderr, "%s\n", message);
    fflush(stdout);
    fflush(stderr);
    abort();
}

static uint8_t *pRun = NULL;

static void sigint(int signal);

void Hal_General_SetupSignals(uint8_t *pStopFlag) {
    pRun = pStopFlag;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &sigint;
    sigaction(SIGINT, &sa, NULL);
}

const char *Hal_General_GetPlatformName(void) {
    return "LEGO EV3";
}

const char *Hal_General_GetPlatformHwVersion(void) {
    const char *result = VersionBuffer;

    if (VersionBuffer[0] == '\0') {
        if (Kdev_RefAdd(&DeviceUi)) {
            if (Kdev_Read(&DeviceUi, VersionBuffer, sizeof(VersionBuffer), 0) < 0) {
                result = "unknown";
            }

            Kdev_RefDel(&DeviceUi);
        } else {
            result = "unknown";
        }
    }

    return result;
}

const char *Hal_General_GetPlatformFwVersion(void) {
    return "unknown";
}

static const char exitMessage[] = "Received SIGINT, exiting...\n";

void sigint(int signal) {
    write(STDERR_FILENO, exitMessage, sizeof(exitMessage));
    *pRun = false;
}
