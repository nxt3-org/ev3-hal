#ifndef EV3_NATIVE_PRIVATE
#define EV3_NATIVE_PRIVATE

#include <hal_ev3sensor.h>
#include <io/core/adapter.h>
#include <io/core/interface.h>

typedef struct {
    hal_sensor2_dev_t out;
    adapter_t         dev;
    const interface_t *iface;

    int port;
} ev3_native_wrapper_t;

#endif //EV3_NATIVE_PRIVATE
