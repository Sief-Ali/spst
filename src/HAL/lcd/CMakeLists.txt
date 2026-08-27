add_library(lcd STATIC
    lcd.c
    lcd_config.c
    include/lcd.h
    include/lcd_config.h
)

target_include_directories(lcd
    PUBLIC
        include
)

target_link_libraries(lcd
    PUBLIC
        avr_options
        twi
)
 