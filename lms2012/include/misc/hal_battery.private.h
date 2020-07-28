#ifndef HAL_BATTERY_PRIVATE
#define HAL_BATTERY_PRIVATE

#include "hal_battery.h"

#define OVERHEAT_WARN      0x0001
#define OVERHEAT_SHDN      0x0002
#define LOW_BATTERY_WARN   0x0004
#define LOW_BATTERY_CRIT   0x0008
#define LOW_BATTERY_SHDN   0x0010
#define IS_RECHARGEABLE    0x1000
#define BATTD_VERSION      0x0002

typedef struct {
    uint16_t BattD_Version;
    uint16_t Events;
    float    Battery_Voltage;
    float    Battery_Percent;
    float    Battery_Current;
    float    Battery_Temperature;
} battd_msg_t;

typedef struct {
    pthread_mutex_t Mutex;
    uint32_t        CounterTx;
    uint32_t        CounterRx;
    battd_msg_t     Message;
} battd_memory_t;

typedef struct {
    int            refCount;
    int            memFd;
    battd_memory_t *mem;
    battd_msg_t    state;
    uint32_t       rxCounter;
    struct timespec lastRx;
} mod_battery_t;

extern mod_battery_t Mod_Battery;
static bool spawn_battd(void);
static bool isOnline(void);

#endif //HAL_BATTERY_PRIVATE
