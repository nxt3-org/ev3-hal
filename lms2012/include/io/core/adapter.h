#ifndef ADAPTER
#define ADAPTER

#include "stdbool.h"
#include <hal_pnp_defs.h>

typedef struct adapter_ops adapter_ops_t;
typedef struct adapter     adapter_t;

typedef enum {
    READY_DEVICE_NOT_PRESENT,
    READY_NOT_SIGNALLED,
    READY_SIGNALLED_YES,
    READY_SIGNALLED_NO
}                          ready_status_t;

struct adapter_ops {
    bool (*Attach)(adapter_t *dev);
    void (*Detach)(adapter_t *dev);
    void (*Destroy)(adapter_t *dev);
    bool (*IsReady)(adapter_t *dev);
    bool (*GetPins)(adapter_t *dev, struct hal_pins *pins);
    bool (*SetPins)(adapter_t *dev, struct hal_pins pins);
    void (*Tick)(adapter_t *dev);
};

struct adapter {
    adapter_ops_t *ops;
};

extern bool Adapter_Attach(adapter_t *dev);
extern bool Adapter_Detach(adapter_t *dev);
extern bool Adapter_Destroy(adapter_t *dev);
extern ready_status_t Adapter_IsReady(adapter_t *dev);
extern bool Adapter_GetPins(adapter_t *dev, struct hal_pins *pins);
extern bool Adapter_SetPins(adapter_t *dev, struct hal_pins pins);
extern void Adapter_Tick(adapter_t *dev);

struct interface;

typedef adapter_t *(*adapter_factory_t)(int port, const struct interface *iface);

extern adapter_t *Create_TTY(int port, const struct interface *iface);
extern adapter_t *Create_EV3Color(int port, const struct interface *iface);
extern adapter_t *Create_EV3Sonic(int port, const struct interface *iface);
extern adapter_t *Create_EV3Gyro(int port, const struct interface *iface);
extern adapter_t *Create_Touch(int port, const struct interface *iface);
extern adapter_t *Create_TachoMotor(int port, const struct interface *iface);
extern adapter_t *Create_NxtLight(int port, const struct interface *iface);
extern adapter_t *Create_EV3Native(int port, const struct interface *iface);


#endif //ADAPTER
