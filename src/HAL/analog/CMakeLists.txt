add_library(analog STATIC
    analog.c
    include/analog.h
)

target_include_directories(analog
    PUBLIC
        include
)

target_link_libraries(analog
    PUBLIC
        avr_options
        adc
)
 