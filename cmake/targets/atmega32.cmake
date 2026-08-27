set(TARGET_MCU atmega32 CACHE STRING "Target AVR TARGET_MCU")
set(TARGET_F_CPU 16000000UL CACHE STRING "CPU frequency in Hz")
# set(AVR_PROGRAMMER "arduino" CACHE STRING "Avrdude programmer type")
# set(AVR_PORT "/dev/ttyACM0" CACHE STRING "Serial upload port")

add_library(avr_options INTERFACE)
target_compile_definitions(avr_options INTERFACE F_CPU=${TARGET_F_CPU})
target_compile_options(avr_options INTERFACE
    -mmcu=${TARGET_MCU}
    -Wall
    -Wextra
    -Wpedantic
    -Os
    -ffunction-sections
    -fdata-sections
)
target_link_options(avr_options INTERFACE
    -mmcu=${TARGET_MCU}
    -Wl,--gc-sections
)