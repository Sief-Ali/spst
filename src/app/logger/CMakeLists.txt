
add_library(logger STATIC
    logger.c
    include/logger.h
)

target_include_directories(logger
    PUBLIC
        include
)

target_link_libraries(logger
    PUBLIC
        avr_options
        serial
)
 