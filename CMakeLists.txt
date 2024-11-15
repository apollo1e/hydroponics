message("Building...")

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

# Project information
project(co2sensor C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(MAIN_PATH ${PROJECT_SOURCE_DIR})

# Set Wi-Fi credentials
if (DEFINED ENV{WIFI_SSID})
    set(WIFI_SSID $ENV{WIFI_SSID})
    message("Using WIFI_SSID from environment ('${WIFI_SSID}')")
else()
    message(FATAL_ERROR "WIFI_SSID environment variable not set")
endif()

if (DEFINED ENV{WIFI_PASSWORD})
    set(WIFI_PASSWORD $ENV{WIFI_PASSWORD})
    message("Using WIFI_PASSWORD from environment")
else()
    message(FATAL_ERROR "WIFI_PASSWORD environment variable not set")
endif()

# Initialize the SDK
pico_sdk_init()

# Add Sensirion driver files to the project
set(SENSIRION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/embedded-i2c-scd4x-master/sensirion_i2c.c
    ${CMAKE_CURRENT_LIST_DIR}/embedded-i2c-scd4x-master/sensirion_i2c_hal.c
    ${CMAKE_CURRENT_LIST_DIR}/embedded-i2c-scd4x-master/sensirion_common.c
    ${CMAKE_CURRENT_LIST_DIR}/embedded-i2c-scd4x-master/scd4x_i2c.c
)


# Include directories for dependencies
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/libs/lwip
    $ENV{PICO_EXTRAS_PATH}/lib/lwip/src/include
    $ENV{PICO_EXTRAS_PATH}/src/rp2_common/lwip/include
    ${CMAKE_CURRENT_LIST_DIR}/include  # Include the 'include' folder where lwipopts.h & arch.h is located
)

# Include Sensirion driver files directory
include_directories(${CMAKE_CURRENT_LIST_DIR}/embedded-i2c-scd4x-master)

# # Include directories for project-specific headers
# include_directories(
#   ${CMAKE_CURRENT_LIST_DIR}/include  # Include the 'include' folder where lwipopts.h is located
# )



# Add executable
add_executable(co2sensor
    ${MAIN_PATH}/main.c
    ${MAIN_PATH}/co2sensor.c  # Specify your main source file
    ${MAIN_PATH}/spectrometer.c  
    ${MAIN_PATH}/low_power_pico.c  # low power modes
    ${MAIN_PATH}/plant_config.c  # plant configs
    ${SENSIRION_SOURCES}      # Add all Sensirion .c files
)

# Link libraries needed for the project
target_link_libraries(co2sensor 
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background  # For Wi-Fi and lwIP support
    hardware_i2c
    pico_lwip_mqtt
    pico_lwip_core
    # FreeRTOS-Kernel-Heap4
)

# Set include directories
target_include_directories(co2sensor PRIVATE
    ${PICO_SDK_PATH}/lib/lwip/include
    ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_arch/include
    ${PICO_SDK_PATH}/lib/lwip/src/include
    ${CMAKE_CURRENT_LIST_DIR}/embedded-i2c-scd4x-master
    ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
)

# Enable USB serial output
pico_enable_stdio_usb(co2sensor 1)
pico_enable_stdio_uart(co2sensor 1)

# Create map/bin/hex file etc.
pico_add_extra_outputs(co2sensor)

# # Pass the Wi-Fi credentials to the code as compile definitions
target_compile_definitions(co2sensor PRIVATE
    WIFI_SSID="${WIFI_SSID}"
    WIFI_PASSWORD="${WIFI_PASSWORD}"

    # NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
    # LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
    # PING_USE_SOCKETS=1
)
