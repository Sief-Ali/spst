add_library(led STATIC
    led.c
    include/led.h
)

target_include_directories(led
    PUBLIC
        include
)

target_link_libraries(led
    PUBLIC
        avr_options
        gpio
        timer_service
)
