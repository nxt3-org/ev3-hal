#ifndef DRV_PORT
#define DRV_PORT

#include "hal_pnp.h"
#include "drv_dcm.h"

typedef struct {
    void (*success)(dcm_port_id_t port, pnp_link_t link, pnp_device_t device, uint8_t hwMode);
    void (*failure)(dcm_port_id_t port);
} identify_callback_t;

typedef struct {
    void (*started)(dcm_port_id_t port);
    void (*failed)(dcm_port_id_t port);
    void (*finished)(dcm_port_id_t port, uint8_t dstMode);
} modeswitch_callback_t;

typedef struct {
    bool (*Init)(void);
    bool (*Exit)(void);
    void (*SetCallbacks)(identify_callback_t *id, modeswitch_callback_t *mode);
    void (*DeviceStart)(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev);
    void (*DeviceStop)(dcm_port_id_t port);
    void (*Tick)(void);
} port_driver_ops_t;

#endif //DRV_PORT
