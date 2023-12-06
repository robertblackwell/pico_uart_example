cmake_minimum_required(VERSION 3.12)

set(ENV{PICO_SDK_PATH} /home/robert/pico/pico/pico-sdk)

set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
message("NOW")
message("PICO_SDK_PATH         ${PICO_SDK_PATH}")
message("env(PICO_SDK_PATH)    $ENV{PICO_SDK_PATH}")
message("NOW")
set(COMMON_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/common)
message("COMMON_SRC_DIR    ${COMMON_SRC_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})



# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# optional
# include(pico_extras_import_optional.cmake)

project(pico_debug_example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# options for changing the pins on which UART0 operates
# set(PICO_DEFAULT_UART_TX_PIN 16)
# set(PICO_DEFAULT_UART_RX_PIN 17)
# or in code
# 
# gpio_set_function(16, GPIO_FUNC_UART)
# gpio_set_function(17, GPIO_FUNC_UART)
# 

set(PICO_ROBOT_PATH ${PROJECT_SOURCE_DIR})
# set(PIC_CXX_ENABLE_EXCEPTIONS 1)

# Initialize the SDK
pico_sdk_init()

# add_subdirectory(src)
set(TARGET pico_uart_example)
add_executable(${TARGET} 
    src/main.c 
    src/stats.h src/stats.c
    src/progress.h src/progress.c 
    )
pico_enable_stdio_usb(${TARGET} 1)
pico_enable_stdio_uart(${TARGET} 0)
pico_add_extra_outputs(${TARGET})
target_link_libraries(${TARGET} pico_stdlib pico_unique_id)
# add_compile_options(-Wall
#         -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
#         -Wno-unused-function # we have some for the docs that aren't called
#         )

# if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
#     add_compile_options(-Wno-maybe-uninitialized)
# endif()

