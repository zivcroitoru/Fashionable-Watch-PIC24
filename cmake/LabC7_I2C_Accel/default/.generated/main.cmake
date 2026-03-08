include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(LabC7_I2C_Accel_default_library_list )

# Handle files with suffix s, for group default-XC16
if(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemble)
add_library(LabC7_I2C_Accel_default_default_XC16_assemble OBJECT ${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemble})
    LabC7_I2C_Accel_default_default_XC16_assemble_rule(LabC7_I2C_Accel_default_default_XC16_assemble)
    list(APPEND LabC7_I2C_Accel_default_library_list "$<TARGET_OBJECTS:LabC7_I2C_Accel_default_default_XC16_assemble>")

endif()

# Handle files with suffix S, for group default-XC16
if(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemblePreproc)
add_library(LabC7_I2C_Accel_default_default_XC16_assemblePreproc OBJECT ${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemblePreproc})
    LabC7_I2C_Accel_default_default_XC16_assemblePreproc_rule(LabC7_I2C_Accel_default_default_XC16_assemblePreproc)
    list(APPEND LabC7_I2C_Accel_default_library_list "$<TARGET_OBJECTS:LabC7_I2C_Accel_default_default_XC16_assemblePreproc>")

endif()

# Handle files with suffix c, for group default-XC16
if(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_compile)
add_library(LabC7_I2C_Accel_default_default_XC16_compile OBJECT ${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_compile})
    LabC7_I2C_Accel_default_default_XC16_compile_rule(LabC7_I2C_Accel_default_default_XC16_compile)
    list(APPEND LabC7_I2C_Accel_default_library_list "$<TARGET_OBJECTS:LabC7_I2C_Accel_default_default_XC16_compile>")

endif()

# Handle files with suffix s, for group default-XC16
if(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_dependentObject)
add_library(LabC7_I2C_Accel_default_default_XC16_dependentObject OBJECT ${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_dependentObject})
    LabC7_I2C_Accel_default_default_XC16_dependentObject_rule(LabC7_I2C_Accel_default_default_XC16_dependentObject)
    list(APPEND LabC7_I2C_Accel_default_library_list "$<TARGET_OBJECTS:LabC7_I2C_Accel_default_default_XC16_dependentObject>")

endif()

# Handle files with suffix elf, for group default-XC16
if(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_bin2hex)
add_library(LabC7_I2C_Accel_default_default_XC16_bin2hex OBJECT ${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_bin2hex})
    LabC7_I2C_Accel_default_default_XC16_bin2hex_rule(LabC7_I2C_Accel_default_default_XC16_bin2hex)
    list(APPEND LabC7_I2C_Accel_default_library_list "$<TARGET_OBJECTS:LabC7_I2C_Accel_default_default_XC16_bin2hex>")

endif()


# Main target for this project
add_executable(LabC7_I2C_Accel_default_image_xr51daR9 ${LabC7_I2C_Accel_default_library_list})

set_target_properties(LabC7_I2C_Accel_default_image_xr51daR9 PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${LabC7_I2C_Accel_default_output_dir}")
target_link_libraries(LabC7_I2C_Accel_default_image_xr51daR9 PRIVATE ${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_link})

# Add the link options from the rule file.
LabC7_I2C_Accel_default_link_rule( LabC7_I2C_Accel_default_image_xr51daR9)

# Call bin2hex function from the rule file
LabC7_I2C_Accel_default_bin2hex_rule(LabC7_I2C_Accel_default_image_xr51daR9)

