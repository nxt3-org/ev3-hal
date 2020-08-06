#ifndef IF_UART_PRIVATE
#define IF_UART_PRIVATE

#include <sys/ioctl.h>
#include "common/kdevices.h"
#include "io/core/interface.h"
#include "dcm.private.h"

typedef enum __attribute__((packed)) {
    UART_OFF,
    UART_WAITING_FOR_BOOT,
    UART_READY,
    UART_WAITING_FOR_WRITE,
    UART_WAITING_FOR_SWITCH,
    UART_TTY
} uart_state_t;

typedef struct {
    typedata_t typedata;
    uint8_t    port;
    uint8_t    mode;
} uartinfo_t;

typedef struct {
    uint8_t link[4];
    uint8_t type[4];
    uint8_t mode[4];
} device_map_t;

#define MAX_MODES 8
#define MAX_HANDSHAKE_TIME 3000
#define MAX_SWITCH_TIME 1500
typedef struct {
    int            refCount;
    typedb_entry_t types[4][MAX_MODES];
    uint16_t       timer[4];
    uart_state_t   state[4];
    device_map_t   devmap;
    uint16_t       datalogAge[4];
    uint16_t       datalogIndex[4];
    bool booting[4];
} drv_uart_t;

extern drv_uart_t Drv_Uart;

extern bool Drv_Uart_KernelReadType(uint8_t sPort, uint8_t mode);
extern bool Drv_Uart_KernelUpload(int invalidateMask);
extern bool Drv_Uart_KernelClearChanged(uint8_t sPort);
extern bool Drv_Uart_KernelWrite(uint8_t sPort, const uint8_t *buffer, uint8_t length);

#define   MAX_UART_MSGLEN       32
#define   KERNEL_UPLOAD         _IOWR('u',0,device_map_t)
#define   KERNEL_READ_INFO      _IOWR('u',1,uartinfo_t)
#define   KERNEL_ENABLE_INFO    _IOWR('u',2,uartinfo_t)
#define   KERNEL_CLEAR_CHANGED  _IOWR('u',3,uartinfo_t)

#endif //IF_UART_PRIVATE
