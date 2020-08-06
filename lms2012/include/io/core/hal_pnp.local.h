#ifndef HAL_PNP_LOCAL
#define HAL_PNP_LOCAL

#include "hwdb.h"

extern void Hal_Pnp_LinkFound(int port, bool output, dcm_link_t link, dcm_type_t dev);
extern void Hal_Pnp_LinkLost(int port, bool output);

extern void Hal_Pnp_HandshakeFinished(int portNo, bool output, pnp_type_t type);
extern void Hal_Pnp_HandshakeFailed(int port, bool output);

#endif //HAL_PNP_LOCAL
