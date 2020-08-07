#ifndef HAL_USB_PRIVATE
#define HAL_USB_PRIVATE

#include "hal_usb.h"

#define BUFFER_SIZE 1024
#define NXT_BUFFER_SIZE 64

#define COMMAND_NXT3_HOST_TO_DEV      0x90
#define COMMAND_NXT3_DEV_TO_HOST      0x91
#define COMMAND_EV3_SYS_REQUEST       0x01
#define COMMAND_EV3_SYS_REQUEST_QUIET 0x81
#define COMMAND_EV3_SYS_REPLY_OK      0x03
#define COMMAND_EV3_SYS_REPLY_ERROR   0x05
#define COMMAND_EV3_VM_REQUEST        0x00
#define COMMAND_EV3_VM_REQUEST_QUIET  0x80
#define COMMAND_EV3_VM_REPLY_OK       0x02
#define COMMAND_EV3_VM_REPLY_ERROR    0x04

typedef struct {
    int refCount;
    uint8_t *rxBuffer;
    uint8_t *txBuffer;
    uint8_t *nxtRxBuffer;
    uint8_t *nxtTxBuffer;
    int modeFd;
    bool addrSet;
    bool present;
    bool ready;
    bool freshNxtRx;
    bool freshNxtTx;
} mod_usb_t;

extern mod_usb_t Mod_Usb;

extern void Hal_Usb_ReloadPresence(void);
extern bool Hal_Usb_DoRead(void);
extern bool Hal_Usb_DoWrite(void);
extern void Hal_Usb_HandleNxtRx(int bytes);
extern bool Hal_Usb_HandleNxtTx(void);
extern void Hal_Usb_RejectDirectCmd(int counter);
extern void Hal_Usb_HandleSysCommand(int bytes, int counter, int type);

#endif //HAL_USB_PRIVATE
