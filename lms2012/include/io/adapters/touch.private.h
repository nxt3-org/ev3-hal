#ifndef TOUCH_PRIVATE
#define TOUCH_PRIVATE

#include <io/core/adapter.h>
#include <io/core/interface.h>
#include "hal_adc.h"
#include "hal_iic.h"

typedef struct {
    hal_adc_dev_t     adc;
    adapter_t         dev;
    const interface_t *iface;
    int               port;
} sensor_ev3_touch_t;

#endif //TOUCH_PRIVATE
