#ifndef MOTOR_PRIVATE
#define MOTOR_PRIVATE

#include <io/core/adapter.h>
#include "hal_motor.h"
#include "hal_pnp_defs.h"

typedef struct {
    hal_motor_dev_t   motor;
    adapter_t         dev;
    const interface_t *iface;
    int               port;
} motor_device_t;

#endif //MOTOR_PRIVATE
