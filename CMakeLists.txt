cmake_minimum_required(VERSION 3.18)

# Pull in SDK (must be before project)
include($ENV{PICO_SDK_EXTERNAL_PATH}/pico_sdk_import.cmake)

project(canCPP)

#I've set this to allow breakpoints on any source line
set(PICO_DEOPTIMIZED_DEBUG=1)

pico_sdk_init()


add_executable(canCPP
    src/main.cpp
    src/modules/can/CAN.cpp
    $ENV{PICO_LIB_PATH}/can2040/src/can2040.c
)

target_include_directories(canCPP PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/source
    $ENV{PICO_LIB_PATH}/can2040/src/
    /src/modules/can
)


target_link_libraries(canCPP pico_stdlib cmsis_core)

pico_add_extra_outputs(canCPP)

pico_enable_stdio_usb(canCPP 1)
pico_enable_stdio_uart(canCPP 0)