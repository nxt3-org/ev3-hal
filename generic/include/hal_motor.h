#ifndef HAL_MOTOR
#define HAL_MOTOR

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    MOTOR_PORT_A,
    MOTOR_PORT_B,
    MOTOR_PORT_C,
    MOTOR_PORT_D
} motor_port_t;

typedef enum {
    STOP_MODE_BRAKE,
    STOP_MODE_COAST
} motor_stop_t;

extern bool Hal_Motor_RefAdd(void);
extern bool Hal_Motor_RefDel(void);

extern bool Hal_Motor_SetStopMode(motor_port_t port, motor_stop_t mode);
extern bool Hal_Motor_PushPwm(motor_port_t port, int8_t percent);
extern bool Hal_Motor_GetTacho(motor_port_t port, int32_t *pDegrees);
extern bool Hal_Motor_ResetTacho(motor_port_t port);

#endif //HAL_MOTOR
