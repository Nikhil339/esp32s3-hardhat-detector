idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp-dl
    PRIV_REQUIRES camera
)

# RAW FLASH: This correctly flashes the model to the 'model' partition without any filesystem metadata
if(CONFIG_APP_BUILD_GENERATE_BINARIES)
    set(model_bin "${CMAKE_CURRENT_SOURCE_DIR}/hardhat_model.espdl")
    # This adds a custom flash target for the model
    add_custom_target(flash_model 
        COMMAND ${PYTHON_EXECUTABLE} ${IDF_PATH}/components/esptool_py/esptool/esptool.py 
        --chip esp32s3 
        write_flash 0x310000 ${model_bin}
        COMMENT "Flashing hardhat_model.espdl to partition at 0x310000"
        VERBATIM)
endif()
