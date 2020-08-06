#ifndef IF_MOTOR_PRIVATE
#define IF_MOTOR_PRIVATE

#include <hal_motor.h>
#include "dcm.private.h"

#define CMD_PROGRAM_STOP    0x02
#define CMD_PROGRAM_START   0x03
#define CMD_SET_MOTOR_TYPE  0xA1
#define CMD_OUTPUT_STOP     0xA3
#define CMD_SET_DUTY_CYCLE  0xA4
#define CMD_OUTPUT_START    0xA6
#define CMD_SET_POLARITY    0xA7
#define CMD_RESET_TACHO     0xB2
#define POLARITY_FORWARD (+1)

typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
} pwm_req_program_start;

typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
} pwm_req_program_stop;

typedef struct __attribute__((__packed__)) {
    uint8_t    cmd;
    dcm_type_t types[4];
} pwm_req_set_motor_type;

typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
} pwm_req_output_start;

typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
    uint8_t doBrake;
} pwm_req_output_stop;

typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
    int8_t  power;
} pwm_req_set_duty_cycle;

typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
} pwm_req_reset_tacho;

typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
    int8_t  multiplier;
} pwm_req_set_polarity;


typedef struct {
    int                  refCount;
    const typedb_entry_t *types[4];
    bool running[4];
} drv_motor_t;

extern drv_motor_t Drv_Motor;

extern bool Drv_Motor_KernelSetPolarity(void);
extern bool Drv_Motor_KernelSetTypes(void);
extern bool Drv_Motor_KernelStart(int mPort);
extern bool Drv_Motor_KernelStop(int mPort, motor_stop_t stop);
extern bool Drv_Motor_KernelPowerOn(void);
extern bool Drv_Motor_KernelPowerOff(void);
extern bool Drv_Motor_KernelStopAll(void);
extern bool Drv_Motor_KernelSetPwm(int port, int pwm);
extern bool Drv_Motor_KernelTachoReset(int port);

#endif //IF_MOTOR_PRIVATE
