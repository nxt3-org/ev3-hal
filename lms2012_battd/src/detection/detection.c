#include <math.h>
#include <time.h>
#include "detection/temperature.h"
#include "detection/analog.h"
#include "detection/detection.h"
#include "fifo.h"

#define   LOW_VOLTAGE_SHUTDOWN_TIME     10             //!< Time from shutdown lower limit to shutdown [s]
#define   TEMP_SHUTDOWN_FAIL            30.0f          //!< Temperature rise before fail shutdown  [C]
#define   TEMP_SHUTDOWN_WARNING         25.0f          //!< Temperature rise before warning        [C]

static float BattIndicatorHigh;
static float BattIndicatorLow;
static float BattWarningHigh;
static float BattWarningLow;
static float BattShutdownHigh;
static float BattShutdownLow;
static bool  doCheckTemps;

static struct timespec lastNotCriticalBattery;

static float getPercent(adc_readings_t *adc);
static void checkVolts(battd_msg_t *state);
static void checkTemps(battd_msg_t *state);
static int elapsedSeconds(struct timespec since);

bool detection_open(bool rechargeable) {
    clock_gettime(CLOCK_MONOTONIC, &lastNotCriticalBattery);
    if (rechargeable) {
        doCheckTemps      = false; // Battery pack has its own thermistor + we do not have a model
        BattIndicatorHigh = 7.5f;  // Rechargeable battery indicator high [V]
        BattIndicatorLow  = 7.1f;  // Rechargeable battery indicator low [V]
        BattWarningHigh   = 7.1f;  // Rechargeable battery voltage warning upper limit [V]
        BattWarningLow    = 6.5f;  // Rechargeable battery voltage warning lower limit [V]
        BattShutdownHigh  = 6.5f;  // Rechargeable battery voltage shutdown upper limit [V]
        BattShutdownLow   = 6.0f;  // Rechargeable battery voltage shutdown lower limit [V]
    } else {
        doCheckTemps      = true;  // AA batteries do not have thermistors
        BattIndicatorHigh = 7.5f;  // Battery indicator high [V]
        BattIndicatorLow  = 6.2f;  // Battery indicator low [V]
        BattWarningHigh   = 6.2f;  // Battery voltage warning upper limit [V]
        BattWarningLow    = 5.5f;  // Battery voltage warning lower limit [V]
        BattShutdownHigh  = 5.5f;  // Battery voltage shutdown upper limit [V]
        BattShutdownLow   = 4.5f;  // Battery voltage shutdown lower limit [V]
    }
    return analog_open();
}

void detection_close(void) {
    analog_close();
}

void detection_update(battd_msg_t *state, adc_readings_t *adc) {

    state->Battery_Voltage = adc->BatteryVoltage;
    state->Battery_Current = adc->BatteryCurrent;
    state->Battery_Percent = getPercent(adc);
    checkVolts(state);

    if (doCheckTemps) {
        // predict battery temperature using a model
        state->Battery_Temperature = temperature_get(state->Battery_Voltage,
                                                     state->Battery_Current * 1.1f,
                                                     0.4f);
        checkTemps(state);
    } else {
        // no model for lego battery pack
        state->Battery_Temperature = NAN;
    }
}

float getPercent(adc_readings_t *adc) {
    float full = (adc->BatteryVoltage - BattIndicatorLow) /
                 (BattIndicatorHigh - BattIndicatorLow) * 100.0f;
    if (full > 100.0f)
        return 100.0f;
    if (full < 0.0f)
        return 0.0f;
    return full;
}

void checkVolts(battd_msg_t *state) {
    if (state->Battery_Voltage >= BattWarningHigh)
        state->Events &= ~LOW_BATTERY_WARN;
    if (state->Battery_Voltage <= BattWarningLow)
        state->Events |= LOW_BATTERY_WARN;

    if (state->Battery_Voltage >= BattShutdownHigh) {
        state->Events &= ~LOW_BATTERY_CRIT;
        state->Events &= ~LOW_BATTERY_SHDN;
    }

    if (state->Battery_Voltage < BattShutdownLow) {
        state->Events |= LOW_BATTERY_CRIT;

        if (elapsedSeconds(lastNotCriticalBattery) > LOW_VOLTAGE_SHUTDOWN_TIME) {
            state->Events |= LOW_BATTERY_SHDN;
        }
    } else {
        clock_gettime(CLOCK_MONOTONIC, &lastNotCriticalBattery);
    }
}

void checkTemps(battd_msg_t *state) {
    if (state->Battery_Temperature >= TEMP_SHUTDOWN_WARNING)
        state->Events |= OVERHEAT_WARN;
    else
        state->Events &= ~OVERHEAT_WARN;

    if (state->Battery_Temperature >= TEMP_SHUTDOWN_FAIL)
        state->Events |= OVERHEAT_SHDN;
}

int elapsedSeconds(struct timespec since) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    int delta = now.tv_sec - since.tv_sec;

    if (delta < 0)
        return 0;

    if (now.tv_nsec <= since.tv_nsec)
        delta -= 1;

    return delta;
}
