#include "mqtt_publisher.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "mqtt_publisher";
static esp_mqtt_client_handle_t client = NULL;

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            mqtt_publish_presence(false);  // Initial state
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT Disconnected");
            break;
        default:
            break;
    }
}

void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_MQTT_BROKER_URI,
        .credentials.username = CONFIG_MQTT_USERNAME,
        .credentials.authentication.password = CONFIG_MQTT_PASSWORD,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void mqtt_publish_presence(bool presence)
{
    if (client == NULL) return;

    const char *topic = "home/presence/esp32c3";
    const char *msg = presence ? "ON" : "OFF";

    esp_mqtt_client_publish(client, topic, msg, 0, 1, 1);
}