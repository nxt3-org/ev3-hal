#ifndef SEN_EV3_TOUCH_PRIVATE
#define SEN_EV3_TOUCH_PRIVATE

#include "sen_ev3_touch.h"

typedef struct {
    hal_adc_dev_t link;
    sensor_dev_t  dev;
    int           port;
} sensor_ev3_touch_t;

extern bool attach(sensor_dev_t *dev);
extern void detach(sensor_dev_t *dev);
extern void destroy(sensor_dev_t *dev);

#endif //SEN_EV3_TOUCH_PRIVATE
