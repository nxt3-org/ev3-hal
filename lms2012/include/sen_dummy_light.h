#ifndef SEN_DUMMY_LIGHT
#define SEN_DUMMY_LIGHT

#include "hal_adc.h"

typedef struct {
    hal_adc_dev_t link;
    int reflectivity;
    bool valueUp;
} dummy_light_t;

extern dummy_light_t *createLight(void);
extern void deleteLight(dummy_light_t *ptr);

#endif //SEN_DUMMY_LIGHT
