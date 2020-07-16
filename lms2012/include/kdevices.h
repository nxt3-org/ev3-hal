#ifndef EV3_KDEVICES
#define EV3_KDEVICES

#include "kdev_core.h"

typedef struct {
    unsigned char isPressed[6];
} ui_mmap_t;

typedef struct {
    uint32_t moveDegrees;
    uint8_t  speed;
    uint32_t senseDegrees;
} one_motor_mmap_t;

typedef struct {
    one_motor_mmap_t motors[4];
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
    uint8_t pad_start[5164];
    uint8_t OutputType[4];
    uint8_t OutputConnection[4];
    uint8_t pad_end[2];
} analog_mmap_t;

typedef struct {
    uint8_t dummy;
} uart_mmap_t;

typedef struct {
    uint8_t usbSpeed;
    uint8_t padding[3];
} usbdev_mmap_t;

typedef struct {
    uint8_t dummy;
} iic_mmap_t;

typedef void                     __ide_formatter_sucks;

typedef KDEV_TYPE(ui_mmap_t)     DeviceUi_t;
typedef KDEV_TYPE(void)          DevicePwm_t;
typedef KDEV_TYPE(motor_mmap_t)  DeviceTacho_t;
typedef KDEV_TYPE(sound_mmap_t)  DeviceSound_t;
typedef KDEV_TYPE(uint8_t)       DeviceDisplay_t;
typedef KDEV_TYPE(uart_mmap_t)   DeviceUart_t;
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
extern DeviceAnalog_t  DeviceAnalog;
extern DeviceConnMgr_t DeviceConnMgr;
extern DeviceI2C_t     DeviceI2C;
extern DeviceUsbDev_t  DeviceUsbDev;
extern DeviceBt_t      DeviceBt;
extern DeviceUpdate_t  DeviceUpdate;
extern DevicePower_t   DevicePower;

#endif //EV3_KDEVICES
