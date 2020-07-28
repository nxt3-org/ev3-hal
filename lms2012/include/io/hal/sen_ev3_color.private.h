#ifndef SEN_EV3_COLOR_PRIVATE
#define SEN_EV3_COLOR_PRIVATE

#include "sen_ev3_color.h"
#include "io/driver/drv_uart.h"
#include <stdint.h>
#include <hal_iic.h>

typedef struct {
    sensor_dev_t dev;
    uint8_t port;
} ev3_color_t;

static bool color_attach(sensor_dev_t *dev);
static void color_detach(sensor_dev_t *dev);
static void color_destroy(sensor_dev_t *dev);
static void color_tick(hal_iic_dev_t *dev);

#endif //SEN_EV3_COLOR_PRIVATE
