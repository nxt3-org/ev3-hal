#include "kdevices.h"
#include "hal_display.private.h"

DeviceUi_t      DeviceUi      = KDEV_INIT("/dev/lms_ui", DeviceUi_t);
DevicePwm_t     DevicePwm     = KDEV_INIT_NOMM("/dev/lms_pwm");
DeviceTacho_t   DeviceTacho   = KDEV_INIT("/dev/lms_motor", DeviceTacho_t);
DeviceSound_t   DeviceSound   = KDEV_INIT("/dev/lms_sound", DeviceSound_t);
DeviceDisplay_t DeviceDisplay = KDEV_INIT_RAWMM("/dev/fb0", EV3_DISPLAY_SIZE);
DeviceUart_t    DeviceUart    = KDEV_INIT("/dev/lms_uart", DeviceUart_t);
DeviceAnalog_t  DeviceAnalog  = KDEV_INIT("/dev/lms_analog", DeviceAnalog_t);
DeviceConnMgr_t DeviceConnMgr = KDEV_INIT_NOMM("/dev/lms_dcm");
DeviceI2C_t     DeviceI2C     = KDEV_INIT("/dev/lms_iic", DeviceI2C_t);
DeviceUsbDev_t  DeviceUsbDev  = KDEV_INIT("/dev/lms_usbdev", DeviceUsbDev_t);
DeviceBt_t      DeviceBt      = KDEV_INIT_NOMM("/dev/lms_bt");
