add_library(tracker STATIC
    tracker.c
)

target_include_directories(tracker
    PUBLIC
        include
)
