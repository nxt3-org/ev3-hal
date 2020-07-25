#include <action/pwm.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static int fd;

bool pwm_open(void) {
    fd = open("/dev/lms_pwm", O_RDWR);
    if (fd < 0) {
        perror("m/open");
        return false;
    }
    return true;
}

void pwm_close(void) {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

void pwm_stop(void) {
    char code = 0x02; // opPROGRAM_STOP
    write(fd, &code, 1);
}
