cmake_minimum_required(VERSION 3.18)

# Pull in SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(can2040_project0)

#I've set this to allow breakpoints on any source line
set(PICO_DEOPTIMIZED_DEBUG=1)

pico_sdk_init()


add_executable(can2040_project0
    src/main.c
    $ENV{CAN2040_LIB_PATH}/src/can2040.c
)

target_include_directories(can2040_project0 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/source
    $ENV{CAN2040_LIB_PATH}/src
)


target_link_libraries(can2040_project0 pico_stdlib cmsis_core)

pico_add_extra_outputs(can2040_project0)

pico_enable_stdio_usb(can2040_project0 1)
pico_enable_stdio_uart(can2040_project0 0)