idf_component_register(
    SRCS "app_logic.cpp"
    INCLUDE_DIRS "."
    REQUIRES uart_hlk_ld2410s wifi_manager mqtt_publisher
)