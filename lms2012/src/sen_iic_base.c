#include "sen_iic_base.h"

uint8_t lego_iic_read(lego_iic_mem_t *self, uint8_t reg) {
    if (reg >= 0x00 && reg < 0x40) {
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
