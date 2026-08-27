add_library(uart STATIC
    uart.c
    uart_registers.c
    include/uart.h
    include/uart_registers.h
)

target_include_directories(uart
    PUBLIC
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(uart
    PUBLIC
        avr_options
        utils
)
