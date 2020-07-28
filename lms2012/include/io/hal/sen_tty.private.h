#ifndef SEN_TTY_PRIVATE
#define SEN_TTY_PRIVATE

#include "hal_iic.h"
#include "sen_tty.h"

typedef struct {
    hal_iic_dev_t link;
    sensor_dev_t  dev;

    int port;
    int counter;
} sensor_tty_t;

extern bool tty_attach(sensor_dev_t *dev);
extern void tty_detach(sensor_dev_t *dev);
extern void tty_destroy(sensor_dev_t *dev);
extern void tty_tick(hal_iic_dev_t *link);

#endif //SEN_TTY_PRIVATE
