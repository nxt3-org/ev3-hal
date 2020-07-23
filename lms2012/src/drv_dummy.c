#include "drv_dummy.h"


bool Dummy_Init(void) {
    return true;
}

bool Dummy_Exit(void) {
    return true;
}

static identify_callback_t *idCalls = NULL;

void Dummy_SetCallbacks(identify_callback_t *id, modeswitch_callback_t *mode) {
    idCalls = id;
    (void) mode;
}

void Dummy_DeviceStart(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev) {
    idCalls->failure(port);
}

void Dummy_DeviceStop(dcm_port_id_t port) {
    return;
}

port_driver_ops_t DriverDummy = {
    .Init = Dummy_Init,
    .Exit = Dummy_Exit,
    .DeviceStart = Dummy_DeviceStart,
    .DeviceStop = Dummy_DeviceStop,
    .SetCallbacks = Dummy_SetCallbacks,
    .Tick = NULL
};
