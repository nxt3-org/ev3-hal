#ifndef EV3_US_PRIVATE
#define EV3_US_PRIVATE

#include <stdint.h>
#include <stdbool.h>
#include <hal_iic.h>
#include <hal_adc.h>
#include <io/core/interface.h>
#include "iic_base.h"
#include "io/core/adapter.h"

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
    hal_iic_dev_t     iic;
    adapter_t         dev;
    lego_iic_mem_t    layout;
    lego_sonic_data_t data;
    const interface_t *iface;
    uint8_t           port;
} ev3_sonic_t;

#define SONIC_REG_INTERVAL      0x40
#define SONIC_REG_MODE          0x41
#define SONIC_REG_ZERO          0x50
#define SONIC_REG_SCALE_FACTOR  0x51
#define SONIC_REG_SCALE_DIVISOR 0x52
#define SONIC_ADDRESS           0x01

#endif //EV3_US_PRIVATE
