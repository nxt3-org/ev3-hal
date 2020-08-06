#ifndef DCM_PRIVATE
#define DCM_PRIVATE

#include "dcm.h"

typedef struct {
    dcm_link_t Link;
    dcm_type_t Device;
} dcm_port_t;

typedef struct {
    int        refCount;
    dcm_port_t Ports[8];
} drv_dcm_t;

extern drv_dcm_t Dcm;

dcm_link_t newLink(int port, bool output);
dcm_type_t newDevice(int port, bool output);
dcm_link_t oldLink(int port, bool output);
dcm_type_t oldDevice(int port, bool output);

#endif //DCM_PRIVATE
