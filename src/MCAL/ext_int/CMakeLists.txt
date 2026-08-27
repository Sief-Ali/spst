add_library(ext_int STATIC
    ext_int.c
    ext_int_registers.c
    include/ext_int.h
    include/ext_int_registers.h
)

target_include_directories(ext_int
    PUBLIC
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ext_int
    PUBLIC
        avr_options
        utils
)
