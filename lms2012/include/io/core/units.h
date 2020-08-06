#ifndef UNITS
#define UNITS

#include <hal_ev3sensor.h>
#include "io/core/hwdb.h"

extern int16_t AdcVolts(uint16_t counts);
extern void Transform(const float *in, float *out, int count, const typedata_t *data, unit_type_t type, bool clip);
extern void TransformBlock(const float *rawIn, float *rawOut, float *pctOut, float *siOut, int realSlots, int fullSlots, const typedata_t *data, bool clip);


#endif //UNITS
