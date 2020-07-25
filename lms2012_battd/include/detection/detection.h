#ifndef DETECTION
#define DETECTION

#include <stdbool.h>
#include "fifo.h"
#include "analog.h"

bool detection_open(bool rechargeable);
void detection_close(void);
void detection_update(battd_msg_t *state, adc_readings_t *adc);

#endif //DETECTION
