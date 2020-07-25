#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <fifo.h>
#include "action/sound.h"

typedef struct {
    uint8_t  cmd;
    uint8_t  volume;
    uint16_t frequency;
    uint16_t duration;
} sound_req_tone;

static int fd;

bool sound_open(void) {
    fd = open("/dev/lms_sound", O_RDWR);
    if (fd < 0) {
        perror("s/open");
        return false;
    }
    return true;
}

void sound_close(void) {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

void sound_alert(uint16_t warning, uint16_t msec) {
    int freq = (warning & (OVERHEAT_WARN | OVERHEAT_SHDN)) ? 1200 : 800;

    uint8_t cmd = 0; // stop
    write(fd, &cmd, sizeof(cmd));

    sound_req_tone req = {
        .cmd = 1,
        .volume = 8,
        .frequency = freq,
        .duration = msec
    };
    write(fd, &req, sizeof(req));
}
