#include "io/adapters/tty.private.h"
#include "hal_general.h"
#include <malloc.h>
#include <io/core/interface.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

#define sen_to_tty(sen) container_of(sen, sensor_tty_t, dev)
#define link_to_tty(lnk) container_of(lnk, sensor_tty_t, link)

static lego_iic_hdr_t tty_eeprom = {
    .fw_version = "V1.0",
    .vendor = "NXT3",
    .product = "Console",
    .factory_zero = 0x00,
    .factory_scale_factor = 0x00,
    .factory_scale_divisor = 0x00,
    .units = ""
};

const char *attached = "attached\r\n";
static bool tty_attach(adapter_t *dev) {
    sensor_tty_t *this = sen_to_tty(dev);
    write(STDOUT_FILENO, attached, strlen(attached));
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    return Hal_IicHost_Attach(&this->link, 0);
}

const char *detached = "detached\r\n";
static void tty_detach(adapter_t *dev) {
    (void) dev;
    Hal_IicHost_Detach(0);
    write(STDOUT_FILENO, detached, strlen(detached));
    fcntl(STDIN_FILENO, F_SETFL, 0);
}

static void tty_destroy(adapter_t *dev) {
    sensor_tty_t *this = sen_to_tty(dev);
    free(this);
}

static void tty_tick(adapter_t *dev) {
    sensor_tty_t *this     = sen_to_tty(dev);
    int          remaining = sizeof(this->data.readBuffer) - this->data.readPointer;
    if (remaining <= 0)
        return;
    uint8_t *dst = this->data.readBuffer + this->data.readPointer;
    int     ok   = read(STDIN_FILENO, dst, remaining);
    if (ok >= 0) {
        this->data.readPointer += ok;
    }
}

static hal_iic_result_t Preread(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    (void) self;
    (void) start;
    (void) length;

    if (addr != TTY_ADDRESS)
        return HAL_IIC_RESULT_ERROR;
    return HAL_IIC_RESULT_DONE;
}

static uint8_t Read(hal_iic_dev_t *self, uint8_t reg) {
    sensor_tty_t *this = link_to_tty(self);
    return lego_iic_read(&this->layout, reg);
}

static hal_iic_result_t Prewrite(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length) {
    (void) self; // not needed
    (void) start; // not checked
    (void) length; // not checked

    return addr != TTY_ADDRESS ? HAL_IIC_RESULT_ERROR : HAL_IIC_RESULT_DONE;
}

static void Write(hal_iic_dev_t *self, uint8_t reg, uint8_t value) {
    sensor_tty_t *this = link_to_tty(self);

    lego_iic_write(&this->layout, reg, value);

    if (reg == TTY_REG_COMMAND) {
        if (value == COMMAND_WRITE) {
            int len = this->data.lengthArgument;
            if (len > sizeof(this->data.writeBuffer))
                len = sizeof(this->data.writeBuffer);
            write(STDOUT_FILENO, this->data.writeBuffer, len);

        } else if (value == COMMAND_ADVANCE) {
            int len = this->data.lengthArgument;
            if (len > sizeof(this->data.readBuffer))
                len = sizeof(this->data.readBuffer);
            int remaining = sizeof(this->data.readBuffer) - len;

            memmove(this->data.readBuffer, this->data.readBuffer + len, remaining);
            memset(this->data.readBuffer + remaining, 0, len);
            this->data.readPointer = 0;
        }
    }
}

static hal_iic_ops_t iic_ops = {
    .prewrite = Prewrite,
    .preread = Preread,
    .write = Write,
    .read = Read,
    .cancel = NULL,
};

static adapter_ops_t sensor_ops = {
    .Attach = tty_attach,
    .Detach = tty_detach,
    .Destroy = tty_destroy,
    .Tick = tty_tick,
};

adapter_t *Create_TTY(int port, const interface_t *iface) {
    (void) iface; // not actually needed

    if (port != 0)
        return NULL;

    sensor_tty_t *dev = malloc(sizeof(sensor_tty_t));
    if (dev) {
        dev->link.ops        = &iic_ops;
        dev->dev.ops         = &sensor_ops;
        dev->layout.header   = &tty_eeprom;
        dev->layout.data     = (uint8_t *) &dev->data;
        dev->layout.data_len = sizeof(dev->data);
        memset(dev->layout.data, 0, dev->layout.data_len);
        return &dev->dev;
    }
    return NULL;
}
