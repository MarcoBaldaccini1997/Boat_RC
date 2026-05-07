cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../../pico-sdk)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(test_control_brushless C CXX ASM)

pico_sdk_init()

# Usa main.cpp invece di main.c
add_executable(test_control_brushless main.c)

pico_set_program_name(test_control_brushless "test_control_brushless")
pico_set_program_version(test_control_brushless "0.1")

pico_enable_stdio_uart(test_control_brushless 0)
pico_enable_stdio_usb(test_control_brushless 1)

target_link_libraries(test_control_brushless
    pico_stdlib
    hardware_pwm
)


pico_add_extra_outputs(test_control_brushless)
