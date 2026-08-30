set(FREERTOS_KERNEL_PATH "./FreeRTOS")

add_library(freertos_config INTERFACE)

target_include_directories(freertos_config 
    SYSTEM
    INTERFACE
        "./config")

# Select the heap port.  values between 1-4 will pick a heap.
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

# Select the native compile PORT
set(FREERTOS_PORT "GCC_ATMEGA" CACHE STRING "" FORCE)

# Adding the FreeRTOS-Kernel subdirectory
add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)