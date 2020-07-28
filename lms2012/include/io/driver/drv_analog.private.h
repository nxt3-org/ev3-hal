#ifndef DRV_ANALOG_PRIVATE
#define DRV_ANALOG_PRIVATE

#include "drv_analog.h"

#define AUTOID_TOLERANCE 50
#define AUTOID_EV3_TOUCH 417
#define AUTOID_IS(SENSOR, V)  (( ((AUTOID_ ## SENSOR)-AUTOID_TOLERANCE) <= (V) ) && \
                               ( ((AUTOID_ ## SENSOR)+AUTOID_TOLERANCE) >= (V) ))

typedef struct {
    float siMin;
    float siMax;
    float pctMin;
    float pctMax;
    float rawMin;
    float rawMax;
    bool present;
    bool pin6;
    pnp_device_t device;
} analog_port_t;

typedef struct {
    int                   refCount;
    identify_callback_t   *idCalls;
    modeswitch_callback_t *modeCalls;
    analog_port_t         ports[4];
} drv_analog_t;

extern drv_analog_t Drv_Analog;

extern bool Drv_Analog_RefAdd(void);
extern bool Drv_Analog_RefDel(void);

extern void Drv_Analog_PnpStart(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev);
extern void Drv_Analog_PnpStop(dcm_port_id_t port);
extern void Drv_Analog_PnpSetCallbacks(identify_callback_t *id, modeswitch_callback_t *mode);

#endif //DRV_ANALOG_PRIVATE
