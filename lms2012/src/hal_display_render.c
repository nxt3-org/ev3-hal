#include "hal_display.h"
#include "hal_display.private.h"
#include "kdevices.h"

#define CENTER2_EV3_X0 39
#define CENTER2_EV3_Y0 32

void writeDirect(const uint8_t *buffer, bool centered) {
    uint8_t *basePtr;
    if (centered)
        basePtr = &DeviceDisplay.mmap[CENTER2_EV3_Y0 * EV3_DISPLAY_STRIDE + CENTER2_EV3_X0 / 3];
    else
        basePtr = &DeviceDisplay.mmap[0];

    const uint8_t *nxtPtr   = buffer;
    uint8_t       *blockPtr = basePtr;

    for (int block = 0; block < 8; block++) {
        uint8_t *columnPtr = blockPtr;

        for (int dx = 0; dx < HAL_DISPLAY_WIDTH / 3; dx++) {

            uint8_t nxtMask   = 0b00000001;
            uint8_t *pixelPtr = columnPtr;

            uint8_t pix1 = nxtPtr[0];
            uint8_t pix2 = nxtPtr[1];
            uint8_t pix3 = nxtPtr[2];

            while (nxtMask) {
                uint8_t part1 = (pix1 & nxtMask ? 0b11000000 : 0b00000000);
                uint8_t part2 = (pix2 & nxtMask ? 0b00011000 : 0b00000000);
                uint8_t part3 = (pix3 & nxtMask ? 0b00000011 : 0b00000000);

                *pixelPtr = part1 | part2 | part3;

                pixelPtr += EV3_DISPLAY_STRIDE;
                nxtMask <<= 1;
            }

            columnPtr += 1;
            nxtPtr += 3;
        }
        nxtPtr += 1;
        blockPtr += EV3_DISPLAY_STRIDE * 8;
    }

    nxtPtr = &buffer[HAL_DISPLAY_WIDTH - 1];
    uint8_t *pixelPtr = basePtr;

    for (int block = 0; block < 8; block++) {
        uint8_t pix = nxtPtr[0];

        for (int i = 0; i < 8; i++) {
            if (pix & 0x01) {
                *pixelPtr = 0b11000000;
            } else {
                *pixelPtr = 0b00000000;
            }
            pixelPtr += EV3_DISPLAY_STRIDE;
            pix >>= 1;
        }
        nxtPtr += HAL_DISPLAY_WIDTH;
    }
}

#define NXT_CROP_X0 6
#define NXT_CROP_WIDTH 87

void writeCrop(const uint8_t *buffer) {
    uint8_t *basePtr = &DeviceDisplay.mmap[0];

    const uint8_t *nxtPtr   = &buffer[NXT_CROP_X0];
    uint8_t       *blockPtr = basePtr;

    for (int block = 0; block < 8; block++) {
        uint8_t *columnPtr = blockPtr;

        for (int dx = 0; dx < NXT_CROP_WIDTH / 3; dx++) {

            uint8_t nxtMask   = 0b00000001;
            uint8_t *pixelPtr = columnPtr;

            uint8_t pix1 = nxtPtr[0];
            uint8_t pix2 = nxtPtr[1];
            uint8_t pix3 = nxtPtr[2];

            while (nxtMask) {
                uint8_t part1, part2, byte1, byte2;

                part1 = (pix1 & nxtMask ? 0b11011000 : 0b00000000);
                part2 = (pix2 & nxtMask ? 0b00000011 : 0b00000000);
                byte1 = part1 | part2;
                part1 = (pix2 & nxtMask ? 0b11000000 : 0b00000000);
                part2 = (pix3 & nxtMask ? 0b00011011 : 0b00000000);
                byte2 = part1 | part2;

                pixelPtr[0] = byte1;
                pixelPtr[1] = byte2;
                pixelPtr += EV3_DISPLAY_STRIDE;
                pixelPtr[0] = byte1;
                pixelPtr[1] = byte2;
                pixelPtr += EV3_DISPLAY_STRIDE;

                nxtMask <<= 1;
            }

            columnPtr += 2;
            nxtPtr += 3;
        }
        nxtPtr += 100 - NXT_CROP_WIDTH;
        blockPtr += 2 * EV3_DISPLAY_STRIDE * 8;
    }
}

void writeStretch(const uint8_t *buffer) {
    uint8_t *basePtr = &DeviceDisplay.mmap[(EV3_DISPLAY_WIDTH - 150) / 6];

    const uint8_t *nxtPtr   = &buffer[0];
    uint8_t       *blockPtr = basePtr;

    for (int block = 0; block < 8; block++) {
        uint8_t *columnPtr = blockPtr;

        for (int dx = 0; dx < HAL_DISPLAY_WIDTH / 2; dx++) {

            uint8_t nxtMask   = 0b00000001;
            uint8_t *pixelPtr = columnPtr;

            uint8_t pix1 = nxtPtr[0];
            uint8_t pix2 = nxtPtr[1];

            while (nxtMask) {
                uint8_t part1, part2, byte;

                part1 = (pix1 & nxtMask ? 0b11011000 : 0b00000000);
                part2 = (pix2 & nxtMask ? 0b00000011 : 0b00000000);
                byte  = part1 | part2;

                *pixelPtr = byte;
                pixelPtr += EV3_DISPLAY_STRIDE;
                *pixelPtr = byte;
                pixelPtr += EV3_DISPLAY_STRIDE;

                nxtMask <<= 1;
            }

            columnPtr += 1;
            nxtPtr += 2;
        }
        blockPtr += 2 * EV3_DISPLAY_STRIDE * 8;
    }
}
