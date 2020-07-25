#include <detection/analog.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

typedef struct {
    uint8_t garbage[28];
    int16_t BatteryCurrentAdc;
    int16_t BatteryVoltageAdc;
} mmaped_t;

static int      fd;
static mmaped_t *memory;
static uint32_t AdcSumSamples;
static uint32_t AdcSumBatteryCurrent;
static uint32_t AdcSumBatteryVoltage;

static void analog_sample(void);

bool analog_open(void) {
    fd = open("/dev/lms_analog", O_RDWR);
    if (fd < 0) {
        perror("a/open");
        return false;
    }
    memory = mmap(NULL, sizeof(mmaped_t), PROT_READ, MAP_SHARED | MAP_LOCKED, fd, 0);
    if (memory == MAP_FAILED) {
        memory = NULL;
        perror("a/map");
        return false;
    }

    AdcSumSamples        = 0;
    AdcSumBatteryCurrent = 0;
    AdcSumBatteryVoltage = 0;
    return true;
}

void analog_close(void) {
    if (memory) {
        munmap(memory, sizeof(mmaped_t));
        memory = NULL;
    }
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

#define   ADC_REF              5.0f       //!< [V]  maximal value on ADC
#define   ADC_RES           4095.0f       //!< [CNT] maximal count on ADC
#define   CURRENT_SHUNT        0.05f      //  [Ohm]
#define   TRANSISTOR_DROP     0.05f       //  [V]
#define   AMPLIFIER_VBATT     0.5f        //  [Times]
#define   AMPLIFIER_IBATT     15.0f       //  [Times]
#define   ADC_VOLTS(C)    (((float)(C) * ADC_REF) / ADC_RES)

void analog_sample(void) {
    AdcSumSamples += 1;
    AdcSumBatteryCurrent += memory->BatteryCurrentAdc * memory->BatteryCurrentAdc;
    AdcSumBatteryVoltage += memory->BatteryVoltageAdc * memory->BatteryVoltageAdc;
}

void analog_read(adc_readings_t *data) {
    // RMS average
    float AvgVoltage = sqrtf((float) AdcSumBatteryVoltage / (float) AdcSumSamples);
    float AvgCurrent = sqrtf((float) AdcSumBatteryCurrent / (float) AdcSumSamples);

    // drop on current-measuring shunt (R248+R250)
    float drop_on_shunt      = ADC_VOLTS(AvgCurrent) / AMPLIFIER_IBATT;
    // drop on adc-enable transistor (Q19B)
    float drop_on_transistor = TRANSISTOR_DROP;
    // voltage measured by the ADC on a /2 voltage divider (R285+R287)
    float measured_voltage   = ADC_VOLTS(AvgVoltage) / AMPLIFIER_VBATT;

    // final voltage is a sum of all drops
    data->BatteryVoltage = measured_voltage + drop_on_shunt + drop_on_transistor;
    // current draw needs additional division by the shunt resistance
    data->BatteryCurrent = drop_on_shunt / CURRENT_SHUNT;
}

bool elapsed_400ms(struct timespec start) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (now.tv_sec == start.tv_sec) {
        return (now.tv_nsec - start.tv_nsec) >= 400000000;
    } else if (now.tv_sec == start.tv_sec + 1) {
        return (now.tv_nsec + 1000000000 - start.tv_nsec) >= 400000000;
    } else {
        return true;
    }
}

void analog_sample_for_400ms(adc_readings_t *data) {
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    AdcSumSamples        = 0;
    AdcSumBatteryCurrent = 0;
    AdcSumBatteryVoltage = 0;

    while (!elapsed_400ms(start)) {
        analog_sample();
        struct timespec msec = {.tv_sec=0, .tv_nsec=10 * 1000000};
        while (nanosleep(&msec, &msec));
    }

    return analog_read(data);
}
