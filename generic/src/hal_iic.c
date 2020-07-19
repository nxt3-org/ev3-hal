#include "hal_iic.h"
#include <memory.h>

bool Hal_IicDev_JustAttached(hal_iic_dev_t *self, int port) {
    if (!self)
        return false;
    if (self->port != -1)
        return false;

    self->port      = port;
    self->last_addr = 0x00;
    self->last_reg  = 0x00;
    return true;
}

void Hal_IicDev_JustDetached(hal_iic_dev_t *self) {
    if (!self)
        return;
    self->port = -1;
}

hal_iic_result_t Hal_IicDev_Start(hal_iic_dev_t *self, const uint8_t *srcBuf, uint32_t srcLen) {
    if (!self)
        return HAL_IIC_RESULT_ERROR;
    if (srcLen < 2)
        return HAL_IIC_RESULT_ERROR;
    self->last_addr = srcBuf[0] >> 1;
    self->last_reg  = srcBuf[1];

    if (srcLen > 2) {
        hal_iic_result_t err = HAL_IIC_RESULT_DONE;
        if (self->ops->preread)
            err = self->ops->prewrite(self, self->last_addr, self->last_reg, srcLen - 2);
        if (err != HAL_IIC_RESULT_DONE)
            return err;

        if (self->ops->write) {
            for (int off = 0; off < srcLen - 2; off++) {
                self->ops->write(self, self->last_addr, self->last_reg + off, srcBuf[off + 2]);
            }
        }
    }
    return HAL_IIC_RESULT_DONE;
}

hal_iic_result_t Hal_IicDev_Poll(hal_iic_dev_t *self, uint8_t *dstBuf, uint32_t dstLen) {
    if (!self)
        return HAL_IIC_RESULT_ERROR;
    if (dstLen > 0) {
        hal_iic_result_t err = HAL_IIC_RESULT_DONE;
        if (self->ops->preread)
            err = self->ops->preread(self, self->last_addr, self->last_reg, dstLen);
        if (err != HAL_IIC_RESULT_DONE)
            return err;

        if (self->ops->read) {
            for (int off = 0; off < dstLen; off++) {
                dstBuf[off] = self->ops->read(self, self->last_addr, self->last_reg + off);
            }
        } else {
            memset(dstBuf, 0x00, dstLen);
        }
    }
    return HAL_IIC_RESULT_DONE;
}

void Hal_IicDev_Cancel(hal_iic_dev_t *self) {
    if (!self)
        return;
    if (self->ops->cancel)
        self->ops->cancel(self, self->last_reg);
}

void Hal_IicDev_Tick(hal_iic_dev_t *self) {
    if (!self)
        return;
    if (self->ops->tick)
        self->ops->tick(self);
}

