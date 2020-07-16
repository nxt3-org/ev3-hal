#ifndef HAL_GENERAL
#define HAL_GENERAL

#include <stdint.h>

extern void Hal_General_AbnormalExit(const char *message);
extern void Hal_General_SetupSignals(uint8_t *pStopFlag);

#endif //HAL_GENERAL
