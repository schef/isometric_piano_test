cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(fluxpad_midi C CXX ASM)

# Compatibility entry point for existing Fluxpad build directories and scripts.
# The 60-key controller firmware is not implemented yet.
message(STATUS "Building the Fluxpad example, not the planned 60-key controller")
include(${CMAKE_CURRENT_LIST_DIR}/examples/fluxpad_midi/CMakeLists.txt)
