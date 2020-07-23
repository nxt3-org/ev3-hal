#ifndef DRV_DCM_PRIVATE
#define DRV_DCM_PRIVATE

#include "drv_dcm.h"

typedef enum {
    DCM_LINK_UNKNOWN      = 111,
    DCM_LINK_NXT_COLOR    = 118,
    DCM_LINK_NXT_DUMB     = 119,
    DCM_LINK_NXT_IIC      = 120,
    DCM_LINK_INPUT_DUMB   = 121,
    DCM_LINK_INPUT_UART   = 122,
    DCM_LINK_OUTPUT_DUMB  = 123,
    DCM_LINK_OUTPUT_SMART = 124,
    DCM_LINK_OUTPUT_TACHO = 125,
    DCM_LINK_NONE         = 126,
    DCM_LINK_ERROR        = 127
} dcm_link_t;

typedef enum {
    DCM_DEV_NXT_TOUCH     = 1,
    DCM_DEV_NXT_LIGHT     = 2,
    DCM_DEV_NXT_SOUND     = 3,
    DCM_DEV_NXT_COLOR     = 4,
    DCM_DEV_NXT_SONIC     = 5,
    DCM_DEV_NXT_TEMP      = 6,
    DCM_DEV_LARGE_MOTOR   = 7,
    DCM_DEV_MEDIUM_MOTOR  = 8,
    DCM_DEV_LARGE_MOTOR_2 = 9,
    DCM_DEV_EV3_TOUCH     = 16,
    DCM_DEV_EV3_COLOR     = 29,
    DCM_DEV_EV3_SONIC     = 30,
    DCM_DEV_EV3_GYRO      = 32,
    DCM_DEV_EV3_IR        = 33,
    DCM_DEV_ENERGYMETER   = 99,
    DCM_DEV_GENERIC_IIC   = 123,
    DCM_DEV_TTY           = 124,
    DCM_DEV_UNKNOWN       = 125,
    DCM_DEV_NONE          = 126,
    DCM_DEV_ERROR            = 127,
} dcm_dev_t;

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
