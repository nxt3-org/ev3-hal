#ifndef HAL_PNP_PRIVATE
#define HAL_PNP_PRIVATE

#include "hal_pnp.h"
#include "adapter.h"
#include "interface.h"

typedef struct {
    const interface_t *Interface;
    pnp_link_t        DetectedLink;
    pnp_type_t        DetectedType;
    adapter_t         *Adapter;
    struct hal_pins   EmulatedPins;
    hal_nxt_type_t    EmulationTarget;
    adapter_factory_t LastAdapterFactory;
    dcm_link_t        LinkFromDcm;
    dcm_type_t        TypeFromDcm;
} pnp_port_t;

typedef struct {
    int        refCount;
    pnp_port_t Ports[8];
} mod_pnp_t;

extern mod_pnp_t Mod_Pnp;

#endif //HAL_PNP_PRIVATE
