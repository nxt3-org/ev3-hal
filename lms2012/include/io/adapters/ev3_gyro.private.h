#ifndef EV3_GYRO_PRIVATE
#define EV3_GYRO_PRIVATE

#include <hal_adc.h>
#include "tty.private.h"
#include "touch.private.h"

typedef struct {
    hal_adc_dev_t     adc;
    adapter_t         dev;
    const interface_t *iface;

    int             port;
} ev3_gyro_t;

#endif //EV3_GYRO_PRIVATE
