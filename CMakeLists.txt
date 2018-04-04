project(lib_clock)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} itf_lib_convention itf_lib_clock)
#add_library(${PROJECT_NAME} OBJECT ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ./)  
#target_include_directories(${PROJECT_NAME} PUBLIC ${itf_lib_convention_SOURCE_DIR} ${itf_lib_clock_SOURCE_DIR})

