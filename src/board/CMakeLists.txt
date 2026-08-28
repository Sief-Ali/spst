add_library(board STATIC
    board.c
)

target_include_directories(board
    PUBLIC
        include
)

target_link_libraries(board
    PUBLIC
        avr_options
        utils
        led
        adc
        analog
        twi
        pwm_driver
)
