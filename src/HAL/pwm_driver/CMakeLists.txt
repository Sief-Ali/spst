add_library(pwm_driver STATIC
    pwm_driver.c
    include/pwm_driver.h
)

target_include_directories(pwm_driver
    PUBLIC
        include
)

target_link_libraries(pwm_driver
    PUBLIC
        avr_options
        timer
)
 