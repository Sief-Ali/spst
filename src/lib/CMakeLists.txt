set(FREERTOS_KERNEL_PATH "./FreeRTOS")

add_library(freertos_config INTERFACE)

target_include_directories(freertos_config 
    SYSTEM
    INTERFACE
        "./config")

# The kernel and its port are compiled as separate targets, so they must also
# receive the selected AVR device and F_CPU definitions.
target_link_libraries(freertos_config INTERFACE avr_options)

# Select the heap port.  values between 1-4 will pick a heap.
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

# Select the native compile PORT
set(FREERTOS_PORT "GCC_ATMEGA" CACHE STRING "" FORCE)

# Adding the FreeRTOS-Kernel subdirectory
add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)
