#include "hal_general.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>

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

static const char exitMessage[] = "Received SIGINT, exiting...\n";
void sigint(int signal) {
    write(STDERR_FILENO, exitMessage, sizeof(exitMessage));
    *pRun = false;
}
