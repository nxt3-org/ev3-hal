#ifndef SEN_DUMMY_US
#define SEN_DUMMY_US

#include "hal_iic.h"
#include "sen_iic_base.h"

typedef enum __attribute__((packed)) {
    SONIC_MODE_OFF,
    SONIC_MODE_SINGLE,
    SONIC_MODE_CONTINUOUS,
    SONIC_MODE_LISTEN,
    SONIC_MODE_RESET
} sonic_mode_t;

typedef struct {
    uint8_t      measurement_interval;
    sonic_mode_t command_state;
    uint8_t      value[8];
    uint8_t      padding[6];
    uint8_t      actual_zero;
    uint8_t      actual_scale_factor;
    uint8_t      actual_scale_divisor;
} lego_sonic_data_t;

typedef struct {
    hal_iic_dev_t     link;
    lego_iic_mem_t    layout;
    lego_sonic_data_t data;
    int               value;
    bool valueUp;
} dummy_sonic_t;

#define SONIC_REG_INTERVAL      0x40
#define SONIC_REG_MODE          0x41
#define SONIC_REG_ZERO          0x50
#define SONIC_REG_SCALE_FACTOR  0x51
#define SONIC_REG_SCALE_DIVISOR 0x52

#define SONIC_ADDRESS 0x01

extern dummy_sonic_t *createSonic(void);
extern void deleteSonic(dummy_sonic_t *dev);


#endif //SEN_DUMMY_US
