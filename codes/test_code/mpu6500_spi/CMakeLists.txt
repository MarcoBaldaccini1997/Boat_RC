# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../../pico-sdk)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(MPU6500_SPI C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(MPU6500_SPI main.c config.c functions.c)

pico_set_program_name(MPU6500_SPI "MPU6500_SPI")
pico_set_program_version(MPU6500_SPI "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(MPU6500_SPI 0)
pico_enable_stdio_usb(MPU6500_SPI 1)

#TODO Add the standard library to the build
target_link_libraries(MPU6500_SPI
        pico_stdlib
        hardware_spi
    )

# Add the standard include files to the build
target_include_directories(MPU6500_SPI PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(MPU6500_SPI)

