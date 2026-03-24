idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp-dl
    PRIV_REQUIRES esp_psram spi_flash
)

# You still absolutely need this line to embed your model!
target_add_binary_data(${COMPONENT_TARGET} "hardhat_model.espdl" TEXT)
