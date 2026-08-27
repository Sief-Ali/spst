add_library(twi STATIC
    twi.c
    include/twi.h
    include/twi_types.h
)

target_include_directories(twi
    PUBLIC
        include
)

target_link_libraries(twi
    PUBLIC
        avr_options
        utils
)
