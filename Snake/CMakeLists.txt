cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(JUST-SNAKE C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    src/main.c
    src/snake.c
    ../src/74HC595.c
    ../src/board.c
    ../src/utilities.c
)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_gpio
    hardware_flash
    hardware_timer
)
