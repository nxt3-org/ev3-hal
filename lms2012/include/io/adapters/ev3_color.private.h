#ifndef EV3_COLOR_PRIVATE
#define EV3_COLOR_PRIVATE

#include <stdint.h>
#include <hal_iic.h>
#include <hal_adc.h>
#include <io/core/adapter.h>
#include <io/core/interface.h>
#include "iic_base.h"

typedef struct {
    hal_adc_dev_t     adc;
    adapter_t         dev;
    const interface_t *iface;

    struct hal_pins pins;
    int             port;
} ev3_color_t;

#endif //EV3_COLOR_PRIVATE
