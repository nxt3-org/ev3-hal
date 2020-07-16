#ifndef HAL_MOTOR_PRIVATE
#define HAL_MOTOR_PRIVATE

#include "hal_motor.h"

#define CMD_PROGRAM_START 0x03
typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
} pwm_req_program_start;

#define CMD_PROGRAM_STOP 0x02
typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
} pwm_req_program_stop;

#define CMD_SET_MOTOR_TYPE 0xA1
#define MOTOR_DCM_CONN_TACHO  125
#define MOTOR_DCM_CONN_NONE   125
#define MOTOR_DCM_TYPE_NONE   126
#define MOTOR_DCM_TYPE_NXT    7
#define MOTOR_DCM_TYPE_LARGE  9
#define MOTOR_DCM_TYPE_MEDIUM 8
typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t types[4];
} pwm_req_set_motor_type;

#define CMD_OUTPUT_START 0xA6
typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
} pwm_req_output_start;

#define CMD_OUTPUT_STOP 0xA3
typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
    uint8_t doBrake;
} pwm_req_output_stop;

#define CMD_SET_DUTY_CYCLE 0xA4
typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
    int8_t  power;
} pwm_req_set_duty_cycle;

#define CMD_RESET_TACHO 0xB2
typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
} pwm_req_reset_tacho;


#define CMD_SET_POLARITY 0xA7
#define POLARITY_FORWARD (+1)
#define POLARITY_REVERSE (-1)
#define POLARITY_TOGGLE  (+1)
typedef struct __attribute__((__packed__)) {
    uint8_t cmd;
    uint8_t mask;
    int8_t  multiplier;
} pwm_req_set_polarity;


typedef struct {
    motor_type_t device;
    motor_stop_t stopMode;
    int8_t       dutyCycle;
    bool isActive;
    bool changed;
} port_instance_t;

typedef struct {
    int             refCount;
    port_instance_t ports[4];
} mod_motor_t;

extern bool doStart(motor_port_t port);
extern bool doStop(motor_port_t port);
extern void doScanDCM(uint8_t *motorTypes, uint8_t *pUploadMask);

extern mod_motor_t Mod_Motor;

#endif //HAL_MOTOR_PRIVATE
