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
        src/hal_general.c
        src/kdev_ui.c
        include/hal_button.private.h
        include/hal_led.private.h
        include/hal_timer.private.h
        include/hal_display.private.h
        include/kdev_ui.h
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

add_library(NXT3::Hal::Lms2012 ALIAS nxthal_lms2012)
