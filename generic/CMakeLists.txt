cmake_minimum_required(VERSION 3.10)

project(NXT3-HAL-Defs  DESCRIPTION "NXT firmware HAL"
                       VERSION 0.1
                       LANGUAGES C)

add_library(nxthal STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_button.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_led.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_timer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_sound.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_motor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_display.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_errorcodes.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_filesystem.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_general.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_iic.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_adc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_adc_defs.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_rs485.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_rs485_defs.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_usb.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_bt.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_pnp.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_pnp_defs.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_battery.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_ev3sensor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_filesystem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_iic.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_adc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_usb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_motor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_ev3sensor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_pnp.c
        )

set_target_properties(nxthal PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
        )

target_include_directories(nxthal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
