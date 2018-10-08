lib_clock_add_architecture("posix")

if("${LIB_CLOCK_ARCHITECTURE}" STREQUAL "posix")
    lib_clock_add_sourcefile_c(lib_clock_POSIX.c)
endif()

