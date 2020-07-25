#ifndef FIFO
#define FIFO

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

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
    battd_msg_t Message;
    uint32_t CounterTx;
    uint32_t CounterRx;
    pthread_mutex_t Mutex;
} battd_memory_t;

bool fifo_open();
void fifo_close(void);
void fifo_send(battd_msg_t *msg, bool *pResponsive);
bool fifo_should_continue(void);

#endif //FIFO
