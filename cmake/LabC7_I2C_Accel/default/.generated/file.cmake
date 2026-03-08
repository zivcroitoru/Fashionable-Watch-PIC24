# The following variables contains the files used by the different stages of the build process.
set(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemble)
set_source_files_properties(${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemblePreproc)
set_source_files_properties(${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemblePreproc} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_assemblePreproc})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Accel_i2c.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/display_manager.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/input_handler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/menu.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/menu_alarm.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/menu_analog_theme.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/menu_display.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/menu_format.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/menu_main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/menu_set_date.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/menu_set_time.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/time_engine.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../FashionableWatch/watch.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../System/clock.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../System/delay.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../System/system.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../i2cDriver/i2c1_driver.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../oledDriver/oledC.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../oledDriver/oledC_shapes.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../spiDriver/spi1_driver.c")
set_source_files_properties(${LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_link)
set(LabC7_I2C_Accel_default_default_XC16_FILE_TYPE_bin2hex)
set(LabC7_I2C_Accel_default_image_name "default.elf")
set(LabC7_I2C_Accel_default_image_base_name "default")

# The output directory of the final image.
set(LabC7_I2C_Accel_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/LabC7_I2C_Accel")

# The full path to the final image.
set(LabC7_I2C_Accel_default_full_path_to_image ${LabC7_I2C_Accel_default_output_dir}/${LabC7_I2C_Accel_default_image_name})
