cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../../pico-sdk)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(boat C CXX ASM)

pico_sdk_init()

# Usa main.cpp invece di main.c
add_executable(boat main.cpp config.c generic_functions.c MPU_6500_functions.c)

pico_set_program_name(boat "boat")
pico_set_program_version(boat "0.1")

pico_enable_stdio_uart(boat 0)
pico_enable_stdio_usb(boat 1)

# Include RF24
add_subdirectory(../library/RF24 RF24_build)

target_link_libraries(boat
    pico_stdlib
    hardware_spi
    hardware_irq
    hardware_adc
    hardware_pwm
    RF24
)

target_include_directories(boat PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ../library/RF24
)

target_compile_definitions(${CMAKE_PROJECT_NAME}
    PUBLIC
        PICO_DEFAULT_SPI=1
        PICO_DEFAULT_SPI_SCK_PIN=10
        PICO_DEFAULT_SPI_TX_PIN=12
        PICO_DEFAULT_SPI_RX_PIN=11
)

pico_add_extra_outputs(boat)
