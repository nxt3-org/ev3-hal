#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "action/power.h"

static int fd = -1;
static bool accu = false;

bool power_open(void) {
    fd = open("/dev/lms_power", O_RDONLY);
    if (fd < 0) {
        perror("p/open");
        return false;
    }
    char buffer[8] = "";
    int ok = read(fd, &buffer, 8);
    if (ok < 1) {
        perror("p/read");
        return false;
    }
    if (buffer[0] == '\0') {
        fputs("p/id\n", stderr);
        return false;
    }
    accu = (buffer[0] == '1');
    return true;
}

void power_close(void) {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

bool power_is_rechargeable(void) {
    return accu;
}

void power_shutdown(void) {
    uint8_t off = 1;
    ioctl(fd, 0, &off);
    system("killall -KILL nxt3.elf >/dev/null 2>&1");
    system("killall -KILL lms2012  >/dev/null 2>&1");
}
