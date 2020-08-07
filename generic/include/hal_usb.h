#ifndef HAL_USB
#define HAL_USB

#include "hal_errorcodes.h"

extern bool Hal_Usb_RefAdd(void);
extern bool Hal_Usb_RefDel(void);
extern void Hal_Usb_Tick(void);

extern void Hal_Usb_StoreBtAddress(const uint8_t *raw);
extern void Hal_Usb_ResetState(void);

extern bool Hal_Usb_IsPresent(void);
extern bool Hal_Usb_IsReady(void);
extern uint32_t Hal_Usb_RxFrame(uint8_t *buffer, uint32_t maxLength);
extern void Hal_Usb_TxFrame(const uint8_t *buffer, uint32_t maxLength);

// implemented locally
extern bool Hal_Usb_AddHandle(handle_t hnd);
extern bool Hal_Usb_RemoveHandle(handle_t hnd);
extern errhnd_t Hal_Usb_GetNextHandle(uint32_t *pPosition);


#endif //HAL_USB
