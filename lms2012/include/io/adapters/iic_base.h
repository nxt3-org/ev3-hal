#ifndef IIC_BASE
#define IIC_BASE

#include <stdint.h>
#include <stdbool.h>
#include "hal_iic.h"

typedef struct {
    char    fw_version[8];
    char    vendor[8];
    char    product[8];
    uint8_t factory_zero;
    uint8_t factory_scale_factor;
    uint8_t factory_scale_divisor;
    char    units[7];
} lego_iic_hdr_t;

typedef struct {
    const lego_iic_hdr_t *header;
    uint8_t *data;
    uint8_t data_len;
} lego_iic_mem_t;

extern bool lego_iic_data_area(lego_iic_mem_t *self, uint8_t start, uint8_t length);
extern uint8_t lego_iic_read(lego_iic_mem_t *self, uint8_t reg);
extern void lego_iic_write(lego_iic_mem_t *self, uint8_t reg, uint8_t val);

#endif //IIC_BASE
