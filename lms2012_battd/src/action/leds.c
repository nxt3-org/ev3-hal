#include <action/leds.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <fifo.h>

static int fd;

bool leds_open(void) {
    fd = open("/dev/lms_ui", O_RDWR);
    if (fd < 0) {
        perror("l/open");
        return false;
    }
    return true;
}

void leds_close(void) {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

void leds_alert(uint16_t warning) {
    const char *code = "1";

    if (warning & OVERHEAT_SHDN)
        code = "5";
    else if (warning & OVERHEAT_WARN)
        code = "6";
    else if (warning & (LOW_BATTERY_CRIT | LOW_BATTERY_SHDN))
        code = "2";
    else if (warning & LOW_BATTERY_WARN)
        code = "3";

    write(fd, code, 2);
}
