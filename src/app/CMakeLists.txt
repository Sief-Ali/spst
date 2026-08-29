add_subdirectory(logger)

add_library(app STATIC
    app.c
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
)