cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../../pico-sdk)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(test_data_boat C CXX ASM)

pico_sdk_init()

# Usa main.cpp invece di main.c
add_executable(test_data_boat main.cpp)

pico_set_program_name(test_data_boat "test_data_boat")
pico_set_program_version(test_data_boat "0.1")

pico_enable_stdio_uart(test_data_boat 0)
pico_enable_stdio_usb(test_data_boat 1)

# Include RF24
add_subdirectory(../library/RF24 RF24_build)

target_link_libraries(test_data_boat
    pico_stdlib
    hardware_spi
    hardware_irq
    RF24
)

target_include_directories(test_data_boat PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ../library/RF24
)

pico_add_extra_outputs(test_data_boat)
