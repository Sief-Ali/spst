cmake_minimum_required(VERSION 3.16)

include(cmake/toolchains/avr-gcc.cmake)
include(cmake/targets/atmega32.cmake)

project(
    spst
    LANGUAGES C
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)