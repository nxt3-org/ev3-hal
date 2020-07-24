#include "sen_ev3_color.private.h"
#include "hal_general.h"
#include <malloc.h>
#include <memory.h>
#include <sen_ev3_color.h>
#include <drv_uart.h>
#include <stdio.h>

#define link_to_color(lnk) container_of(lnk, ev3_color_t, link)
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
    ev3_color_t *this = sen_to_color(dev);
    return Hal_IicHost_Attach(&this->link, this->port);
}

void color_detach(sensor_dev_t *dev) {
    ev3_color_t *this = sen_to_color(dev);
    Hal_IicHost_Detach(this->port);
}

void color_destroy(sensor_dev_t *dev) {
    ev3_color_t *this = sen_to_color(dev);
    free(this);
}

sensor_dev_t *Sensor_EV3Color_Create(int port) {
    ev3_color_t *this = malloc(sizeof(ev3_color_t));
    if (this) {
        this->link.ops      = &iic_ops;
        this->dev.ops       = &sensor_ops;
        this->port          = port;
        this->launched      = false;
        this->fired = false;
        this->switchCounter = 10;
    }
    return &this->dev;
}


const char *challenge = "LEGO-FAC-CAL-1";

void color_tick(hal_iic_dev_t *dev) {
    ev3_color_t *this = link_to_color(dev);
    if (this->launched == false) {
        this->launched = true;
        printf("god created the world in 6 days\n");
        Drv_Uart_StartCalRead(this->port, &this->datalog);
    }

    if (this->switchCounter > 0) {
        this->switchCounter--;
        if (this->switchCounter == 0) {
            Drv_Uart_SwitchTo(this->port, PNP_MODE_EV3_COLOR_CALREAD);
            printf("let the games begin\n");
        }
    }

    bool        ready;
    bool        write;
    uint16_t    items[3];
    while (Drv_Uart_ContinueCalRead(this->port, &this->datalog, items, &ready, &write)) {
        printf("%c%c %4hu %4hu %4hu\n",
               (ready ? 'R' : ' '),
               (write ? 'W' : ' '),
               items[0],
               items[1],
               items[2]);
    }
    if (this->switchCounter == 0 && ready && !this->fired) {
        printf("poking >:)\n");
        this->fired = true;
        Drv_Uart_Enqueue(this->port, (const uint8_t *) challenge, strlen(challenge));
    }
}
