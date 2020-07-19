#ifndef HAL_GENERAL
#define HAL_GENERAL

#include <stdint.h>
#include <stddef.h>

extern void Hal_General_AbnormalExit(const char *message);
extern void Hal_General_SetupSignals(uint8_t *pStopFlag);
extern const char *Hal_General_GetPlatformName(void);
extern const char *Hal_General_GetPlatformHwVersion(void);
extern const char *Hal_General_GetPlatformFwVersion(void);

#define container_of(ptr, type, member) ({ \
                const typeof( ((type *)0)->member ) *__mptr = (ptr); \
                (type *)( (char *)__mptr - offsetof(type,member) );})

#endif //HAL_GENERAL
