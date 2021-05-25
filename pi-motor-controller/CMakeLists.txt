cmake_minimum_required(VERSION 3.12)

set(PROJ_NAME pi-motor-controller)

# Pull in SDK (must be before project)
include(${CMAKE_SOURCE_DIR}/pico-sdk/external/pico_sdk_import.cmake)

project(${PROJ_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Generate proto-bufs
execute_process(COMMAND /bin/flatc --cpp "${CMAKE_SOURCE_DIR}/../interfaces/motor.fbs")

# Initialize the SDK
pico_sdk_init()

add_executable(${PROJ_NAME}
        main.cpp
        comms.cpp
        led.cpp
        )

pico_enable_stdio_usb(${PROJ_NAME} 1)
pico_enable_stdio_uart(${PROJ_NAME} 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${PROJ_NAME}
        pico_stdlib
        pico_multicore
        hardware_pio
        hardware_pwm
)


# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJ_NAME})
