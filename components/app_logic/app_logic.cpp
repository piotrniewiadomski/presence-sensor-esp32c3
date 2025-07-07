#include "app_logic.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"


#define WAKEUP_PIN GPIO_NUM_0

void IRAM_ATTR gpio_isr_handler(void* arg) {

}

void gpio_isr_setup()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << WAKEUP_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(WAKEUP_PIN, gpio_isr_handler, NULL);
}


void app_logic_main()
{
    gpio_isr_setup();
    wifi_init();
    presence_sensor_init();
    
}