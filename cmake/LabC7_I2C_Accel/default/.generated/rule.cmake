# The following functions contains all the flags passed to the different build stages.

set(PACK_REPO_PATH "C:/Users/zuver/.mchp_packs" CACHE PATH "Path to the root of a pack repository.")

function(LabC7_I2C_Accel_default_default_XC16_assemble_rule target)
    set(options
        "-g"
        "-mcpu=24FJ256GA705"
        "-Wa,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-g,--defsym=__MPLAB_DEBUGGER_PK3=1,--no-relax"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC24F-GA-GB_DFP/1.4.141/xc16")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG=1"
        PRIVATE "__MPLAB_DEBUGGER_PK3=1"
        PRIVATE "XPRJ_default=default")
endfunction()
function(LabC7_I2C_Accel_default_default_XC16_assemblePreproc_rule target)
    set(options
        "-x"
        "assembler-with-cpp"
        "-g"
        "-mcpu=24FJ256GA705"
        "-Wa,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-g,--defsym=__MPLAB_DEBUGGER_PK3=1,--no-relax"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC24F-GA-GB_DFP/1.4.141/xc16")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG=1"
        PRIVATE "__MPLAB_DEBUGGER_PK3=1"
        PRIVATE "XPRJ_default=default")
endfunction()
function(LabC7_I2C_Accel_default_default_XC16_compile_rule target)
    set(options
        "-g"
        "-mcpu=24FJ256GA705"
        "-O0"
        "-msmart-io=1"
        "-Wall"
        "-msfr-warn=off"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC24F-GA-GB_DFP/1.4.141/xc16")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG"
        PRIVATE "__MPLAB_DEBUGGER_PK3=1"
        PRIVATE "XPRJ_default=default"
        PRIVATE "FCY=4000000")
    target_include_directories(${target}
        PRIVATE "bsp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
endfunction()
function(LabC7_I2C_Accel_default_dependentObject_rule target)
    set(options
        "-c"
        "-mcpu=24FJ256GA705"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC24F-GA-GB_DFP/1.4.141/xc16")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
endfunction()
function(LabC7_I2C_Accel_default_link_rule target)
    set(options
        "-g"
        "-mcpu=24FJ256GA705"
        "-Wl,--script=p24FJ256GA705.gld,--local-stack,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_PK3=1,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path=${CMAKE_CURRENT_SOURCE_DIR}/../../..,--no-force-link,--smart-io,--report-mem,--memorysummary,memoryfile.xml"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC24F-GA-GB_DFP/1.4.141/xc16")
    list(REMOVE_ITEM options "")
    target_link_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG=__DEBUG"
        PRIVATE "__MPLAB_DEBUGGER_PK3=1"
        PRIVATE "XPRJ_default=default")
endfunction()
function(LabC7_I2C_Accel_default_bin2hex_rule target)
    add_custom_target(
        LabC7_I2C_Accel_default_Bin2Hex ALL
        COMMAND ${MP_BIN2HEX} ${LabC7_I2C_Accel_default_image_name} -a -mdfp=${PACK_REPO_PATH}/Microchip/PIC24F-GA-GB_DFP/1.4.141/xc16
        WORKING_DIRECTORY ${LabC7_I2C_Accel_default_output_dir}
        BYPRODUCTS "${LabC7_I2C_Accel_default_output_dir}/${LabC7_I2C_Accel_default_image_base_name}.hex"
        COMMENT "Convert build file to .hex")
    add_dependencies(LabC7_I2C_Accel_default_Bin2Hex ${target})
endfunction()
