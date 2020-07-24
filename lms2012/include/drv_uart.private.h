#ifndef DRV_UART_PRIVATE
#define DRV_UART_PRIVATE

#include "drv_uart.h"
#include "kdevices.h"
#include "drv_port.h"
#include "drv_dcm.private.h"
#include <sys/ioctl.h>

typedef enum __attribute__((packed)) {
    UART_OFF,
    UART_KERNEL_HANDSHAKE,
    UART_READING_INFO,
    UART_WAITING_FOR_READY,
    UART_READY,
    UART_WRITING,
    UART_MODESWITCHING,
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
#define MAX_HANDSHAKE_TIME 5000
typedef struct {
    typedata_t   types[MAX_MODES];
    uint16_t     timer;
    uart_state_t state;
} uart_port_t;

typedef struct {
    int                   refCount;
    identify_callback_t   *idCalls;
    modeswitch_callback_t *modeCalls;
    uart_port_t           ports[4];
    device_map_t          devmap;
} drv_uart_t;

extern drv_uart_t Drv_Uart;

extern bool Drv_Uart_RefAdd(void);
extern bool Drv_Uart_RefDel(void);
extern void Drv_Uart_Tick(void);

extern void Drv_Uart_PnpStart(dcm_port_id_t port, pnp_link_t link, pnp_device_t dev);
extern void Drv_Uart_PnpStop(dcm_port_id_t port);
extern void Drv_Uart_PnpSetCallbacks(identify_callback_t *id, modeswitch_callback_t *mode);

extern bool Drv_Uart_ReadType(uint8_t sPort, uint8_t mode);
extern bool Drv_Uart_KernelUpload(void);
extern bool Drv_Uart_ClearChanged(uint8_t sPort);
extern bool Drv_Uart_Write(uint8_t sPort, const uint8_t *buffer, uint8_t length);

#define   MAX_UART_MSGLEN       32
#define   KERNEL_UPLOAD         _IOWR('u',0,device_map_t)
#define   KERNEL_READ_INFO      _IOWR('u',1,uartinfo_t)
#define   KERNEL_ENABLE_INFO    _IOWR('u',2,uartinfo_t)
#define   KERNEL_CLEAR_CHANGED  _IOWR('u',3,uartinfo_t)

#endif //DRV_UART_PRIVATE
