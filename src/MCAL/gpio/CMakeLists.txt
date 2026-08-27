add_library(gpio STATIC
    gpio.c
    gpio_registers.c
    include/gpio.h
)

target_include_directories(gpio
    PUBLIC
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(gpio
    PUBLIC
        avr_options
        utils
)
