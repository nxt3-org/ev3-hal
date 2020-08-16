#include <action/display.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fifo.h>
#include <string.h>

#include "overtemp.xbm"
#include "undervoltage.xbm"

#define LCD_WIDTH 178
#define LCD_HEIGHT 128
#define LCD_STRIDE 60
#define LCD_BYTES (LCD_STRIDE * LCD_HEIGHT)

static int     displayFD;
static uint8_t *memory;

bool display_open(void) {
    displayFD = open("/dev/fb0", O_RDWR);
    if (displayFD < 0) {
        perror("d/open");
        return false;
    }

    memory = mmap(NULL, LCD_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, displayFD, 0);
    if (memory == MAP_FAILED) {
        memory = NULL;
        perror("d/map");
        return false;
    }
    return true;
}

void display_close(void) {
    if (displayFD >= 0) {
        close(displayFD);
        displayFD = -1;
    }
    if (memory) {
        munmap(memory, LCD_BYTES);
        memory = NULL;
    }
}

void display_clear(void) {
    memset(memory, 0, LCD_BYTES);
}

void display_drawbuf(uint8_t *source, int width, int height) {
    int startX = (LCD_WIDTH - width) / 2;
    int startY = (LCD_HEIGHT - height) / 2;

    display_clear();

    int xbm_stride = (width + 7) / 8;

    for (int y = 0; y < height; y++) {
        uint8_t *pRow = &memory[(y + startY) * LCD_STRIDE];

        for (int x = 0; x < width; x++) {
            uint8_t srcByte = source[y * xbm_stride + x / 8];
            uint8_t srcBit  = srcByte & (1 << (x % 8));

            if (srcBit) {
                int dx = x + startX;
                pRow[dx / 3] |= 0b00000011 << (3 * (2 - dx % 3));
            }
        }
    }
}

void display_draw(uint16_t warning) {
    // display full image
    if (warning & OVERHEAT_SHDN)
        return display_drawbuf(overtemp_bits, overtemp_width, overtemp_height);
    if (warning & LOW_BATTERY_SHDN)
        return display_drawbuf(undervoltage_bits, undervoltage_width, undervoltage_height);

    // display only partial image - last 16 rows is "Shutting down..."
    if (warning & OVERHEAT_WARN)
        return display_drawbuf(overtemp_bits, overtemp_width, overtemp_height - 16);
    if (warning & (LOW_BATTERY_WARN | LOW_BATTERY_CRIT))
        return display_drawbuf(undervoltage_bits, undervoltage_width, undervoltage_height - 16);
    return;
}
