#ifndef HAL_PNP_DEFS
#define HAL_PNP_DEFS

#include <stdbool.h>
#include <stdint.h>

typedef enum __attribute__((packed)) {
    PNP_STATE_OFF,
    PNP_STATE_HANDSHAKING,
    PNP_STATE_SWITCHING,
    PNP_STATE_RUNNING,
} port_state_t;

/**
 * Communication protocols
 */
typedef enum __attribute__((packed)) {
    PNP_LINK_NONE = -1,
    PNP_LINK_MOTOR,
    PNP_LINK_UART,
    PNP_LINK_IIC,
    PNP_LINK_ANALOG,
    PNP_LINK_NXTCOLOR,
    PNP_LINK_COUNT
} pnp_link_t;

/**
 * Device IDs
 */
typedef enum __attribute__((packed)) {
    PNP_DEVICE_UNKNOWN = -2,
    PNP_DEVICE_NONE    = -1,
    PNP_DEVICE_TTY,
    PNP_DEVICE_SENSOR_NXT_TOUCH,
    PNP_DEVICE_SENSOR_NXT_LIGHT,
    PNP_DEVICE_SENSOR_NXT_SOUND,
    PNP_DEVICE_SENSOR_NXT_COLOR,
    PNP_DEVICE_SENSOR_NXT_SONIC,
    PNP_DEVICE_SENSOR_NXT_TEMP,
    PNP_DEVICE_MOTOR_LARGE, // essential
    PNP_DEVICE_MOTOR_MEDIUM, // essential
    PNP_DEVICE_SENSOR_EV3_TOUCH, // essential
    PNP_DEVICE_SENSOR_NXT_ENERGY,
    PNP_DEVICE_SENSOR_EV3_COLOR, // essential
    PNP_DEVICE_SENSOR_EV3_SONIC, // essential
    PNP_DEVICE_SENSOR_EV3_GYRO, // essential
    PNP_DEVICE_SENSOR_EV3_IR, // essential
} pnp_type_t;

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
    PNP_MODE_EV3_COLOR_REFLECT   = 0,
    PNP_MODE_EV3_COLOR_AMBIENT   = 1,
    PNP_MODE_EV3_COLOR_COLORID   = 2,
    PNP_MODE_EV3_COLOR_REF_RAW   = 3,
    PNP_MODE_EV3_COLOR_RGB_RAW   = 4,
    PNP_MODE_EV3_COLOR_CALIBRATE = 5,
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

/* Constants related to Digital I/O */
typedef enum {
    DIGI0 = 1,
    DIGI1 = 2
} pin_t;

enum {
    CUSTOMINACTIVE = 0x00,
    CUSTOM9V       = 0x01,
    CUSTOMACTIVE   = 0x02
};

#define PIN_LOW 0
#define PIN_HIGH 1
#define DIR_IN 0
#define DIR_OUT 1
typedef enum {
    POWER_AUX_OFF = 0,
    POWER_AUX_RCX = 1,
    POWER_AUX_9V  = 2
} pwr_aux_t;

struct hal_pins {
    uint8_t d0_out : 1;
    uint8_t d1_out : 1;
    uint8_t d0_dir : 1;
    uint8_t d1_dir : 1;
    uint8_t d0_in : 1;
    uint8_t d1_in : 1;
    uint8_t pwr_mode : 2;
};

/* Constants related to sensor type  */
typedef enum __attribute__((packed)) {
    NO_SENSOR          = 0,
    SWITCH             = 1,
    TEMPERATURE        = 2,
    REFLECTION         = 3,
    ANGLE              = 4,
    LIGHT_ACTIVE       = 5,
    LIGHT_INACTIVE     = 6,
    SOUND_DB           = 7,
    SOUND_DBA          = 8,
    CUSTOM             = 9,
    LOWSPEED           = 10,
    LOWSPEED_9V        = 11,
    HIGHSPEED          = 12,
    COLORFULL          = 13,
    COLORRED           = 14,
    COLORGREEN         = 15,
    COLORBLUE          = 16,
    COLORNONE          = 17,
    COLOREXIT          = 18, /* For internal use when going from color or Lamp to no_sensor*/
    EV3SENSOR          = 19,
    NO_OF_SENSOR_TYPES = 19
} hal_nxt_type_t;

/* Constants related to sensor mode */
typedef enum __attribute__((packed)) {
    RAWMODE           = 0x00,
    BOOLEANMODE       = 0x20,
    TRANSITIONCNTMODE = 0x40,
    PERIODCOUNTERMODE = 0x60,
    PCTFULLSCALEMODE  = 0x80,
    CELSIUSMODE       = 0xA0,
    FAHRENHEITMODE    = 0xC0,
    ANGLESTEPSMODE    = 0xE0,
    SLOPEMASK         = 0x1F,
    MODEMASK          = 0xE0
} hal_nxt_mode_t;


#endif //HAL_PNP_DEFS
