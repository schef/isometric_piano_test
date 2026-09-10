cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(fluxpad_midi C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

pico_sdk_init()

add_executable(fluxpad_midi
    src/main.c
    src/usb_descriptors.c
)

target_link_libraries(fluxpad_midi
    pico_stdlib
    hardware_adc
    tinyusb_device
    tinyusb_board
)

target_include_directories(fluxpad_midi PRIVATE src)
pico_add_extra_outputs(fluxpad_midi)
