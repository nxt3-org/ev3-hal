#include "sen_ev3_us.h"
#include "sen_ev3_us.private.h"
#include "hal_general.h"
#include <memory.h>
#include <malloc.h>
#include <drv_uart.h>

#define link_to_sonic(dev) container_of(dev, ev3_sonic_t, link)
#define sen_to_sonic(sen) container_of(sen, ev3_sonic_t, dev)

static hal_iic_ops_t iic_ops = {
    .prewrite = sonic_prewrite,
    .write    = sonic_write,
    .preread  = sonic_preread,
    .read     = sonic_read,
    .cancel   = NULL,
    .tick     = NULL,
};

static sensor_ops_t sensor_ops = {
    .attach = sonic_attach,
    .detach = sonic_detach,
    .destroy = sonic_destroy,
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

sensor_dev_t *Sensor_EV3Sonic_Create(int port) {
    ev3_sonic_t *sonic = malloc(sizeof(ev3_sonic_t));
    if (sonic) {
        memset(sonic, 0x00, sizeof(ev3_sonic_t));
        sonic->link.ops                  = &iic_ops;
        sonic->dev.ops                   = &sensor_ops;
        sonic->layout.header             = &dummy_sonic_header;
        sonic->layout.data               = (uint8_t *) &sonic->data;
        sonic->layout.data_len           = sizeof(lego_sonic_data_t);
        sonic->data.measurement_interval = 0x15;
        sonic->data.command_state        = SONIC_MODE_OFF;
        sonic->data.actual_zero          = dummy_sonic_header.factory_zero;
        sonic->data.actual_scale_factor  = dummy_sonic_header.factory_scale_factor;
        sonic->data.actual_scale_divisor = dummy_sonic_header.factory_scale_divisor;
        sonic->port                      = port;
    }
    return &sonic->dev;
}

hal_iic_result_t sonic_prewrite(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    return addr != SONIC_ADDRESS ? HAL_IIC_RESULT_ERROR : HAL_IIC_RESULT_DONE;
}

void sonic_write(hal_iic_dev_t *self, uint8_t addr, uint8_t reg, uint8_t value) {
    ev3_sonic_t *sonic = link_to_sonic(self);
    return lego_iic_write(&sonic->layout, reg, value);
}

hal_iic_result_t sonic_preread(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    if (addr != SONIC_ADDRESS)
        return HAL_IIC_RESULT_ERROR;

    ev3_sonic_t *sonic = link_to_sonic(self);
    memset(sonic->data.value, 0, 8);
    if (sonic->data.command_state == SONIC_MODE_CONTINUOUS) {
        float fltValue;
        if (!Drv_Uart_SiValue(sonic->port, 0, &fltValue))
            fltValue = 0.0f;

        int value = (int)fltValue;
        sonic->data.value[0] = 0xFF & value;
    }
    return HAL_IIC_RESULT_DONE;
}

uint8_t sonic_read(hal_iic_dev_t *self, uint8_t addr, uint8_t reg) {
    ev3_sonic_t *sonic = link_to_sonic(self);
    return lego_iic_read(&sonic->layout, reg);
}

bool sonic_attach(sensor_dev_t *dev) {
    ev3_sonic_t *this = sen_to_sonic(dev);
    return Hal_IicHost_Attach(&this->link, this->port);
}

void sonic_detach(sensor_dev_t *dev) {
    ev3_sonic_t *this = sen_to_sonic(dev);
    Hal_IicHost_Detach(this->port);
}

void sonic_destroy(sensor_dev_t *dev) {
    ev3_sonic_t *this = sen_to_sonic(dev);
    free(this);
}
