#include <memory.h>
#include <malloc.h>
#include <hal_general.h>
#include <io/adapters/ev3_us.private.h>

#define iic_to_sonic(dev) container_of(dev, ev3_sonic_t, iic)
#define sen_to_sonic(sen) container_of(sen, ev3_sonic_t, dev)

static lego_iic_hdr_t ultrasonic_eeprom = {
    .fw_version = "V1.0",
    .vendor = "LEGO",
    .product = "Sonar",
    .factory_zero = 0x00,
    .factory_scale_factor = 0x01,
    .factory_scale_divisor = 0x0E,
    .units = "10E-2m"
};

static bool Attach(adapter_t *dev) {
    ev3_sonic_t *this = sen_to_sonic(dev);
    return Hal_IicHost_Attach(&this->iic, this->port);
}

static void Detach(adapter_t *dev) {
    ev3_sonic_t *this = sen_to_sonic(dev);
    Hal_IicHost_Detach(this->port);
}

static void Destroy(adapter_t *dev) {
    ev3_sonic_t *this = sen_to_sonic(dev);
    free(this);
}

static hal_iic_result_t Preread(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    if (addr != SONIC_ADDRESS)
        return HAL_IIC_RESULT_ERROR;

    ev3_sonic_t *sonic = iic_to_sonic(self);

    if (!lego_iic_data_area(&sonic->layout, start, length))
        return HAL_IIC_RESULT_DONE;

    memset(sonic->data.value, 0, 8);

    float fltValue;
    if (!sonic->iface->Sensor.Measure(sonic->port, &fltValue, 1, UNIT_SI, true))
        return HAL_IIC_RESULT_ERROR;

    if (sonic->data.command_state == SONIC_MODE_CONTINUOUS) {
        sonic->data.value[0] = 0xFF & ((int) fltValue);

    } else if (sonic->data.command_state == SONIC_MODE_SINGLE) {
        sonic->data.value[0] = 0xFF & ((int) fltValue);

    } else if (sonic->data.command_state == SONIC_MODE_LISTEN) {
        sonic->data.value[0] = fltValue >= 0.5f;
    }
    return HAL_IIC_RESULT_DONE;
}

static uint8_t Read(hal_iic_dev_t *self, uint8_t reg) {
    ev3_sonic_t *sonic = iic_to_sonic(self);
    return lego_iic_read(&sonic->layout, reg);
}

static hal_iic_result_t Prewrite(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    (void) self; // not needed
    (void) start; // not checked
    (void) length; // not checked

    return addr != SONIC_ADDRESS ? HAL_IIC_RESULT_ERROR : HAL_IIC_RESULT_DONE;
}

static void Write(hal_iic_dev_t *self, uint8_t reg, uint8_t value) {
    ev3_sonic_t *sonic = iic_to_sonic(self);

    switch (reg) {
    case SONIC_REG_MODE:
        switch (value) {
        case SONIC_MODE_OFF:
        case SONIC_MODE_SINGLE:
            sonic->iface->Sensor.StartSwitch(sonic->port, PNP_MODE_EV3_SONIC_SINGLE_CM);
            break;
        case SONIC_MODE_RESET:
        case SONIC_MODE_CONTINUOUS:
            value = SONIC_MODE_CONTINUOUS;
            sonic->iface->Sensor.StartSwitch(sonic->port, PNP_MODE_EV3_SONIC_DISTANCE_CM);
            break;
        case SONIC_MODE_LISTEN:
            sonic->iface->Sensor.StartSwitch(sonic->port, PNP_MODE_EV3_SONIC_LISTEN);
            break;
        }
        lego_iic_write(&sonic->layout, SONIC_REG_MODE, value);
        break;
    case SONIC_REG_INTERVAL:
    case SONIC_REG_ZERO:
    case SONIC_REG_SCALE_FACTOR:
    case SONIC_REG_SCALE_DIVISOR:
        // ignore
        break;
    }
}

static bool IsValid(adapter_t *self) {
    ev3_sonic_t *this = sen_to_sonic(self);
    return this->iface->Ready(this->port);
}

static hal_iic_ops_t iic_ops = {
    .prewrite = Prewrite,
    .write    = Write,
    .preread  = Preread,
    .read     = Read,
};

static adapter_ops_t sensor_ops = {
    .Attach = Attach,
    .Detach = Detach,
    .Destroy = Destroy,
    .IsReady = IsValid,
};

adapter_t *Create_EV3Sonic(int port, const interface_t *iface) {
    ev3_sonic_t *sonic = malloc(sizeof(ev3_sonic_t));
    if (sonic) {
        memset(sonic, 0x00, sizeof(ev3_sonic_t));
        sonic->iic.ops                   = &iic_ops;
        sonic->dev.ops                   = &sensor_ops;
        sonic->layout.header             = &ultrasonic_eeprom;
        sonic->layout.data               = (uint8_t *) &sonic->data;
        sonic->layout.data_len           = sizeof(lego_sonic_data_t);
        sonic->data.measurement_interval = 0x15;
        sonic->data.command_state        = SONIC_MODE_CONTINUOUS;
        sonic->data.actual_zero          = ultrasonic_eeprom.factory_zero;
        sonic->data.actual_scale_factor  = ultrasonic_eeprom.factory_scale_factor;
        sonic->data.actual_scale_divisor = ultrasonic_eeprom.factory_scale_divisor;
        sonic->iface                     = iface;
        sonic->port                      = port;
        return &sonic->dev;
    }
    return NULL;
}
