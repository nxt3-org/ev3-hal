#ifndef ANALOG
#define ANALOG

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float BatteryCurrent;
    float BatteryVoltage;
} adc_readings_t;

bool analog_open(void);
void analog_close(void);
bool analog_sample_for_400ms(adc_readings_t *data);
void analog_sample_single(adc_readings_t *data);

#endif //ANALOG
