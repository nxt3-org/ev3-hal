#ifndef HAL_PNP_PRIVATE
#define HAL_PNP_PRIVATE

#include "hal_pnp.h"
#include "sen_base.h"

typedef struct {
    sensor_dev_t *sensor;
    pnp_link_t   link;
    pnp_device_t device;
    uint8_t      hwMode;
    uint8_t      emulMode;
    port_state_t state;
} pnp_port_t;

typedef struct {
    int        refCount;
    pnp_port_t ports[8];
} mod_pnp_t;

extern mod_pnp_t Mod_Pnp;

void dcmLinkFound(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev);
void dcmLinkLost(dcm_port_id_t port);
void portHandshakeSuccess(dcm_port_id_t port, pnp_link_t link, pnp_device_t device, uint8_t hwMode);
void portHandshakeFailure(dcm_port_id_t port);
void switchStarted(dcm_port_id_t port);
void switchFailed(dcm_port_id_t port);
void switchFinished(dcm_port_id_t port, uint8_t hwMode);
void startEmulation(dcm_port_id_t port);
void stopEmulation(dcm_port_id_t port);

#endif //HAL_PNP_PRIVATE
