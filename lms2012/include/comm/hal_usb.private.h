#ifndef HAL_USB_PRIVATE
#define HAL_USB_PRIVATE

#include "hal_usb.h"
#include "ev3proto.h"

#define BUFFER_SIZE 1024
#define NXT_BUFFER_SIZE 64

typedef struct {
    int refCount;
    uint8_t *rxBuffer;
    uint8_t *txBuffer;
    int rxCount;
    int txCount;
    uint8_t *nxtRxBuffer;
    uint8_t *nxtTxBuffer;
    int nxtRxCount;
    int nxtTxCount;
    channel_t ev3;
    int modeFd;
    bool addrSet;
    bool present;
    bool ready;
} mod_usb_t;

extern mod_usb_t Mod_Usb;

extern void Hal_Usb_ReloadPresence(void);
extern bool Hal_Usb_DoRead(void);
extern bool Hal_Usb_DoWrite(void);
extern void Hal_Usb_HandleNxtRx(int bytes);
extern bool Hal_Usb_HandleNxtTx(void);
extern void Hal_Usb_RejectDirectCmd(int counter);
extern void Hal_Usb_RejectSysCommand(int bytes, int counter, int type);

#endif //HAL_USB_PRIVATE
