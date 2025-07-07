idf_component_register(
    SRCS "wifi_service.c"
    INCLUDE_DIRS "."
    REQUIRES esp_wifi esp_pm nvs_flash
)

set(CONFIG_KCONFIG_FILENAME "Kconfig")