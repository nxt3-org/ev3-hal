#ifndef HAL_ADC_DEFS
#define HAL_ADC_DEFS

#include <stdint.h>
#include <stdbool.h>

#define   NO_OF_INPUTS     4
#define   pMapInput        ((IOMAPINPUT*)(pHeaders[ENTRY_INPUT]->pIOMap))

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
