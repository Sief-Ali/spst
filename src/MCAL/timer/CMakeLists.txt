add_library(timer STATIC
    timer.c
    timer_registers.c
    include/timer.h
    include/timer_registers.h
)

target_include_directories(timer
    PUBLIC
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(timer
    PUBLIC
        avr_options
        utils
)
