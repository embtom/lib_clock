project(lib_clock)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")


#Generate the static library from the sources
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} lib_convention itf_lib_clock)