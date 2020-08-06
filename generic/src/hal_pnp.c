#include "hal_pnp.h"

void Hal_Pnp_SetDigiDir(struct hal_pins *pins, pin_t num, bool out) {
    if (pins) {
        if (num & DIGI0)
            pins->d0_dir = out ? DIR_OUT : DIR_IN;
        if (num & DIGI1)
            pins->d1_dir = out ? DIR_OUT : DIR_IN;
    }
}

void Hal_Pnp_SetDigiOut(struct hal_pins *pins, pin_t num, bool high) {
    Hal_Pnp_SetDigiDir(pins, num, DIR_OUT);
    if (pins) {
        if (num & DIGI0)
            pins->d0_out = high ? PIN_HIGH : PIN_LOW;
        if (num & DIGI1)
            pins->d1_out = high ? PIN_HIGH : PIN_LOW;
    }
}

void Hal_Pnp_GetDigiIn(struct hal_pins *pins, pin_t num, uint8_t *pMask) {
    if (pins) {
        if (num & DIGI0) {
            if (pins->d0_in)
                *pMask |= 0x01;
            else
                *pMask &= ~0x01;
        }
        if (num & DIGI1) {
            if (pins->d1_in)
                *pMask |= 0x02;
            else
                *pMask &= ~0x02;
        }
    }

}

void Hal_Pnp_SetPower(struct hal_pins *pins, pwr_aux_t mode) {
    if (pins) {
        pins->pwr_mode = mode;
    }
}
