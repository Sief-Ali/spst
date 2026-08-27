add_library(btn STATIC
    btn.c
    include/btn.h
)

target_include_directories(btn
    PUBLIC
        include
)

target_link_libraries(btn
    PUBLIC
        avr_options
        gpio
        ext_int
)
