#include "uart_hlk_ld2410s.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>


#define UART_TX_PIN 4
#define UART_RX_PIN 5

static const char *TAG = "uart_sensor";

#define UART_BUFFER_SIZE (1024 * 2)
#define UART_TIMEOUT_MS 100

QueueHandle_t uart_queue;
const uart_port_t uart_num = UART_NUM_1;


static const uint8_t sensor_command_enable[] = {
    0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00,
    0x01, 0x00, 0x04, 0x03, 0x02, 0x01
};

static const uint8_t sensor_command_end[] = {
    0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00,
    0x04, 0x03, 0x02, 0x01
};

static const uint8_t sensor_command_write_config[] = {
    0xFD, 0xFC, 0xFB, 0xFA, 0x26, 0x00, 0x70, 0x00,
    0x05, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x0A, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x0C, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x0B, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x04, 0x03, 0x02, 0x01
};

static const uint8_t sensor_command_automatically_update_threshold[] = {
    0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0x09, 0x00,
    0x02, 0x00, 0x01, 0x00, 0x78, 0x00,
    0x04, 0x03, 0x02, 0x01
};

static void presence_sensor_send_command(const uint8_t *command, size_t command_len)
{
    ESP_LOGI(TAG, "Sending command (%d bytes)", command_len);

    if (uart_write_bytes(uart_num, (const char *)command, command_len) < 0) {
        ESP_LOGE(TAG, "UART write failed");
        return;
    }
}

int presence_sensor_init(void)
{
    ESP_LOGI(TAG, "Installing UART driver");
    ESP_ERROR_CHECK(uart_driver_install(uart_num, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 10, &uart_queue, 0));

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };

    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(TAG, "Sending initialization commands");
    presence_sensor_send_command(sensor_command_enable, sizeof(sensor_command_enable));
    vTaskDelay(100);
    presence_sensor_send_command(sensor_command_write_config, sizeof(sensor_command_write_config));
    vTaskDelay(100);
    presence_sensor_send_command(sensor_command_automatically_update_threshold, sizeof(sensor_command_automatically_update_threshold));
    vTaskDelay(100);
    presence_sensor_send_command(sensor_command_end, sizeof(sensor_command_end));

    ESP_LOGI(TAG, "Sensor initialization complete");
    return 0;
}
