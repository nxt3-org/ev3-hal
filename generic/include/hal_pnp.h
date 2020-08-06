#ifndef HAL_PNP
#define HAL_PNP

#include "hal_pnp_defs.h"

extern bool Hal_Pnp_RefAdd(void);
extern bool Hal_Pnp_RefDel(void);
extern void Hal_Pnp_Tick(void);

extern pnp_link_t Hal_Pnp_GetLink(int port, bool output);
extern pnp_type_t Hal_Pnp_GetDevice(int port, bool output);
extern bool Hal_Pnp_Restart(int inputPort);

extern bool Hal_Pnp_IsReady(int inputPort);
extern void Hal_Pnp_SetType(int port, bool output, hal_nxt_type_t type);

extern bool Hal_Pnp_GetPins(int inputPort, struct hal_pins *pins);
extern bool Hal_Pnp_SetPins(int inputPort, struct hal_pins pins);

extern void Hal_Pnp_SetDigiDir(struct hal_pins *pins, pin_t num, bool out);
extern void Hal_Pnp_SetDigiOut(struct hal_pins *pins, pin_t num, bool high);
extern void Hal_Pnp_GetDigiIn(struct hal_pins *pins, pin_t num, uint8_t *mask);
extern void Hal_Pnp_SetPower(struct hal_pins *pins, pwr_aux_t mode);

#endif //HAL_PNP
