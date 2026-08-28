add_library(servo STATIC
    servo.c
)

target_include_directories(servo
    PUBLIC
        include
)

target_link_libraries(servo
    PUBLIC
        pwm_driver
        gpio
)