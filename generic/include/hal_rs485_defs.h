#ifndef HAL_RS485_DEFS
#define HAL_RS485_DEFS

typedef enum {
    HS_BAUD_1200,
    HS_BAUD_2400,
    HS_BAUD_3600,
    HS_BAUD_4800,
    HS_BAUD_7200,
    HS_BAUD_9600,
    HS_BAUD_14400,
    HS_BAUD_19200,
    HS_BAUD_28800,
    HS_BAUD_38400,
    HS_BAUD_57600,
    HS_BAUD_76800,
    HS_BAUD_115200,
    HS_BAUD_230400,
    HS_BAUD_460800,
    HS_BAUD_921600
} hs_speed_t;

typedef enum {
    HS_MODE_UART_RS485 = 0x0,
    HS_MODE_UART_RS232 = 0x1
} hs_uart_mode_t;

typedef enum {
    // constants referring to HsMode (number of bits)
    HS_MODE_5_DATA = 0x0000,
    HS_MODE_6_DATA = 0x0040,
    HS_MODE_7_DATA = 0x0080,
    HS_MODE_8_DATA = 0x00C0,

    // constants referring to HsMode (number of stop bits)
    HS_MODE_10_STOP = 0x0000,
    HS_MODE_15_STOP = 0x1000,
    HS_MODE_20_STOP = 0x2000,

    // constants referring to HsMode (parity)
    HS_MODE_E_PARITY = 0x0000,
    HS_MODE_O_PARITY = 0x0200,
    HS_MODE_S_PARITY = 0x0400,
    HS_MODE_M_PARITY = 0x0600,
    HS_MODE_N_PARITY = 0x0800,

    // constants referring to HsMode (D|P|S)
    HS_MODE_8N1 = (HS_MODE_8_DATA | HS_MODE_N_PARITY | HS_MODE_10_STOP),
    HS_MODE_7E1 = (HS_MODE_7_DATA | HS_MODE_E_PARITY | HS_MODE_10_STOP),
} hs_params_t;

#define HS_MODE_MASK 0x3EC0
#define HS_UART_MASK 0x000F

typedef enum {
    HS_ADDRESS_ALL = 0,
    HS_ADDRESS_1   = 1,
    HS_ADDRESS_2   = 2,
    HS_ADDRESS_3   = 3,
    HS_ADDRESS_4   = 4,
    HS_ADDRESS_5   = 5,
    HS_ADDRESS_6   = 6,
    HS_ADDRESS_7   = 7,
    HS_ADDRESS_8   = 8,
} hs_addr_t;

#endif //HAL_RS485_DEFS
