#ifndef HAL_PNP_DEFS
#define HAL_PNP_DEFS

#include <stdbool.h>
#include <stdint.h>

/**
 * Communication protocols
 */
typedef enum {
    PNP_LINK_MOTOR,
    PNP_LINK_UART,
    PNP_LINK_IIC,
    PNP_LINK_ANALOG_PIN1,
    PNP_LINK_NXTCOLOR,
} link_type_t;

/**
 * Device IDs
 */
typedef enum {
    PNP_DEVICE_SENSOR_NXT_TOUCH  = 1,
    PNP_DEVICE_SENSOR_NXT_LIGHT  = 2,
    PNP_DEVICE_SENSOR_NXT_SOUND  = 3,
    PNP_DEVICE_SENSOR_NXT_COLOR  = 4,
    PNP_DEVICE_SENSOR_NXT_SONIC  = 5,
    PNP_DEVICE_SENSOR_NXT_TEMP   = 6,
    PNP_DEVICE_MOTOR_LARGE       = 7, // essential
    PNP_DEVICE_MOTOR_MEDIUM      = 8, // essential
    PNP_DEVICE_SENSOR_EV3_TOUCH  = 16, // essential
    PNP_DEVICE_SENSOR_NXT_ENERGY = 99,
    PNP_DEVICE_SENSOR_EV3_COLOR  = 29, // essential
    PNP_DEVICE_SENSOR_EV3_SONIC  = 30, // essential
    PNP_DEVICE_SENSOR_EV3_GYRO   = 32, // essential
    PNP_DEVICE_SENSOR_EV3_IR     = 33, // essential
} device_type_t;

typedef enum {
    PNP_MODE_NXT_TOUCH_PRESS = 0,
} nxt_touch_mode_t;

typedef enum {
    PNP_MODE_NXT_LIGHT_REFLECT = 0,
    PNP_MODE_NXT_LIGHT_AMBIENT = 1,
} nxt_light_mode_t;

typedef enum {
    PNP_MODE_NXT_SOUND_DB  = 0,
    PNP_MODE_NXT_SOUND_DBA = 1,
} nxt_sound_mode_t;

typedef enum {
    PNP_MODE_NXT_COLOR_BACKGROUND = 0,
    PNP_MODE_NXT_COLOR_RED        = 1,
    PNP_MODE_NXT_COLOR_GREEN      = 2,
    PNP_MODE_NXT_COLOR_BLUE       = 3,
    PNP_MODE_NXT_COLOR_RGB        = 4,
    PNP_MODE_NXT_COLOR_COLORID    = 5,
} nxt_color_mode_t;

typedef enum {
    PNP_MODE_NXT_SONIC_OFF           = 0,
    PNP_MODE_NXT_SONIC_SINGLE_CM     = 1,
    PNP_MODE_NXT_SONIC_CONTINUOUS_CM = 2,
    PNP_MODE_NXT_SONIC_LISTEN        = 3,
} nxt_sonic_mode_t;

typedef enum {
    PNP_MODE_NXT_TEMP_TEMPERATURE = 0,
} nxt_temp_mode_t;

typedef enum {
    PNP_MODE_MOTOR_PWM = 0,
} motor_mode_t;

typedef enum {
    PNP_MODE_EV3_TOUCH_PRESS = 0,
} ev3_touch_mode_t;

typedef enum {
    PNP_MODE_NXT_ENERGY_VOLTAGE_IN  = 0,
    PNP_MODE_NXT_ENERGY_CURRENT_IN  = 1,
    PNP_MODE_NXT_ENERGY_VOLTAGE_OUT = 2,
    PNP_MODE_NXT_ENERGY_CURRENT_OUT = 3,
    PNP_MODE_NXT_ENERGY_STORED      = 4,
    PNP_MODE_NXT_ENERGY_POWER_IN    = 5,
    PNP_MODE_NXT_ENERGY_POWER_OUT   = 6,
    PNP_MODE_NXT_ENERGY_ALL         = 7,
} nxt_energy_mode_t;

typedef enum {
    PNP_MODE_EV3_COLOR_REFLECT = 0,
    PNP_MODE_EV3_COLOR_AMBIENT = 1,
    PNP_MODE_EV3_COLOR_COLORID = 2,
    PNP_MODE_EV3_COLOR_REF_RAW = 3,
    PNP_MODE_EV3_COLOR_RGB_RAW = 4,
    PNP_MODE_EV3_COLOR_CALREAD = 5,
} ev3_color_mode_t;

typedef enum {
    PNP_MODE_EV3_SONIC_DISTANCE_CM = 0,
    PNP_MODE_EV3_SONIC_DISTANCE_IN = 1,
    PNP_MODE_EV3_SONIC_LISTEN      = 2,
    PNP_MODE_EV3_SONIC_SINGLE_CM   = 3,
    PNP_MODE_EV3_SONIC_SINGLE_IN   = 4,
} ev3_sonic_mode_t;

typedef enum {
    PNP_MODE_EV3_GYRO_ANGLE     = 0,
    PNP_MODE_EV3_GYRO_RATE      = 1,
    PNP_MODE_EV3_GYRO_RATE_FAST = 2,
    PNP_MODE_EV3_GYRO_COMBINED  = 3,
} ev3_gyro_mode_t;

typedef enum {
    PNP_MODE_EV3_IR_PROXIMITY = 0,
    PNP_MODE_EV3_IR_SEEKER    = 1,
    PNP_MODE_EV3_IR_REMOTE    = 2,
} ev3_ir_mode_t;

#endif //HAL_PNP_DEFS
