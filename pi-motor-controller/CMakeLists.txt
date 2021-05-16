cmake_minimum_required(VERSION 3.12)

set(PROJ_NAME pi-motor-controller)

# Pull in SDK (must be before project)
include(${CMAKE_SOURCE_DIR}/pico-sdk/external/pico_sdk_import.cmake)

project(${PROJ_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(${PROJ_NAME}
        blink.c
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${PROJ_NAME} pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJ_NAME})
