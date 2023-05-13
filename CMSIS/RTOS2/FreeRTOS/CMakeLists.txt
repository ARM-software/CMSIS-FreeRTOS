# defines library FW3_CMSIS_TARGET

add_library(FW3_CMSIS_TARGET)
target_sources(
  FW3_CMSIS_TARGET
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/cmsis_os2.c
          ${CMAKE_CURRENT_SOURCE_DIR}/Source/freertos_evr.c
          ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_systick.c
          ${CMAKE_CURRENT_SOURCE_DIR}/Source/ARM/clib_arm.c)

set_source_files_properties(${FW3_CMSIS_DIR}/Source/cmsis_os1.c
                            PROPERTIES COMPILE_OPTIONS -Wno-switch-enum)
