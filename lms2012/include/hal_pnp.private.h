#ifndef HAL_PNP_PRIVATE
#define HAL_PNP_PRIVATE

#include "hal_pnp.h"

typedef struct {
    int refCount;
} mod_pnp_t;

extern mod_pnp_t Mod_Pnp;

#endif //HAL_PNP_PRIVATE
