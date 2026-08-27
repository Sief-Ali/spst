add_library(adc STATIC
    adc.c
    adc_registers.c
    include/adc.h
    include/adc_registers.h
    include/adc_types.h
)

target_include_directories(adc
    PUBLIC
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(adc
    PUBLIC
        avr_options
        utils
)
