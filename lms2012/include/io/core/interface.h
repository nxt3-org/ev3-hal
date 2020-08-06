#ifndef INTERFACE
#define INTERFACE

#include <stdbool.h>
#include <hal_motor.h>
#include <common/kdevices.h>
#include <hal_ev3sensor.h>
#include "hwdb.h"
#include "units.h"

typedef struct {
    bool (*StartSwitch)(int port, int hwMode);
    bool (*StartCommand)(int port, const uint8_t *cmd, int count);
    int (*GetMode)(int port);
    const char *(*GetModeName)(int port, int mode);
    int (*GetModes)(int port);
    const char *(*GetSiUnit)(int port);
    bool (*Measure)(int port, float *out, int slots, unit_type_t msr, bool clip);
    bool (*ResetDatalog)(int port);
    int (*ReadDatalog)(int port, int *pRepeat, float *si, float *pct, float *raw, int slots, bool clip);
} sensor_interface_t;

typedef struct {
    bool (*SetPwm)(int port, int pwm, motor_stop_t stop);
    bool (*GetTacho)(int port, int *pTacho);
    bool (*ResetTacho)(int port);
} motor_interface_t;


typedef struct interface {
    bool (*Init)(void);
    bool (*Exit)(void);
    void (*Tick)(void);

    bool (*Present)(int port);
    bool (*Ready)(int port);

    const char *(*GetDeviceName)(int port);
    pnp_type_t (*GetDevice)(int port);

    bool (*Start)(int port, dcm_link_t link, dcm_type_t type);
    void (*Stop)(int port);

    sensor_interface_t Sensor;
    motor_interface_t  Motor;
} interface_t;

extern interface_t DriverMotor;
extern interface_t DriverUart;
extern interface_t DriverAnalog;

// callbacks are in PNP manager
#include "hal_pnp.local.h"

#endif //INTERFACE
