cmake_minimum_required(VERSION 3.10)

project(NXT3-HAL-LMS2012-BattD DESCRIPTION "Battery monitoring daemon"
        VERSION 0.1
        LANGUAGES C)

add_executable(battd.elf
        src/battd.c
        src/fifo.c
        src/action/action.c
        src/action/power.c
        src/action/pwm.c
        src/action/display.c
        src/action/leds.c
        src/action/sound.c
        src/detection/analog.c
        src/detection/detection.c
        src/detection/temperature.c
        screens/overtemp.xbm
        screens/undervoltage.xbm
        include/fifo.h
        include/action/action.h
        include/action/power.h
        include/action/pwm.h
        include/action/display.h
        include/action/leds.h
        include/action/sound.h
        include/detection/analog.h
        include/detection/detection.h
        include/detection/temperature.h
        )

target_include_directories(battd.elf
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}/screens
        )

target_link_libraries(battd.elf PRIVATE -static)

set_target_properties(battd.elf PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS YES
        )