#ifndef DCM
#define DCM

#include <stdbool.h>
#include <stdint.h>

// implemented here
extern bool Dcm_RefAdd(void);
extern bool Dcm_RefDel(void);
extern void Dcm_Tick(void);
extern bool Dcm_SetPins(int port, char code);
extern int16_t Dcm_MeasureAutoID(int port, bool output);

// callbacks implemented in PNP manager
#include "io/core/hal_pnp.local.h"

#endif //DCM
