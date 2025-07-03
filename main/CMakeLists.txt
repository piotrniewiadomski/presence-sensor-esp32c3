idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES uart_hlk-ld2410s wifi_manager mqtt_client app_logic
)