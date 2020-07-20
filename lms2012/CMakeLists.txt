cmake_minimum_required(VERSION 3.10)

project(NXT3-HAL-LMS2012 DESCRIPTION "NXT firmware HAL implementation for EV3 stock firmware"
        VERSION 0.1
        LANGUAGES C)

add_library(nxthal_lms2012 STATIC
        src/hal_button.c
        src/hal_led.c
        src/hal_timer.c
        src/hal_display.c
        src/hal_display_render.c
        src/hal_sound.c
        src/hal_sound_melody.c
        src/hal_sound_tone.c
        src/hal_sound_pcm.c
        src/hal_motor.c
        src/hal_filesystem.c
        src/hal_general.c
        src/hal_iic.c
        src/hal_adc.c
        src/kdev_core.c
        src/kdevices.c
        src/posix_fs.c
        src/sen_iic_base.c
        src/sen_dummy_us.c
        src/sen_dummy_light.c
        include/hal_button.private.h
        include/hal_led.private.h
        include/hal_timer.private.h
        include/hal_display.private.h
        include/hal_sound.private.h
        include/hal_motor.private.h
        include/hal_filesystem.private.h
        include/hal_iic.private.h
        include/hal_adc.private.h
        include/kdev_core.h
        include/kdevices.h
        include/posix_fs.h
        include/posix_fs.private.h
        include/sen_iic_base.h
        include/sen_dummy_us.h
        include/sen_dummy_light.h
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
