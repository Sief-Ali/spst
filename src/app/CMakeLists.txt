add_subdirectory(logger)
add_subdirectory(tracker)
add_subdirectory(panel)
add_subdirectory(controller)

add_library(app STATIC
    app.c
    config.c
)

target_include_directories(app
    PUBLIC
        include
)

target_link_libraries(app
    PUBLIC
    logger
    analog
    board
    servo
    lcd
    eeprom
    led
    rtc
    tracker
    panel
    controller
    freertos_kernel
    freertos_config
)
