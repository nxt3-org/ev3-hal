#ifndef SEN_EV3_TOUCH_PRIVATE
#define SEN_EV3_TOUCH_PRIVATE

#include "hal_adc.h"
#include "hal_iic.h"
#include "sen_ev3_touch.h"

typedef struct {
    hal_adc_dev_t adc;
    hal_iic_dev_t iic;
    sensor_dev_t  dev;
    int           port;
} sensor_ev3_touch_t;

extern bool attach(sensor_dev_t *dev);
extern void detach(sensor_dev_t *dev);
extern void destroy(sensor_dev_t *dev);

#endif //SEN_EV3_TOUCH_PRIVATE
