#include "sen_dummy_us.h"
#include "hal_general.h"
#include <memory.h>
#include <malloc.h>

#define to_sonic(dev) container_of(dev, dummy_sonic_t, link)

static hal_iic_result_t sonic_prewrite(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length);
static void sonic_write(hal_iic_dev_t *self, uint8_t addr, uint8_t reg, uint8_t value);
static hal_iic_result_t sonic_preread(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length);
static uint8_t sonic_read(hal_iic_dev_t *self, uint8_t addr, uint8_t reg);
static void sonic_tick(hal_iic_dev_t *self);

static hal_iic_ops_t dummy_sonic_ops = {
    .prewrite = sonic_prewrite,
    .write    = sonic_write,
    .preread  = sonic_preread,
    .read     = sonic_read,
    .cancel   = NULL,
    .tick     = sonic_tick,
};

static lego_iic_hdr_t dummy_sonic_header = {
    .fw_version = "V1.0",
    .vendor = "LEGO",
    .product = "Sonar",
    .factory_zero = 0x00,
    .factory_scale_factor = 0x01,
    .factory_scale_divisor = 0x0E,
    .units = "10E-2m"
};

dummy_sonic_t *createSonic(void) {
    dummy_sonic_t *sonic = malloc(sizeof(dummy_sonic_t));
    if (sonic) {
        memset(sonic, 0x00, sizeof(dummy_sonic_t));
        sonic->link.ops                  = &dummy_sonic_ops;
        sonic->link.port                 = -1;
        sonic->layout.header             = &dummy_sonic_header;
        sonic->layout.data               = (uint8_t *) &sonic->data;
        sonic->layout.data_len           = sizeof(lego_sonic_data_t);
        sonic->data.measurement_interval = 0x15;
        sonic->data.command_state        = SONIC_MODE_OFF;
        sonic->data.actual_zero          = dummy_sonic_header.factory_zero;
        sonic->data.actual_scale_factor  = dummy_sonic_header.factory_scale_factor;
        sonic->data.actual_scale_divisor = dummy_sonic_header.factory_scale_divisor;
        sonic->valueUp                   = true;
        sonic->value                     = 0;
    }
    return sonic;
}

void deleteSonic(dummy_sonic_t *dev) {
    free(dev);
}

hal_iic_result_t sonic_prewrite(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    return addr != SONIC_ADDRESS ? HAL_IIC_RESULT_ERROR : HAL_IIC_RESULT_DONE;
}

void sonic_write(hal_iic_dev_t *self, uint8_t addr, uint8_t reg, uint8_t value) {
    dummy_sonic_t *sonic = to_sonic(self);
    return lego_iic_write(&sonic->layout, reg, value);
}

hal_iic_result_t sonic_preread(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    if (addr != SONIC_ADDRESS)
        return HAL_IIC_RESULT_ERROR;

    dummy_sonic_t *sonic = to_sonic(self);
    memset(sonic->data.value, 0, 8);
    if (sonic->data.command_state == SONIC_MODE_CONTINUOUS) {
        int val = sonic->value / 10;
        sonic->data.value[0] = 0xFF & (val >> 0);
        sonic->data.value[1] = 0xFF & (val >> 8);
        sonic->data.value[2] = 0xFF & (val >> 16);
        sonic->data.value[3] = 0xFF & (val >> 24);
    }
    return HAL_IIC_RESULT_DONE;
}

uint8_t sonic_read(hal_iic_dev_t *self, uint8_t addr, uint8_t reg) {
    dummy_sonic_t *sonic = to_sonic(self);
    return lego_iic_read(&sonic->layout, reg);
}

void sonic_tick(hal_iic_dev_t *self) {
    dummy_sonic_t *sonic = to_sonic(self);
    if (sonic->valueUp) {
        sonic->value++;
        if (sonic->value == 2550)
            sonic->valueUp = false;
    } else {
        sonic->value--;
        if (sonic->value == 0)
            sonic->valueUp = true;
    }
}
