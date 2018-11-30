lib_clock_add_architecture("stm32f1")
lib_clock_add_architecture("stm32f4")

if("${LIB_CLOCK_ARCHITECTURE}" STREQUAL "stm32f1")
    lib_clock_add_sourcefile_c(lib_clock_STM32.c)
    lib_clock_add_dependencies(dev_framework_STM32F1_CUBE lib_isr)
    lib_clock_add_private_definition(-DARCH_STM32F1) 
endif()

if("${LIB_CLOCK_ARCHITECTURE}" STREQUAL "stm32f4")
    lib_clock_add_sourcefile_c(lib_clock_STM32.c)
    lib_clock_add_dependencies(dev_framework_STM32F1_CUBE lib_isr)
    lib_clock_add_private_definition(-DARCH_STM32F4) 
endif()


