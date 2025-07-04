idf_component_register(
    SRCS "uart_hlk-ld2410s.c"
    INCLUDE_DIRS "."
    REQUIRES driver freertos esp_system
)