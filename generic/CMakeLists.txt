cmake_minimum_required(VERSION 3.10)

project(NXT3-HAL-Defs  DESCRIPTION "NXT firmware HAL"
                       VERSION 0.1
                       LANGUAGES C)

add_library(nxthal STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_button.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_led.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_types.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_timer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_sound.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_display.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_general.h
)

target_include_directories(nxthal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

add_library(NXT3::Hal ALIAS nxthal)