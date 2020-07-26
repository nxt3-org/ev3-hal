#ifndef HAL_BATTERY
#define HAL_BATTERY

#include <stdint.h>
#include <stdbool.h>

extern bool Hal_Battery_RefAdd(void);
extern bool Hal_Battery_RefDel(void);
extern void Hal_Battery_Tick(void);

extern bool Hal_Battery_IsRechargeable(bool *pReally);
extern bool Hal_Battery_GetVoltage(float *pV);
extern bool Hal_Battery_GetCurrent(float *pA);
extern bool Hal_Battery_GetPercentRemaining(float *pPercent);
extern bool Hal_Battery_GetTemperature(float *pC);
extern bool Hal_Battery_CheckBatteryWarning(bool *pWarning, bool *pCritical);
extern bool Hal_Battery_CheckTempWarning(bool *pReally);

#endif //HAL_BATTERY
