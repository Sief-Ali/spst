# CMake build : main application

#configure variables

set (MAIN_NAME "${PROJECT_NAME}main")

#configure directories
set (MAIN_MODULE_PATH "${PROJECT_SOURCE_DIR}/src")
set (MAIN_SRC_PATH  "${MAIN_MODULE_PATH}" )

set (UTILS_MODULE_PATH "${MAIN_SRC_PATH}/utils")
set (MCAL_MODULE_PATH "${MAIN_SRC_PATH}/MCAL")
set (HAL_MODULE_PATH "${MAIN_SRC_PATH}/HAL")
set (APP_MODULE_PATH  "${MAIN_MODULE_PATH}/app" )

file (GLOB MAIN_SOURCE_FILE "${APP_MODULE_PATH}/main.c")

add_subdirectory("${UTILS_MODULE_PATH}")
add_subdirectory("${MCAL_MODULE_PATH}")
add_subdirectory("${HAL_MODULE_PATH}")
# add_subdirectory("${APP_MODULE_PATH}")

#set target executable
add_executable (${PROJECT_NAME}.elf ${MAIN_SOURCE_FILE})

target_link_libraries(${PROJECT_NAME}.elf
    PRIVATE
        avr_options
        # app
)

add_custom_command(
  TARGET ${PROJECT_NAME}.elf
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
  COMMAND ${CMAKE_SIZE} --format=avr --mcu=${TARGET_MCU} ${PROJECT_NAME}.elf
  COMMENT "Generating HEX and printing size"
)