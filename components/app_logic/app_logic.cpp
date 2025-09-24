#include "app_logic.hpp"

#include "mqtt_publisher.h"
#include "wifi_service.h"
#include "uart_hlk_ld2410s.h"

#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define PRESENCE_PIN GPIO_NUM_3

RTC_DATA_ATTR static bool first_boot_done = false;
RTC_DATA_ATTR static bool wait_for_high = false;

static const char *TAG = "app_logic";

void app_logic_main(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PRESENCE_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    while (1)
    { 
        bool presence_level = gpio_get_level(PRESENCE_PIN);

        if (!first_boot_done) {
            presence_sensor_init();
            wifi_init();
            while(!wifi_is_connected());
            mqtt_init();
            while(!mqtt_connected());
            first_boot_done = true;
            ESP_LOGI(TAG, "First boot init done");
        } else {
            ESP_LOGI(TAG, "Woke up from deep sleep, by presence level: %d", presence_level);
            wifi_init();
            while(!wifi_is_connected());
            mqtt_init();
            while(!mqtt_connected());
        }

        mqtt_publish_presence(presence_level);
        vTaskDelay(pdMS_TO_TICKS(50));
        wait_for_high = !presence_level;

        ESP_LOGI(TAG, "Setting GPIO wake up level to %s", wait_for_high ? "HIGH" : "LOW");
        esp_deep_sleep_enable_gpio_wakeup((1 << PRESENCE_PIN), wait_for_high ? ESP_GPIO_WAKEUP_GPIO_HIGH : ESP_GPIO_WAKEUP_GPIO_LOW);

        presence_level = gpio_get_level(PRESENCE_PIN);
        ESP_LOGI(TAG, "GPIO level before sleep: %d", gpio_get_level(PRESENCE_PIN));
        if (presence_level != wait_for_high) esp_deep_sleep_start();
    }
}

