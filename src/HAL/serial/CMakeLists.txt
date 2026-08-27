add_library(serial STATIC
    serial.c
    include/serial.h
)

target_include_directories(serial
    PUBLIC
        include
)

target_link_libraries(serial
    PUBLIC
        avr_options
        uart
)
 