#include <action/power.h>
#include <action/pwm.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <fifo.h>
#include <action/sound.h>
#include <action/display.h>
#include <action/leds.h>
#include "action/action.h"

static void action_kill_brick(void);
static void action_suspend_program(void);
static void action_resume_program(void);
static void action_wait(int msec);

bool action_open(void) {
    return power_open() && pwm_open() && sound_open() && display_open() && leds_open();
}

void action_close(void) {
    power_close();
    pwm_close();
    sound_close();
    display_close();
    leds_close();
}

bool action_handle(uint16_t new) {
    if (new & (LOW_BATTERY_SHDN | OVERHEAT_SHDN)) {
        action_suspend_program();
        pwm_stop();
        display_draw(new);
        leds_alert(new);
        sound_alert(new, 1000);
        action_wait(1000);
        action_kill_brick();
        return false;

    } else if (new & LOW_BATTERY_CRIT) {
        action_suspend_program();
        pwm_stop();
        display_draw(new);
        leds_alert(new);
        sound_alert(new, 1000);
        action_wait(1000);
        action_resume_program();
        return true;

    } else if (new & (LOW_BATTERY_WARN | OVERHEAT_WARN)) {
        action_suspend_program();
        display_draw(new);
        leds_alert(new);
        sound_alert(new, 500);
        action_wait(500);
        action_resume_program();
        return true;
    }
    return true;
}

void action_kill_brick(void) {
    power_shutdown();
}

void action_suspend_program(void) {
    system("killall -STOP nxt3.elf >/dev/null 2>&1");
    system("killall -STOP lms2012  >/dev/null 2>&1");
}

void action_resume_program(void) {
    system("killall -CONT nxt3.elf >/dev/null 2>&1");
    system("killall -CONT lms2012  >/dev/null 2>&1");
}

void action_wait(int msec) {
    struct timespec ts = {
        .tv_sec = msec / 1000,
        .tv_nsec = (msec % 1000) * 1000000,
    };
    while (nanosleep(&ts, &ts));
}
