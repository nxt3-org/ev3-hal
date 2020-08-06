#ifndef HAL_MOTOR
#define HAL_MOTOR

#include <stdbool.h>
#include <stdint.h>

typedef struct hal_motor_ops hal_motor_ops_t;
typedef struct hal_motor_dev hal_motor_dev_t;

typedef enum {
    STOP_MODE_BRAKE,
    STOP_MODE_COAST
} motor_stop_t;

typedef enum {
    MOTOR_PORT_A,
    MOTOR_PORT_B,
    MOTOR_PORT_C,
    MOTOR_PORT_D,
} motor_port_t;

struct hal_motor_ops {
    bool (*SetPwm)(hal_motor_dev_t *dev, int percent, motor_stop_t mode);
    bool (*GetTacho)(hal_motor_dev_t *dev, int *pDegrees);
    bool (*ResetTacho)(hal_motor_dev_t *dev);
};

struct hal_motor_dev {
    const hal_motor_ops_t *ops;
};

// Motor "controller"
// to be implemented by NXT3 core
extern bool Hal_MotorHost_Attach(hal_motor_dev_t *device, motor_port_t port);
extern bool Hal_MotorHost_Detach(motor_port_t port);
extern bool Hal_MotorHost_Present(motor_port_t port);

// Motor "device"
// implemented locally in HAL core (proxy for struct hal_motor_ops)
extern bool Hal_MotorDev_SetPwm(hal_motor_dev_t *dev, int percent, motor_stop_t mode);
extern bool Hal_MotorDev_GetTacho(hal_motor_dev_t *dev, int *pDegrees);
extern bool Hal_MotorDev_ResetTacho(hal_motor_dev_t *dev);

#endif //HAL_MOTOR
