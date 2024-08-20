cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(vvvf_drive C CXX ASM)
set(CMAKE_C_STNDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(vvvf_drive)

set(PICO_COPY_TO_RAM,1)

target_sources(vvvf_drive PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/gpio_control.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(vvvf_drive PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(vvvf_drive PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board pico_multicore)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(vvvf_drive PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(vvvf_drive)
