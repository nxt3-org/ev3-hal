#include <stdio.h>
#include <detection/detection.h>
#include <action/action.h>
#include <action/power.h>
#include <autooff.h>

int main(int argc, char **argv) {
    int  retval = 0;

    if (argc != 1) {
        fputs("BattD: no args expected\n", stderr);
        return 1;
    }

    if (!fifo_open()) {
        retval = 2;
        goto cleanup;
    }
    if (!autooff_open()) {
        retval = 2;
        goto cleanup;
    }
    if (!action_open()) {
        retval = 2;
        goto cleanup;
    }
    if (!detection_open(power_is_rechargeable())) {
        retval = 2;
        goto cleanup;
    }

    bool responsive;

    adc_readings_t adc;
    battd_msg_t    state;

    state.BattD_Version = BATTD_VERSION;
    state.Events        = power_is_rechargeable() ? IS_RECHARGEABLE : 0;
    uint16_t lastWarns = state.Events;

    while (fifo_should_continue() && !autooff_should_exit()) {
        analog_sample_for_400ms(&adc);
        detection_update(&state, &adc);

        uint16_t warns = state.Events;
        uint16_t new   = warns & ~lastWarns;
        lastWarns = warns;

        if (new != 0) {
            // if peer (nxt3) is responsive, expect it to report the undervoltage warning properly
            if (!(new == LOW_BATTERY_WARN && responsive)) {
                bool cont = action_handle(new);
                if (!cont)
                    break;
            }
        }

        fifo_send(&state, &responsive);
    }

cleanup:
    detection_close();
    action_close();
    autooff_close();
    fifo_close();
    return retval;
}
