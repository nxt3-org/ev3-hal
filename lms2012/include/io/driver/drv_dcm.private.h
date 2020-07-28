#ifndef DRV_DCM_PRIVATE
#define DRV_DCM_PRIVATE

#include "drv_dcm.h"


typedef struct {
    dcm_link_t lastLink;
    dcm_dev_t  lastDevice;
} dcm_port_t;

typedef struct {
    int            refCount;
    dcm_port_t     ports[8];
    dcm_callback_t *mgr;
} drv_dcm_t;

extern drv_dcm_t Drv_Dcm;

pnp_link_t dcm2pnp_link(dcm_link_t dcm);
pnp_device_t dcm2pnp_dev(dcm_dev_t dcm);

dcm_link_t getNewLink(dcm_port_id_t port);
dcm_dev_t getNewDevice(dcm_port_id_t port);

dcm_link_t getLastLink(dcm_port_id_t port);
dcm_dev_t getLastDevice(dcm_port_id_t port);

#endif //DRV_DCM_PRIVATE
