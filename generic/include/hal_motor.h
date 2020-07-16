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

typedef enum {
    MOTOR_TYPE_ERROR,
    MOTOR_TYPE_NONE,
    MOTOR_TYPE_NXT,
    MOTOR_TYPE_EV3_LARGE,
    MOTOR_TYPE_EV3_MEDIUM,
} motor_type_t;

extern bool Hal_Motor_RefAdd(void);
extern bool Hal_Motor_RefDel(void);
extern void Hal_Motor_Tick(void);

extern bool Hal_Motor_SetStopMode(motor_port_t port, motor_stop_t mode);
extern bool Hal_Motor_PushPwm(motor_port_t port, int8_t percent);
extern bool Hal_Motor_GetTacho(motor_port_t port, int32_t *pDegrees);
extern bool Hal_Motor_ResetTacho(motor_port_t port);

extern motor_type_t Hal_Motor_Detect(motor_port_t which);
extern bool Hal_Motor_GetChangeMask(uint8_t *pMask);
extern bool Hal_Motor_ClearChangeMask(uint8_t processed);

#endif //HAL_MOTOR
