add_library(panel STATIC
    panel.c
)

target_include_directories(panel
    PUBLIC
        include
)

target_link_libraries(panel
    PUBLIC
        board
        servo
)