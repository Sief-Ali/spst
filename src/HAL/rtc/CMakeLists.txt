add_library(rtc STATIC
    rtc.c
)

target_include_directories(rtc
    PUBLIC
        include
)

target_link_libraries(rtc
    PUBLIC
        avr_options
        twi
)