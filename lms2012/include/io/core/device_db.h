#ifndef DEVICE_DB
#define DEVICE_DB

#include <stdint.h>
#include <stdbool.h>
#include <hal_pnp_defs.h>

typedef enum __attribute__((packed)) {
    FORMAT_S8  = 0x0,
    FORMAT_S16 = 0x1,
    FORMAT_S32 = 0x2,
    FORMAT_FLT = 0x3
} data_format_t;

typedef enum {
    DCM_LINK_UNKNOWN      = 111,
    DCM_LINK_NXT_COLOR    = 118,
    DCM_LINK_NXT_DUMB     = 119,
    DCM_LINK_NXT_IIC      = 120,
    DCM_LINK_INPUT_DUMB   = 121,
    DCM_LINK_INPUT_UART   = 122,
    DCM_LINK_OUTPUT_DUMB  = 123,
    DCM_LINK_OUTPUT_SMART = 124,
    DCM_LINK_OUTPUT_TACHO = 125,
    DCM_LINK_NONE         = 126,
    DCM_LINK_ERROR        = 127
} dcm_link_t;

typedef enum {
    DCM_DEV_NXT_TOUCH     = 1,
    DCM_DEV_NXT_LIGHT     = 2,
    DCM_DEV_NXT_SOUND     = 3,
    DCM_DEV_NXT_COLOR     = 4,
    DCM_DEV_NXT_SONIC     = 5,
    DCM_DEV_NXT_TEMP      = 6,
    DCM_DEV_LARGE_MOTOR   = 7,
    DCM_DEV_MEDIUM_MOTOR  = 8,
    DCM_DEV_LARGE_MOTOR_2 = 9,
    DCM_DEV_EV3_TOUCH     = 16,
    DCM_DEV_EV3_COLOR     = 29,
    DCM_DEV_EV3_SONIC     = 30,
    DCM_DEV_EV3_GYRO      = 32,
    DCM_DEV_EV3_IR        = 33,
    DCM_DEV_ENERGYMETER   = 99,
    DCM_DEV_GENERIC_IIC   = 123,
    DCM_DEV_TTY           = 124,
    DCM_DEV_UNKNOWN       = 125,
    DCM_DEV_NONE          = 126,
    DCM_DEV_ERROR            = 127,
} dcm_dev_t;

#define SENSOR_NAME_CHARS 11
#define SENSOR_NAME_BYTES 12
#define UNIT_CHARS 4
#define UNIT_BYTES 5
typedef struct {
    char          Name[SENSOR_NAME_BYTES];
    uint8_t       Device;
    uint8_t       Link;
    uint8_t       Mode;
    uint8_t       Values;
    data_format_t InterpretAs;
    uint8_t       Digits;
    uint8_t       DecimalPlaces;
    uint8_t       GuiVisibleModes;
    float         RawMin;
    float         RawMax;
    float         PercentMin;
    float         PercentMax;
    float         SiMin;
    float         SiMax;
    uint16_t      ModeswitchMsec;
    uint16_t      AdcAutoId;
    char          PinSetupString;
    char          Unit[UNIT_BYTES];
    uint16_t      _padding;
} devinfo_t;

#define IIC_NAME_CHARS 8
#define IIC_NAME_BYTES 9
#define IIC_MSG_BYTES 4
typedef struct {
    char     IicManufacturer[IIC_NAME_BYTES];
    char     IicProduct[IIC_NAME_BYTES];
    uint8_t  SetupMsg[IIC_MSG_BYTES];
    uint8_t  SetupLength;
    uint8_t  PollMsg[IIC_MSG_BYTES];
    uint8_t  PollLength;
    uint8_t  ReadLength;
} iic_info_t;

typedef struct {
    pnp_link_t Link;
    pnp_device_t Device;
} nxt3_info_t;

typedef struct {
    devinfo_t   Base;
    iic_info_t  Iic;
    nxt3_info_t Nxt3;
    bool Present;
} devdb_entry_t;

#define MAX_TYPEDATA_ENTRIES 100
extern devdb_entry_t DeviceDB[MAX_TYPEDATA_ENTRIES];

#endif //DEVICE_DB
