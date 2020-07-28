#include "hal_rs485.h"
#include "hal_rs485_defs.h"
#include "comm/hal_rs485.private.h"
#include <stdbool.h>
#include <stdint.h>

mod_rs485_t Mod_Rs485;

bool Hal_Rs485_RefAdd(void) {
    if (Mod_Rs485.refCount > 1) {
        Mod_Rs485.refCount++;
        return true;
    }
    // noop
    Mod_Rs485.refCount++;
    return true;
}

bool Hal_Rs485_RefDel(void) {
    if (Mod_Rs485.refCount == 0)
        return false;
    if (Mod_Rs485.refCount == 1) {
        // noop
    }
    Mod_Rs485.refCount--;
    return true;
}

void Hal_Rs485_Setup(hs_speed_t speed, hs_params_t params, hs_uart_mode_t umode) {
    return;
}

void Hal_Rs485_EnableRx(void) {
    return;
}

void Hal_Rs485_Transmit(const uint8_t *buffer, uint32_t *pLength) {
    *pLength = 0;
}

void Hal_Rs485_Receive(uint8_t *buffer, uint32_t *pLength) {
    *pLength = 0;
}

void Hal_Rs485_TxStatus(uint32_t *pQueueLength) {
    *pQueueLength = 0;
}

void Hal_Rs485_EnablePins(void) {
    return;
}

void Hal_Rs485_Disable(void) {
    return;
}
