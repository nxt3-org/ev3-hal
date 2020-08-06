#ifndef TTY_PRIVATE
#define TTY_PRIVATE

#include "iic_base.h"
#include "hal_iic.h"
#include "hal_general.h"
#include <io/core/adapter.h>

#define COMMAND_ADVANCE 0
#define COMMAND_WRITE   1
typedef struct {
    uint8_t readBuffer[32];
    uint8_t writeBuffer[32];
    uint8_t readPointer;
    uint8_t lengthArgument;
    uint8_t command;
} tty_data_t;

typedef struct {
    hal_iic_dev_t  link;
    adapter_t      dev;
    lego_iic_mem_t layout;
    tty_data_t     data;
} sensor_tty_t;

#define TTY_ADDRESS 0x01
#define TTY_REG_COMMAND (0x40 + offsetof(tty_data_t, command))

#endif //TTY_PRIVATE
