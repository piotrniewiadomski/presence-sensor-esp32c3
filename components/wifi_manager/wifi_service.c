#include "wifi_service.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_pm.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"

#define DEFAULT_SSID CONFIG_WIFI_SSID
#define DEFAULT_PWD  CONFIG_WIFI_PASSWORD

static const char *TAG = "wifi_service";

// Forward declarations
esp_err_t wifi_ps_mode_min_modem();
esp_err_t wifi_ps_mode_none();

// Wi-Fi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Wi-Fi started. Connecting...");
        ESP_ERROR_CHECK(esp_wifi_connect());

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Wi-Fi disconnected. Reconnecting...");
        ESP_ERROR_CHECK(esp_wifi_connect());

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

// Wi-Fi initialization and power save setup
static void wifi_power_save(void)
{
    ESP_LOGI(TAG, "Initializing Wi-Fi STA with power save...");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Setting power save mode to MIN_MODEM");
    ESP_ERROR_CHECK(wifi_ps_mode_min_modem());
}

// Main Wi-Fi init entry point
void wifi_init()
{
    ESP_LOGI(TAG, "Initializing NVS...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS Flash full or incompatible. Erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_power_save();
}

// Set Wi-Fi to minimum modem power save
esp_err_t wifi_ps_mode_min_modem()
{
    ESP_LOGI(TAG, "Wi-Fi power save: MIN_MODEM");
    return esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
}

// Disable Wi-Fi power save
esp_err_t wifi_ps_mode_none()
{
    ESP_LOGI(TAG, "Wi-Fi power save: NONE");
    return esp_wifi_set_ps(WIFI_PS_NONE);
}
