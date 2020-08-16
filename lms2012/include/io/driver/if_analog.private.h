#ifndef IF_ANALOG_PRIVATE
#define IF_ANALOG_PRIVATE

#include <io/core/interface.h>

typedef enum {
    ANALOG_OFF,
    ANALOG_WAITING,
    ANALOG_READY
} analog_state_t;

typedef struct {
    const typedb_entry_t *analog;
    analog_state_t       state;
    int32_t              timer;
    bool booting;
} analog_port_t;

typedef struct {
    int           refCount;
    analog_port_t ports[4];
} drv_analog_t;

extern drv_analog_t Drv_Analog;

#endif //IF_ANALOG_PRIVATE
