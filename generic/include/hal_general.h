#ifndef HAL_GENERAL
#define HAL_GENERAL

#include <stdint.h>

extern void Hal_General_AbnormalExit(const char *message);
extern void Hal_General_SetupSignals(uint8_t *pStopFlag);
extern const char *Hal_General_GetPlatformName(void);
extern const char *Hal_General_GetPlatformHwVersion(void);
extern const char *Hal_General_GetPlatformFwVersion(void);

#endif //HAL_GENERAL
