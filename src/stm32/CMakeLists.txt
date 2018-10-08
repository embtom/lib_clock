lib_clock_add_architecture("stm32")

if("${LIB_CLOCK_ARCHITECTURE}" STREQUAL "stm32")
    lib_clock_add_sourcefile_c(lib_clock_STM32.c)
    
    if(${MCU} STREQUAL "cortex-m3")
        lib_clock_add_private_definition(-DCORTEX_M3) 
    ENDIF()

    if(${MCU} STREQUAL "cortex-m4")
        lib_clock_add_private_definition(-DCORTEX_M4) 
    ENDIF() 
    
    lib_clock_add_dependencies(dev_framework_STM32F1_CUBE lib_isr)
endif()

