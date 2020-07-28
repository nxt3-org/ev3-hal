#ifndef DRV_MOTOR
#define DRV_MOTOR

#include "hal_motor.h"
#include "drv_port.h"

extern bool Drv_Motor_RefAdd(void);
extern bool Drv_Motor_RefDel(void);

extern bool Drv_Motor_SetPwm(uint8_t mPort, motor_stop_t stop, int8_t pwm, bool wasRunning);
extern bool Drv_Motor_TachoReset(uint8_t mPort);
extern bool Drv_Motor_TachoGet(uint8_t mPort, int32_t *pTacho);

extern port_driver_ops_t DriverMotor;

#endif //DRV_MOTOR
