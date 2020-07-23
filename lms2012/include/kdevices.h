#ifndef EV3_KDEVICES
#define EV3_KDEVICES

#include "kdev_core.h"
#include "hal_adc_defs.h"

#define NO_BUTTONS 6
#define NO_OUTPUTS 4
#define NO_INPUTS 4
#define KERNEL_DATALOG_ENTRIES 300

typedef struct {
    unsigned char isPressed[NO_BUTTONS];
} ui_mmap_t;

typedef struct {
    uint32_t moveDegrees;
    uint8_t  speed;
    uint32_t senseDegrees;
} one_motor_mmap_t;

typedef struct {
    one_motor_mmap_t motors[NO_OUTPUTS];
} motor_mmap_t;

typedef enum __attribute__((packed)) {
    FIFO_EMPTY      = 0,
    FIFO_PROCESSING = 1,
} sound_kstate_t;

typedef struct {
    sound_kstate_t fifo_state;
    uint8_t        padding[3];
} sound_mmap_t;

typedef struct {
    CALDATA  Calibration;
    uint16_t Crc16;
    uint16_t Adc_Color[NO_OF_COLORS];
    uint16_t Unknown[NO_OF_COLORS];
} nxtcolor_t;

typedef struct {
    int16_t    Adc_Pin1_S[NO_INPUTS];
    int16_t    Adc_Pin6_S[NO_INPUTS];
    int16_t    Adc_Pin5_M[NO_OUTPUTS];
    int16_t    Adc_Temp_Battery;
    int16_t    Adc_Current_Motor;
    int16_t    Adc_Current_Battery;
    int16_t    Adc_Voltage_Battery;
    int16_t    Buffer_Pin1_S[NO_INPUTS][KERNEL_DATALOG_ENTRIES];
    int16_t    Buffer_Pin6_S[NO_INPUTS][KERNEL_DATALOG_ENTRIES];
    uint16_t   Buffer_LastPtr[NO_INPUTS];
    uint16_t   Buffer_WritePtr[NO_INPUTS];
    uint16_t   Buffer_ReadPtr[NO_INPUTS];
    nxtcolor_t Aux_NxtColor[NO_INPUTS];
    int16_t    Aux_Pin5_M[NO_OUTPUTS];
    uint8_t    Aux_NewData[NO_INPUTS];
    uint8_t    Dcm_InType[NO_INPUTS];
    uint8_t    Dcm_InLink[NO_INPUTS];
    uint8_t    Dcm_OutType[NO_OUTPUTS];
    uint8_t    Dcm_OutLink[NO_OUTPUTS];
    uint16_t   Aux_MsecCounter;
} analog_mmap_t;

typedef enum __attribute__((packed)) {
    FORMAT_S8  = 0x0,
    FORMAT_S16 = 0x1,
    FORMAT_S32 = 0x2,
    FORMAT_FLT = 0x3
} uart_format_t;

#define SENSOR_NAME_CHARS 11
#define SENSOR_NAME_BYTES 12
#define UNIT_CHARS 4
#define UNIT_BYTES 5
typedef struct {
    char          Name[SENSOR_NAME_BYTES];
    uint8_t       Device;
    uint8_t       Link;
    uint8_t       Mode;
    uart_format_t InterpretAs;
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
} typedata_t;

typedef enum __attribute__((packed)) {
    UART_FLAG_PNP_REQUEST  = 0x01,
    UART_FLAG_DATA_READY   = 0x08,
    UART_FLAG_SEND_COMMAND = 0x10,
} uart_flags_t;

#define MAX_DEVICE_MODES 8
#define UART_DATA_BYTES 32
typedef struct {
    typedata_t Meta[NO_INPUTS][MAX_DEVICE_MODES];
    uint16_t   Buffer_Age[NO_INPUTS][KERNEL_DATALOG_ENTRIES];
    uint8_t    Buffer_Data[NO_INPUTS][KERNEL_DATALOG_ENTRIES][UART_DATA_BYTES];
    uint16_t   Buffer_LastPtr[NO_INPUTS];
    uint16_t   Buffer_WritePtr[NO_INPUTS];
    uint8_t    Flags[NO_INPUTS];
    uint8_t    Command_Data[NO_INPUTS][UART_DATA_BYTES];
    uint8_t    Command_Length[NO_INPUTS];
} uart_mmap_t;

typedef struct {
    uint8_t usbSpeed;
    uint8_t padding[3];
} usbdev_mmap_t;

typedef enum __attribute__((packed)) {
    IIC_FLAG_DATA_READY   = 0x08,
    IIC_FLAG_SEND_COMMAND = 0x10,
} iic_flags_t;

#define IIC_DATA_BYTES 32
typedef struct {
    typedata_t Meta[NO_INPUTS][MAX_DEVICE_MODES];
    uint16_t   Buffer_Age[NO_INPUTS][KERNEL_DATALOG_ENTRIES];
    uint8_t    Buffer_Data[NO_INPUTS][KERNEL_DATALOG_ENTRIES][IIC_DATA_BYTES];
    uint16_t   Buffer_LastPtr[NO_INPUTS];
    uint16_t   Buffer_WritePtr[NO_INPUTS];
    uint8_t    Flags[NO_INPUTS];
    uint8_t    PnpRequest[NO_INPUTS];
    uint8_t    Command_Data[NO_INPUTS][IIC_DATA_BYTES];
    uint8_t    Command_Length[NO_INPUTS];
} iic_mmap_t;

typedef void                     __ide_formatter_sucks;

typedef KDEV_TYPE(ui_mmap_t)     DeviceUi_t;
typedef KDEV_TYPE(void)          DevicePwm_t;
typedef KDEV_TYPE(motor_mmap_t)  DeviceTacho_t;
typedef KDEV_TYPE(sound_mmap_t)  DeviceSound_t;
typedef KDEV_TYPE(uint8_t)       DeviceDisplay_t;
typedef KDEV_TYPE(uart_mmap_t)   DeviceUart_t;
typedef KDEV_TYPE(void)          DeviceTTY_t;
typedef KDEV_TYPE(analog_mmap_t) DeviceAnalog_t;
typedef KDEV_TYPE(void)          DeviceConnMgr_t;
typedef KDEV_TYPE(iic_mmap_t)    DeviceI2C_t;
typedef KDEV_TYPE(usbdev_mmap_t) DeviceUsbDev_t;
typedef KDEV_TYPE(void)          DeviceBt_t;
typedef KDEV_TYPE(void)          DeviceUpdate_t;
typedef KDEV_TYPE(void)          DevicePower_t;

extern DeviceUi_t      DeviceUi;
extern DevicePwm_t     DevicePwm;
extern DeviceTacho_t   DeviceTacho;
extern DeviceSound_t   DeviceSound;
extern DeviceDisplay_t DeviceDisplay;
extern DeviceUart_t    DeviceUart;
extern DeviceTTY_t     DeviceTTY;
extern DeviceAnalog_t  DeviceAnalog;
extern DeviceConnMgr_t DeviceConnMgr;
extern DeviceI2C_t     DeviceI2C;
extern DeviceUsbDev_t  DeviceUsbDev;
extern DeviceBt_t      DeviceBt;
extern DeviceUpdate_t  DeviceUpdate;
extern DevicePower_t   DevicePower;

#endif //EV3_KDEVICES
