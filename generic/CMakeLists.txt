cmake_minimum_required(VERSION 3.10)

project(NXT3-HAL-Defs  DESCRIPTION "NXT firmware HAL"
                       VERSION 0.1
                       LANGUAGES C)

add_library(nxthal INTERFACE)

target_include_directories(nxthal
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_sources(nxthal INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_button.h
)

add_library(NXT3::Hal ALIAS nxthal)
