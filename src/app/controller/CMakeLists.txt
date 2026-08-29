add_library(controller STATIC
    controller.c
)

target_include_directories(controller
    PUBLIC
        include
)

target_link_libraries(controller
    PUBLIC
      tracker
      logger 
)