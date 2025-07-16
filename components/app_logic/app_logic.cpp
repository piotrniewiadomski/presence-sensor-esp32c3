#include "app_logic.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_log.h"

#define HOLD_DURATION_MS 5000

static const char *TAG = "app_logic";

static bool presence_state = false;
static bool last_presence_state = false;



void app_logic_task(void *arg)
{
    wifi_init();
    presence_sensor_init();
    mqtt_init();

    while (1) {
        if (presence_state == true && last_presence_state == false) {
            ESP_LOGI(TAG, "Presence detected");
            wifi_ps_mode_none();
            mqtt_publish_presence(true);
            last_presence_state = true;
            wifi_ps_mode_min_modem();
        }

        if (presence_state == false && last_presence_state == true) {
            wifi_ps_mode_none();
            mqtt_publish_presence(false);
            last_presence_state = false;
            wifi_ps_mode_min_modem();
        }
        presence_state = presence_sensor_state();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_logic_main()
{
    xTaskCreate(app_logic_task, "app_logic_presence_task", 4096, NULL, 5, NULL);
}
