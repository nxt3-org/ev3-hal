#ifndef HAL_PNP
#define HAL_PNP

#include "hal_pnp_defs.h"

extern bool Hal_Pnp_RefAdd(void);
extern bool Hal_Pnp_RefDel(void);
extern void Hal_Pnp_Tick(void);
extern bool Hal_Pnp_GetLink(uint8_t port, bool output, pnp_link_t *pLink);
extern bool Hal_Pnp_GetDevice(uint8_t port, bool output, pnp_device_t *pDevice);
extern bool Hal_Pnp_GetHwMode(uint8_t port, bool output, uint8_t *pMode);
extern bool Hal_Pnp_GetEmulatedMode(uint8_t port, bool output, uint8_t *pMode);
extern bool Hal_Pnp_IsReady(uint8_t port, bool output);
extern bool Hal_Pnp_IsSwitching(uint8_t port, bool output);


#endif //HAL_PNP
