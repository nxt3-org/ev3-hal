#ifndef SEN_BASE
#define SEN_BASE

#include "stdbool.h"

typedef struct sensor_ops sensor_ops_t;
typedef struct sensor_dev sensor_dev_t;

struct sensor_ops {
    bool (*attach)(sensor_dev_t *dev);
    void (*detach)(sensor_dev_t *dev);
    void (*destroy)(sensor_dev_t *dev);
};

struct sensor_dev {
    sensor_ops_t *ops;
};

extern bool Sensor_Attach(sensor_dev_t *dev);
extern bool Sensor_Detach(sensor_dev_t *dev);
extern bool Sensor_Destroy(sensor_dev_t *dev);

#endif //SEN_BASE
