cmake_minimum_required(VERSION 3.10)

project(NXT3-HAL-LMS2012  DESCRIPTION "NXT firmware HAL implementation for EV3 stock firmware"
                          VERSION 0.1
                          LANGUAGES C)

add_library(nxthal_lms2012 STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_button.c
)

target_include_directories(nxthal_lms2012
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

set_target_properties(nxthal_lms2012 PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
)

add_library(NXT3::Hal::Lms2012 ALIAS nxthal_lms2012)
