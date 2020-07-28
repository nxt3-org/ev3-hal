#ifndef HAL_MOTOR_PRIVATE
#define HAL_MOTOR_PRIVATE

#include "hal_motor.h"
#include "hal_pnp_defs.h"

typedef struct {
    motor_stop_t stopMode;
    int8_t       dutyCycle;
    bool isActive;
    bool isPresent;
} port_instance_t;

typedef struct {
    int                 refCount;
    port_instance_t     ports[4];
} mod_motor_t;

extern mod_motor_t Mod_Motor;

extern bool Hal_Motor_DeviceAttached(uint8_t mPort, pnp_link_t link, pnp_device_t device, uint8_t hwMode);
extern bool Hal_Motor_DeviceDetached(uint8_t mPort);

#endif //HAL_MOTOR_PRIVATE
