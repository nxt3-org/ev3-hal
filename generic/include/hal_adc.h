#ifndef HAL_ADC
#define HAL_ADC

#include "hal_adc_defs.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct hal_adc_ops hal_adc_ops_t;
typedef struct hal_adc_dev hal_adc_dev_t;
typedef struct hal_pins    hal_pins_t;

struct hal_adc_ops {
    bool (*is_valid)(hal_adc_dev_t *dev);
    uint16_t (*read_adc)(hal_adc_dev_t *dev);
    bool (*color_active)(hal_adc_dev_t *dev);
    bool (*read_color_adc1)(hal_adc_dev_t *dev, uint16_t *rawAD);
    bool (*read_color_adc4)(hal_adc_dev_t *dev, uint16_t *rawAD);
    bool (*read_color_caldata)(hal_adc_dev_t *dev, CALDATA *out);
    void (*set_type)(hal_adc_dev_t *dev, hal_adc_type_t type);
    void (*load_pins)(hal_adc_dev_t *dev);
    void (*store_pins)(hal_adc_dev_t *dev);
    void (*tick)(hal_adc_dev_t *dev);
};

#define PIN_LOW 0
#define PIN_HIGH 1
#define DIR_IN 0
#define DIR_OUT 1
typedef enum {
    POWER_AUX_OFF = 0,
    POWER_AUX_RCX = 1,
    POWER_AUX_9V  = 2
}                          pwr_aux_t;

struct hal_pins {
    uint8_t d0_out : 1;
    uint8_t d1_out : 1;
    uint8_t d0_dir : 1;
    uint8_t d1_dir : 1;
    uint8_t d0_in : 1;
    uint8_t d1_in : 1;
    uint8_t pwr_mode : 2;
};

struct hal_adc_dev {
    const hal_adc_ops_t *ops;

    int        port;
    hal_pins_t pins;
};

// ADC "controller"
// to be implemented by NXT3 core
extern bool Hal_AdcHost_Attach(hal_adc_dev_t *device, int port);
extern bool Hal_AdcHost_Detach(int port);
extern bool Hal_AdcHost_Present(int port);

// ADC "device"
// implemented locally in HAL core (proxy for struct hal_adc_ops)
extern bool Hal_AdcDev_JustAttached(hal_adc_dev_t *dev, int port);
extern bool Hal_AdcDev_JustDetached(hal_adc_dev_t *dev);
extern bool Hal_AdcDev_IsValid(hal_adc_dev_t *dev);
extern uint16_t Hal_AdcDev_ReadAdc(hal_adc_dev_t *dev);
extern bool Hal_AdcDev_IsColorPresent(hal_adc_dev_t *dev);
extern bool Hal_AdcDev_ReadColorAdc1(hal_adc_dev_t *dev, uint16_t *rawAD);
extern bool Hal_AdcDev_ReadColorAdc4(hal_adc_dev_t *dev, uint16_t *rawAD);
extern bool Hal_AdcDev_ReadColorCalib(hal_adc_dev_t *dev, CALDATA *out);
extern void Hal_AdcDev_SetType(hal_adc_dev_t *dev, hal_adc_type_t type);
extern void Hal_AdcDev_LoadPins(hal_adc_dev_t *dev);
extern void Hal_AdcDev_StorePins(hal_adc_dev_t *dev);
extern void Hal_AdcDev_Tick(hal_adc_dev_t *dev);
extern void Hal_AdcDev_SetDigiDir(hal_adc_dev_t *dev, pin_t num, bool out);
extern void Hal_AdcDev_SetDigiOut(hal_adc_dev_t *dev, pin_t num, bool high);
extern void Hal_AdcDev_GetDigiIn(hal_adc_dev_t *dev, pin_t num, uint8_t *mask);
extern void Hal_AdcDev_SetPower(hal_adc_dev_t *dev, pwr_aux_t mode);

// ADC "manager"
// to be implemented by the HAL
extern bool Hal_AdcMgr_RefAdd(void);
extern bool Hal_AdcMgr_RefDel(void);
extern void Hal_AdcMgr_Tick(void);

#endif //HAL_ADC
