cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(test_timing_controller_boat C CXX ASM)

pico_sdk_init()

# Usa main.cpp invece di main.c
add_executable(test_timing_controller_boat main.cpp)

pico_set_program_name(test_timing_controller_boat "test_timing_controller_boat")
pico_set_program_version(test_timing_controller_boat "0.1")

pico_enable_stdio_uart(test_timing_controller_boat 0)
pico_enable_stdio_usb(test_timing_controller_boat 1)

# Include RF24
add_subdirectory(../library/RF24 RF24_build)

target_link_libraries(test_timing_controller_boat
    pico_stdlib
    hardware_spi
    RF24
)

target_include_directories(test_timing_controller_boat PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ../library/RF24
)

pico_add_extra_outputs(test_timing_controller_boat)
