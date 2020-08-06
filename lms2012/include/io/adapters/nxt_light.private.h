#ifndef NXT_LIGHT_PRIVATE
#define NXT_LIGHT_PRIVATE

#include <io/core/adapter.h>
#include <io/core/interface.h>
#include "hal_adc.h"
#include "hal_iic.h"

typedef struct {
    hal_adc_dev_t     adc;
    adapter_t         dev;
    const interface_t *iface;
    int               port;
    struct hal_pins pins;
} sensor_nxt_light_t;

#endif //NXT_LIGHT_PRIVATE
