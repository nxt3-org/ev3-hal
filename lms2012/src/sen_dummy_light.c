#include "sen_dummy_light.h"
#include "hal_general.h"
#include <malloc.h>

#define to_light(dev) container_of(dev, dummy_light_t, link)

#define   ACTUAL_AD_RES                 1023L
#define   SENSOR_RESOLUTION             1023L
#define   VCC_SENSOR                    5000L
#define   AD_MAX                        1023L
#define   NEWLIGHTSENSORMIN             (800L/(VCC_SENSOR/AD_MAX))
#define   NEWLIGHTSENSORMAX             ((AD_MAX * 4400L)/VCC_SENSOR)
#define   NEWLIGHTSENSORPCTDYN          (((NEWLIGHTSENSORMAX - NEWLIGHTSENSORMIN) * 100L)/AD_MAX)


static bool light_is_valid(hal_adc_dev_t *dev) {
    return true;
}

static uint16_t light_read_adc(hal_adc_dev_t *dev) {
    dummy_light_t *light = to_light(dev);

    int value = light->reflectivity;
    if (dev->pins.d0_dir == DIR_OUT && dev->pins.d0_out == PIN_HIGH) {
        value >>= 0;
    } else {
        value >>= 1;
    }

    return NEWLIGHTSENSORMAX - (value * (NEWLIGHTSENSORMAX - NEWLIGHTSENSORMIN)) / 2000;
}

static void light_tick(hal_adc_dev_t *dev) {
    dummy_light_t *light = to_light(dev);
    if (light->valueUp) {
        light->reflectivity++;
        if (light->reflectivity == 2000)
            light->valueUp = false;
    } else {
        light->reflectivity--;
        if (light->reflectivity == 0)
            light->valueUp = true;
    }
}

static hal_adc_ops_t dummy_light_ops = {
    .is_valid = light_is_valid,
    .read_adc = light_read_adc,
    .color_active = NULL,
    .read_color_adc1 = NULL,
    .read_color_adc4= NULL,
    .read_color_caldata= NULL,
    .set_type = NULL,
    .load_pins= NULL,
    .store_pins= NULL,
    .tick = light_tick,
};

dummy_light_t *createLight(void) {
    dummy_light_t *dev = malloc(sizeof(dummy_light_t));
    if (dev) {
        dev->link.ops     = &dummy_light_ops;
        dev->link.port    = -1;
        dev->reflectivity = 0;
        dev->valueUp      = true;
    }
    return dev;
}

void deleteLight(dummy_light_t *ptr) {
    free(ptr);
}
