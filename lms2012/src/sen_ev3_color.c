#include "sen_ev3_color.private.h"
#include "hal_general.h"
#include <malloc.h>
#include <sen_ev3_color.h>
#include <stdio.h>

#define sen_to_color(sns)  container_of(sns, ev3_color_t, dev)

static sensor_ops_t sensor_ops = {
    .attach = color_attach,
    .detach = color_detach,
    .destroy = color_destroy
};

static hal_iic_ops_t iic_ops = {
    .prewrite = NULL,
    .write = NULL,
    .preread = NULL,
    .read = NULL,
    .cancel = NULL,
    .tick = color_tick,
};

bool color_attach(sensor_dev_t *dev) {
    return true;
}

void color_detach(sensor_dev_t *dev) {
    return;
}

void color_destroy(sensor_dev_t *dev) {
    ev3_color_t *this = sen_to_color(dev);
    free(this);
}

sensor_dev_t *Sensor_EV3Color_Create(int port) {
    ev3_color_t *this = malloc(sizeof(ev3_color_t));
    if (this) {
        this->dev.ops  = &sensor_ops;
        this->port     = port;
        return &this->dev;
    }
    return NULL;
}

void color_tick(hal_iic_dev_t *dev) {
}
