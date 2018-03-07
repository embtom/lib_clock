project(lib_clock_STM32)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")

if(${MCU} STREQUAL "cortex-m3")
    add_definitions(-DCORTEX_M3) 
ENDIF()

if(${MCU} STREQUAL "cortex-m4")
    add_definitions(-DCORTEX_M4) 
ENDIF()


#Generate the static library from the sources
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} itf_lib_convention itf_lib_clock dev_framework_STM32F1_CUBE)
#target_include_directories(${PROJECT_NAME} PUBLIC ${itf_lib_convention_SOURCE_DIR})
