add_library(eeprom STATIC
    eeprom.c
)

target_include_directories(eeprom
    PUBLIC
        include
)

target_link_libraries(eeprom
    PUBLIC
        avr_options
        utils
)