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
        include/io/core/hwdb.h
        include/io/core/hwdb.private.h
        include/io/core/hal_pnp.private.h
        include/io/core/adapter.h
        include/io/core/hal_pnp.local.h
        include/io/core/interface.h
        include/io/core/units.h
        include/io/driver/if_analog.private.h
        include/io/driver/dcm.h
        include/io/driver/dcm.private.h
        include/io/driver/if_motor.private.h
        include/io/driver/if_uart.private.h
        include/io/adapters/iic_base.h
        include/io/adapters/motor.private.h
        include/io/adapters/ev3_color.private.h
        include/io/adapters/touch.private.h
        include/io/adapters/ev3_us.private.h
        include/io/adapters/tty.private.h
        include/io/adapters/nxt_light.private.h
        include/io/adapters/ev3_gyro.private.h
        include/io/adapters/ev3_native.private.h
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
        src/io/core/hwdb.c
        src/io/core/hwdb.parser.c
        src/io/core/hal_pnp.c
        src/io/core/adapter.c
        src/io/core/units.c
        src/io/adapters/iic_base.c
        src/io/adapters/motor.c
        src/io/adapters/ev3_color.c
        src/io/adapters/touch.c
        src/io/adapters/ev3_us.c
        src/io/adapters/tty.c
        src/io/adapters/nxt_light.c
        src/io/adapters/ev3_gyro.c
        src/io/adapters/ev3_native.c
        src/io/driver/if_analog.c
        src/io/driver/dcm.c
        src/io/driver/if_motor.c
        src/io/driver/if_motor.kernel.c
        src/io/driver/if_uart.c
        src/io/driver/if_uart.kernel.c
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
