#ifndef DRV_MOTOR_PRIVATE
#define DRV_MOTOR_PRIVATE

#include "drv_motor.h"
#include "drv_dcm.private.h"

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
    dcm_dev_t types[4];
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
    int       refCount;
    dcm_dev_t types[4];
    identify_callback_t *idCalls;
} drv_motor_t;

extern drv_motor_t Drv_Motor;

extern bool Drv_Motor_SetPolarity(void);
extern bool Drv_Motor_SetType(uint8_t mPort, pnp_device_t type);
extern bool Drv_Motor_Start(uint8_t mPort);
extern bool Drv_Motor_Stop(uint8_t mPort, motor_stop_t stop);
extern bool Drv_Motor_SetTypes(void);
extern bool Drv_Motor_PowerOn(void);
extern bool Drv_Motor_PowerOff(void);
extern bool Drv_Motor_StopAll(void);

extern void Drv_Motor_PnpStart(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev);
extern void Drv_Motor_PnpStop(dcm_port_id_t port);
extern void Drv_Motor_PnpSetCallbacks(identify_callback_t *id, modeswitch_callback_t *mode);

#endif //DRV_MOTOR_PRIVATE
