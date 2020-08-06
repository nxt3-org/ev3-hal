#ifndef HWDB
#define HWDB

#include <stdint.h>
#include <stdbool.h>
#include "hal_pnp_defs.h"
#include "adapter.h"

typedef enum __attribute__((packed)) {
    FORMAT_S8  = 0x0,
    FORMAT_S16 = 0x1,
    FORMAT_S32 = 0x2,
    FORMAT_FLT = 0x3
} data_format_t;

typedef enum __attribute__((packed)) {
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

typedef enum __attribute__((packed)) {
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
    DCM_DEV_ERROR         = 127,
} dcm_type_t;

extern pnp_link_t IdentifyLink(dcm_link_t dcm);
extern pnp_type_t IdentifyDevice(dcm_type_t dcm);

#define SENSOR_NAME_CHARS 11
#define SENSOR_NAME_BYTES 12
#define UNIT_CHARS 4
#define UNIT_BYTES 5
typedef struct {
    char          Name[SENSOR_NAME_BYTES];
    dcm_type_t    Device;
    dcm_link_t    Link;
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
    int16_t       AdcAutoId;
    char          PinSetup;
    char          Unit[UNIT_BYTES];
    uint16_t      _padding;
} typedata_t;

#define IIC_NAME_CHARS 8
#define IIC_NAME_BYTES 9
#define IIC_MSG_BYTES 4
typedef struct {
    char     IicManufacturer[IIC_NAME_BYTES];
    char     IicProduct[IIC_NAME_BYTES];
    uint32_t SetupMsg;
    uint8_t  SetupLength;
    uint32_t PollMsg;
    uint8_t  PollLength;
    uint8_t  ReadLength;
} iic_info_t;

typedef struct {
    typedata_t Main;
    iic_info_t Iic;
} typedb_entry_t;

typedef struct {
    pnp_type_t        Type;
    hal_nxt_type_t    Mode;
    adapter_factory_t Factory;
} adapter_info_t;

extern bool HwDb_RefAdd(void);
extern bool HwDb_RefDel(void);
extern void HwDb_Bugfix(typedb_entry_t *entry);

extern const typedb_entry_t *HwDb_FindDefaultForAutoID(dcm_link_t link, int16_t idVoltage);
extern const typedb_entry_t *HwDb_FindDefaultForType(dcm_link_t link, dcm_type_t type);
extern const typedb_entry_t *HwDb_FindForMode(dcm_link_t link, dcm_type_t type, uint8_t hwMode);
extern adapter_factory_t HwDb_FindAdapter(pnp_type_t type, hal_nxt_type_t emulation);

#endif //HWDB
