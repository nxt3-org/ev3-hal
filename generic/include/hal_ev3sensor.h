#ifndef HAL_EV3SENSOR
#define HAL_EV3SENSOR

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    UNIT_RAW,
    UNIT_RAW_RANGE,
    UNIT_PERCENT,
    UNIT_SI,
} unit_type_t;

typedef struct hal_sensor2_ops hal_sensor2_ops_t;
typedef struct hal_sensor2_dev hal_sensor2_dev_t;

struct hal_sensor2_ops {
    const char *(*GetName)(hal_sensor2_dev_t *dev);

    int (*GetMode)(hal_sensor2_dev_t *dev);
    bool (*SetMode)(hal_sensor2_dev_t *dev, int mode);
    int (*GetModes)(hal_sensor2_dev_t *dev);
    const char *(*GetModeName)(hal_sensor2_dev_t *dev, int mode);
    const char *(*GetModeUnits)(hal_sensor2_dev_t *dev);
    int (*GetModeSlots)(hal_sensor2_dev_t *dev);

    bool (*Command)(hal_sensor2_dev_t *dev, const uint8_t *buffer, uint32_t length);
    int (*Measure)(hal_sensor2_dev_t *dev, float *si, float *pct, float *raw, int slots, bool clip);

    bool (*ResetDatalog)(hal_sensor2_dev_t *dev);
    int (*ReadDatalog)(hal_sensor2_dev_t *dev, int *pRepeat, float *si, float *pct, float *raw, int slots, bool clip);
};

struct hal_sensor2_dev {
    const hal_sensor2_ops_t *ops;
};

// EV3 sensor "controller"
// to be implemented by NXT3 core
extern bool Hal_Sensor2Host_Attach(hal_sensor2_dev_t *device, int port);
extern bool Hal_Sensor2Host_Detach(int port);

// EV3 sensor "device"
// implemented locally in HAL core (proxy for struct hal_adc_ops)

const char *Hal_Sensor2_GetName(hal_sensor2_dev_t *dev);
int Hal_Sensor2_GetMode(hal_sensor2_dev_t *dev);
bool Hal_Sensor2_SetMode(hal_sensor2_dev_t *dev, int mode);
int Hal_Sensor2_GetModes(hal_sensor2_dev_t *dev);
const char *Hal_Sensor2_GetModeName(hal_sensor2_dev_t *dev, int mode);
const char *Hal_Sensor2_GetModeUnits(hal_sensor2_dev_t *dev);
int Hal_Sensor2_GetModeSlots(hal_sensor2_dev_t *dev);
bool Hal_Sensor2_Command(hal_sensor2_dev_t *dev, const uint8_t *buffer, uint32_t length);
int Hal_Sensor2_Measure(hal_sensor2_dev_t *dev, float *si, float *pct, float *raw, int slots, bool clip);
bool Hal_Sensor2_ResetDatalog(hal_sensor2_dev_t *dev);
int Hal_Sensor2_ReadDatalog(hal_sensor2_dev_t *dev,
                            int *pRepeat, float *si, float *pct, float *raw,
                            int slots, bool clip);

#endif //HAL_EV3SENSOR
