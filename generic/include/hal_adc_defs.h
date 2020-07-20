#ifndef HAL_ADC_DEFS
#define HAL_ADC_DEFS

#include <stdint.h>
#include <stdbool.h>

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
    NO_OF_SENSOR_TYPES = 18
} hal_adc_type_t;

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
} hal_adc_mode_t;

#define   NO_OF_INPUTS     4
#define   pMapInput        ((IOMAPINPUT*)(pHeaders[ENTRY_INPUT]->pIOMap))

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

enum {
    INVALID_DATA = 0x01
};

/* Constants related to calling cInputPinFunc */
enum {
    PINDIR,
    SETPIN,
    CLEARPIN,
    READPIN
};

/* Constants related to Colorstruct */
typedef enum {
    RED,
    GREEN,
    BLUE,
    BLANK,
    NO_OF_COLORS
} color_t;


/* Constants related to color sensor value using */
/* Color sensor as color detector                */
enum {
    BLACKCOLOR  = 1,
    BLUECOLOR   = 2,
    GREENCOLOR  = 3,
    YELLOWCOLOR = 4,
    REDCOLOR    = 5,
    WHITECOLOR  = 6
};


/* Constants related to Color CalibrationState */
/* When STARTCAL is TRUE then calibration is   */
/* in progress                                 */
enum {
    SENSORCAL  = 0x01,
    SENSOROFF  = 0x02,
    RUNNINGCAL = 0x20,
    STARTCAL   = 0x40,
    RESETCAL   = 0x80,
};

enum {
    CAL_POINT_0,
    CAL_POINT_1,
    CAL_POINT_2,
    NO_OF_POINTS
};

typedef struct {
    uint32_t Calibration[NO_OF_POINTS][NO_OF_COLORS];
    uint16_t CalLimits[NO_OF_POINTS - 1];
} CALDATA;

#endif //HAL_ADC_DEFS
