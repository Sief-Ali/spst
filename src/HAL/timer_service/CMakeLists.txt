add_library(timer_service STATIC
    timer_service.c
    include/timer_service.h
)

target_include_directories(timer_service
    PUBLIC
        include
)

target_link_libraries(timer_service
    PUBLIC
        avr_options
        timer
)
 