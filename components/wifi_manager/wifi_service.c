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

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;

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
        wifi_event_sta_disconnected_t *disconn = (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGW(TAG, "Wi-Fi disconnected. Reason: %d", disconn->reason);
        ESP_ERROR_CHECK(esp_wifi_connect());

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Wi-Fi initialization and power save setup
static void wifi_power_save(void)
{
    esp_log_level_set("wifi", CONFIG_LOG_MAXIMUM_LEVEL);
    esp_log_level_set("wifi_init", ESP_LOG_VERBOSE);

    ESP_LOGI(TAG, "Initializing Wi-Fi STA with power save");
    
    s_wifi_event_group = xEventGroupCreate();
    assert(s_wifi_event_group != NULL);

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
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .listen_interval = 3,
            .pmf_cfg = {
                .capable = false,
                .required = false,
            }
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G);
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                        WIFI_CONNECTED_BIT,
                                        pdFALSE,
                                        pdTRUE,
                                        portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI("wifi", "connected to ap");
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE("wifi", "Failed to connect to SSID");
    } else {
        ESP_LOGE("wifi", "UNEXPECTED EVENT");
    }
    
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
