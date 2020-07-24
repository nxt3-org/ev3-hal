#include "sen_tty.private.h"
#include "hal_general.h"
#include <malloc.h>
#include <stdio.h>

#define sen_to_tty(sen) container_of(sen, sensor_tty_t, dev)
#define link_to_tty(lnk) container_of(lnk, sensor_tty_t, link)

bool tty_attach(sensor_dev_t *dev) {
    sensor_tty_t *this = sen_to_tty(dev);
    printf("NXT3 ready\r\n>\r\n");
    return Hal_IicHost_Attach(&this->link, this->port);
}

void tty_detach(sensor_dev_t *dev) {
    sensor_tty_t *this = sen_to_tty(dev);
    Hal_IicHost_Detach(this->port);
}

void tty_destroy(sensor_dev_t *dev) {
    sensor_tty_t *this = sen_to_tty(dev);
    free(this);
}

void tty_tick(hal_iic_dev_t *dev) {
    sensor_tty_t *this = link_to_tty(dev);

    if (this->port != 0)
        return;

    this->counter++;
    if (this->counter < 1000)
        return;
    this->counter = 0;

    printf(".");
    fflush(stdout);
}

static hal_iic_ops_t iic_ops = {
    .prewrite = NULL,
    .preread = NULL,
    .write = NULL,
    .read = NULL,
    .cancel = NULL,
    .tick = tty_tick,
};

static sensor_ops_t sensor_ops = {
    .attach = tty_attach,
    .detach = tty_detach,
    .destroy = tty_destroy,
};

sensor_dev_t *Sensor_TTY_Create(int port) {
    sensor_tty_t *dev = malloc(sizeof(sensor_tty_t));
    if (dev) {
        dev->link.ops = &iic_ops;
        dev->dev.ops  = &sensor_ops;
        dev->port     = port;
        dev->counter  = 0;
        return &dev->dev;
    }
    return NULL;
}
