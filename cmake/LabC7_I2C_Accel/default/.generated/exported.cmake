set(DEPENDENT_MP_BIN2HEXLabC7_I2C_Accel_default_xr51daR9 "c:/Program Files/Microchip/xc16/v2.10/bin/xc16-bin2hex.exe")
set(DEPENDENT_DEPENDENT_TARGET_ELFLabC7_I2C_Accel_default_xr51daR9 ${CMAKE_CURRENT_LIST_DIR}/../../../../out/LabC7_I2C_Accel/default.elf)
set(DEPENDENT_TARGET_DIRLabC7_I2C_Accel_default_xr51daR9 ${CMAKE_CURRENT_LIST_DIR}/../../../../out/LabC7_I2C_Accel)
set(DEPENDENT_BYPRODUCTSLabC7_I2C_Accel_default_xr51daR9 ${DEPENDENT_TARGET_DIRLabC7_I2C_Accel_default_xr51daR9}/${sourceFileNameLabC7_I2C_Accel_default_xr51daR9}.s)
add_custom_command(
    OUTPUT ${DEPENDENT_TARGET_DIRLabC7_I2C_Accel_default_xr51daR9}/${sourceFileNameLabC7_I2C_Accel_default_xr51daR9}.s
    COMMAND ${DEPENDENT_MP_BIN2HEXLabC7_I2C_Accel_default_xr51daR9} ${DEPENDENT_DEPENDENT_TARGET_ELFLabC7_I2C_Accel_default_xr51daR9} --image ${sourceFileNameLabC7_I2C_Accel_default_xr51daR9} ${addressLabC7_I2C_Accel_default_xr51daR9} ${modeLabC7_I2C_Accel_default_xr51daR9} -mdfp=C:/Users/zuver/.mchp_packs/Microchip/PIC24F-GA-GB_DFP/1.4.141/xc16 
    WORKING_DIRECTORY ${DEPENDENT_TARGET_DIRLabC7_I2C_Accel_default_xr51daR9}
    DEPENDS ${DEPENDENT_DEPENDENT_TARGET_ELFLabC7_I2C_Accel_default_xr51daR9})
add_custom_target(
    dependent_produced_source_artifactLabC7_I2C_Accel_default_xr51daR9 
    DEPENDS ${DEPENDENT_TARGET_DIRLabC7_I2C_Accel_default_xr51daR9}/${sourceFileNameLabC7_I2C_Accel_default_xr51daR9}.s
    )
