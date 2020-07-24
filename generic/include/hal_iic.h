#ifndef HAL_IIC
#define HAL_IIC

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    HAL_IIC_RESULT_ERROR,
    HAL_IIC_RESULT_PROCESSING,
    HAL_IIC_RESULT_DONE,
} hal_iic_result_t;

typedef struct hal_iic_dev hal_iic_dev_t;
typedef struct hal_iic_ops hal_iic_ops_t;

struct hal_iic_dev {
    const hal_iic_ops_t *ops;

    uint8_t last_addr;
    uint8_t last_reg;
};

struct hal_iic_ops {
    hal_iic_result_t (*prewrite)(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length);
    void (*write)(hal_iic_dev_t *self, uint8_t addr, uint8_t reg, uint8_t value);
    hal_iic_result_t (*preread)(hal_iic_dev_t *self, uint8_t addr, uint8_t start, uint8_t length);
    uint8_t (*read)(hal_iic_dev_t *self, uint8_t addr, uint8_t reg);
    void (*cancel)(hal_iic_dev_t *self, uint8_t addr);
    void (*tick)(hal_iic_dev_t *self);
};

// I2C "controller"
// to be implemented by NXT3 core
extern bool Hal_IicHost_Attach(hal_iic_dev_t *device, int port);
extern bool Hal_IicHost_Detach(int port);
extern bool Hal_IicHost_Present(int port);

// I2C "device"
// implemented locally in HAL core (proxy for struct hal_i2c_ops)
extern bool Hal_IicDev_JustAttached(hal_iic_dev_t *self);
extern void Hal_IicDev_JustDetached(hal_iic_dev_t *self);
extern hal_iic_result_t Hal_IicDev_Start(hal_iic_dev_t *self, const uint8_t *srcBuf, uint32_t srcLen);
extern hal_iic_result_t Hal_IicDev_Poll(hal_iic_dev_t *self, uint8_t *dstBuf, uint32_t dstLen);
extern void Hal_IicDev_Cancel(hal_iic_dev_t *self);
extern void Hal_IicDev_Tick(hal_iic_dev_t *self);

#endif //HAL_IIC
