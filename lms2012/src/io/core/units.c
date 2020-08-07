#include <io/core/units.h>
#include <hal_ev3sensor.h>
#include <math.h>

void Transform(const float *in, float *out, int count, const typedata_t *data, unit_type_t type, bool clip) {
    float inMin = data->RawMin;
    float inMax = data->RawMax;
    float outMin;
    float outMax;

    switch(type) {
    case UNIT_RAW:
        outMin = data->RawMin;
        outMax = data->RawMax;
        break;
    case UNIT_RAW_RANGE:
        outMin = 0.0f;
        outMax = 1.0f;
        break;
    case UNIT_PERCENT:
        outMin = data->PercentMin;
        outMax = data->PercentMax;
        break;
    case UNIT_SI:
        outMin = data->SiMin;
        outMax = data->SiMax;
        break;
    }

    float scaler = (outMax - outMin) / (inMax - inMin);

    for (int i = 0; i < count; i++) {
        float output;
        if (type != UNIT_RAW) {
            output = (in[i] - inMin) * scaler + outMin;
        } else {
            output = in[i];
        }
        if (clip) {
            if (output > outMax)
                output = outMax;
            if (output < outMin)
                output = outMin;
        }
        out[i] = output;
    }
}

#define   ADC_MAX_VOLTS_I   5000
#define   ADC_MAX_POINTS_I  4095

int16_t AdcVolts(uint16_t counts) {
    return (counts * ADC_MAX_VOLTS_I) / ADC_MAX_POINTS_I;
}

void TransformBlock(const float *rawIn,
                    float *rawOut, float *pctOut, float *siOut,
                    int realSlots, int fullSlots,
                    const typedata_t *info, bool clip) {
    if (rawOut) {
        Transform(rawIn, rawOut, realSlots, info, UNIT_RAW, clip);
        for (int i = realSlots; i < fullSlots; i++)
            rawOut[i] = NAN;
    }
    if (pctOut) {
        Transform(rawIn, pctOut, realSlots, info, UNIT_PERCENT, clip);
        for (int i = realSlots; i < fullSlots; i++)
            pctOut[i] = NAN;
    }
    if (siOut) {
        Transform(rawIn, siOut, realSlots, info, UNIT_SI, clip);
        for (int i = realSlots; i < fullSlots; i++)
            siOut[i] = NAN;
    }
}
