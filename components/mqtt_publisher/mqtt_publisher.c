#include "mqtt_publisher.h"

#include "mqtt_client.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "mqtt_publisher";
static esp_mqtt_client_handle_t client = NULL;
static EventGroupHandle_t mqtt_event_group;
#define MQTT_CONNECTED_BIT BIT0

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;


    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        xEventGroupSetBits(mqtt_event_group, MQTT_CONNECTED_BIT);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        xEventGroupClearBits(mqtt_event_group, MQTT_CONNECTED_BIT);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR ID:%d", event->error_handle);
        xEventGroupClearBits(mqtt_event_group, MQTT_CONNECTED_BIT);
        break;

    default:
        break;
    }
}

void mqtt_init(void) {

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_MQTT_BROKER_URI,
        .credentials.username = CONFIG_MQTT_USERNAME,
        .credentials.authentication.password = CONFIG_MQTT_PASSWORD,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void mqtt_publish_presence(bool presence) {
    if (client == NULL) return;

    const char *topic = "home/presence/corridor";
    const char *msg = presence ? "ON" : "OFF";
    ESP_LOGI(TAG, "Presence: %d", (presence ? 1 : 0));
    esp_mqtt_client_publish(client, topic, msg, 0, 1, 1);
}

bool mqtt_connected() {
    EventBits_t bits = xEventGroupGetBits(mqtt_event_group);
    return (bits & MQTT_CONNECTED_BIT) != 0;
}