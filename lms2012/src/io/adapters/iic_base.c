#include "io/adapters/iic_base.h"

uint8_t lego_iic_read(lego_iic_mem_t *self, uint8_t reg) {
    if (reg < 0x40) {
        if (reg < sizeof(lego_iic_hdr_t)) {
            return ((const uint8_t *) self->header)[reg];
        } else {
            return 0x00;
        }
    } else {
        reg -= 0x40;
        if (reg < self->data_len) {
            return self->data[reg];
        } else {
            return 0x00;
        }
    }
}

void lego_iic_write(lego_iic_mem_t *self, uint8_t reg, uint8_t val) {
    if (reg >= 0x40) {
        reg -= 0x40;
        if (reg < self->data_len)
            self->data[reg] = val;
    }
}

bool lego_iic_data_area(lego_iic_mem_t *self, uint8_t start, uint8_t length) {
    uint8_t dataStart = 0x40;
    uint8_t dataEnd   = dataStart + self->data_len;
    uint8_t ioStart   = start;
    uint8_t ioEnd     = ioStart + length;

    return (ioStart < dataEnd) && (ioEnd > dataStart);
}
