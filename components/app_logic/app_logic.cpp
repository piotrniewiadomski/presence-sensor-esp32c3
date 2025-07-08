#include "app_logic.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#define PRESENCE_PIN GPIO_NUM_0
#define HOLD_DURATION_MS 5000

static volatile bool wake_flag = false;
static bool presence_state = false;
static TickType_t last_presence_tick = 0;

void IRAM_ATTR gpio_isr_handler(void* arg) {
    wake_flag = true;
}

void gpio_isr_setup()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PRESENCE_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PRESENCE_PIN, gpio_isr_handler, NULL);
}

void app_logic_task(void *arg)
{
    wifi_init();
    presence_sensor_init();
    mqtt_init();
    gpio_isr_setup();

    presence_state = false;
    last_presence_tick = 0;


    while (1) {
        if (wake_flag) {
            wifi_ps_mode_none();

            last_presence_tick = xTaskGetTickCount();

            if (!presence_state) {
                mqtt_publish_presence(true);
                presence_state = true;
            }

            wake_flag = false;
            wifi_ps_mode_min_modem();
        }

        if (presence_state && (xTaskGetTickCount() - last_presence_tick > pdMS_TO_TICKS(HOLD_DURATION_MS))) {
            wifi_ps_mode_none();
            mqtt_publish_presence(false);
            presence_state = false;
            wifi_ps_mode_min_modem();
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_logic_main()
{
    xTaskCreate(app_logic_task, "app_logic_presence_task", 4096, NULL, 5, NULL);
}
