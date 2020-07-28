cmake_minimum_required(VERSION 3.10)

project(NXT3-HAL-LMS2012 DESCRIPTION "NXT firmware HAL implementation for EV3 stock firmware"
        VERSION 0.1
        LANGUAGES C)

add_library(nxthal_lms2012 STATIC
        include/comm/hal_bt.private.h
        include/comm/hal_rs485.private.h
        include/comm/hal_usb.private.h
        include/common/kdev_core.h
        include/common/kdevices.h
        include/fs/hal_filesystem.private.h
        include/fs/posix_fs.h
        include/fs/posix_fs.private.h
        include/io/core/device_db.h
        include/io/core/hal_pnp.private.h
        include/io/core/sen_base.h
        include/io/core/sen_iic_base.h
        include/io/driver/drv_analog.h
        include/io/driver/drv_analog.private.h
        include/io/driver/drv_dcm.h
        include/io/driver/drv_dcm.private.h
        include/io/driver/drv_dummy.h
        include/io/driver/drv_motor.h
        include/io/driver/drv_motor.private.h
        include/io/driver/drv_port.h
        include/io/driver/drv_uart.h
        include/io/driver/drv_uart.private.h
        include/io/hal/hal_motor.private.h
        include/io/hal/sen_ev3_color.h
        include/io/hal/sen_ev3_color.private.h
        include/io/hal/sen_ev3_touch.h
        include/io/hal/sen_ev3_touch.private.h
        include/io/hal/sen_ev3_us.h
        include/io/hal/sen_ev3_us.private.h
        include/io/hal/sen_tty.h
        include/io/hal/sen_tty.private.h
        include/misc/hal_battery.private.h
        include/misc/hal_timer.private.h
        include/ui/hal_button.private.h
        include/ui/hal_display.private.h
        include/ui/hal_led.private.h
        include/ui/hal_sound.private.h
        src/comm/hal_bt.c
        src/comm/hal_rs485.c
        src/comm/hal_usb.c
        src/common/kdev_core.c
        src/common/kdevices.c
        src/fs/hal_filesystem.c
        src/fs/posix_fs.c
        src/io/core/device_db.c
        src/io/core/hal_pnp.c
        src/io/core/sen_base.c
        src/io/core/sen_iic_base.c
        src/io/driver/drv_analog.c
        src/io/driver/drv_dcm.c
        src/io/driver/drv_dummy.c
        src/io/driver/drv_motor.c
        src/io/driver/drv_uart.c
        src/io/hal/hal_motor.c
        src/io/hal/sen_ev3_color.c
        src/io/hal/sen_ev3_touch.c
        src/io/hal/sen_ev3_us.c
        src/io/hal/sen_tty.c
        src/misc/hal_battery.c
        src/misc/hal_general.c
        src/misc/hal_timer.c
        src/ui/hal_button.c
        src/ui/hal_display.c
        src/ui/hal_display_render.c
        src/ui/hal_led.c
        src/ui/hal_sound.c
        src/ui/hal_sound_melody.c
        src/ui/hal_sound_pcm.c
        src/ui/hal_sound_tone.c
        )

target_include_directories(nxthal_lms2012
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        )

target_link_libraries(nxthal_lms2012 PUBLIC nxthal)

set_target_properties(nxthal_lms2012 PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS YES
        )
