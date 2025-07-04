#include "uart_hlk-ld2410s.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include <string.h>

/*
From docs:
The default baud rate of the serial port on the sensor is 115200.
The port has 1 stop bit and no parity bit.
1.Enter the command mode.
2.Set parameter commands or obtain parameter commands.
3.Exit the command mode.
HLK-LD2410S data communication uses a small-endian format

UART setup written based on https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/peripherals/uart.html
1. Install Drivers - Allocating ESP32-C3's resources for the UART driver
2. Set Communication Parameters - Setting baud rate, data bits, stop bits, etc.
3. Set Communication Pins - Assigning pins for connection to a device
4. Run UART Communication - Sending/receiving data
5. Use Interrupts - Triggering interrupts on specific communication events
6. Deleting a Driver - Freeing allocated resources if a UART communication is no longer required
*/

#define UART_TX_PIN 4
#define UART_RX_PIN 5

const int uart_buffer_size = (1024 * 2);
QueueHandle_t uart_queue;
const uart_port_t uart_num = UART_NUM_1;

uint8_t sensor_command_enable[] = {
    0xFD, 0xFC, 0xFB, 0xFA,
    0x04, 0x00,
    0xFF, 0x00,
    0x01, 0x00,
    0x04, 0x03, 0x02, 0x01
};

uint8_t sensor_command_enable_ack[] = {
    0xFD, 0xFC, 0xFB, 0xFA,
    0x08, 0x00,
    0xFF, 0x01,
    0x00, 0x00,
    0x03, 0x00,
    0x80, 0x00,
    0x04, 0x03, 0x02, 0x01
};

uint8_t sensor_command_end[] = {
    0xFD, 0xFC, 0xFB, 0xFA,
    0x02, 0x00,
    0xFF, 0x00,
    0x04, 0x03, 0x02, 0x01
};

uint8_t sensor_command_end_ack[] = {
    0xFD, 0xFC, 0xFB, 0xFA,
    0x04, 0x00,
    0xFF, 0x01,
    0x00, 0x00,
    0x04, 0x03, 0x02, 0x01
};

uint8_t sensor_command_write_config[] = {
    0xFD, 0xFC, 0xFB, 0xFA,
    0x26, 0x00,
    0x70, 0x00,
    0x05, 0x00, 0x0F, 0x00, 0x00, 0x00,
    0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x0C, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x0B, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x04, 0x03, 0x02, 0x01
};

uint8_t sensor_command_write_config_ack[] = {
    0xFD, 0xFC, 0xFB, 0xFA,
    0x04, 0x00,
    0x70, 0x01,
    0x00, 0x00,
    0x04, 0x03, 0x02, 0x01
};

bool presence_sensor_send_command(uint8_t *command, size_t command_len, const uint8_t *expected_ack, size_t ack_len);

int presence_sensor_init()
{
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    presence_sensor_send_command(
        sensor_command_enable,
        sizeof(sensor_command_enable),
        sensor_command_enable_ack,
        sizeof(sensor_command_enable_ack)
    );

    presence_sensor_send_command(
        sensor_command_write_config,
        sizeof(sensor_command_write_config),
        sensor_command_write_config_ack,
        sizeof(sensor_command_write_config_ack)
    );

    presence_sensor_send_command(
        sensor_command_end,
        sizeof(sensor_command_end),
        sensor_command_end_ack,
        sizeof(sensor_command_end_ack)
    );

    return 0;
};


bool presence_sensor_send_command(uint8_t *command, size_t command_len, const uint8_t *expected_ack, size_t ack_len) 
{
    // Read data from UART.
    uart_write_bytes(uart_num, (const char*)command, command_len);
    ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 100));

    uint8_t data[128];
    int len = uart_read_bytes(uart_num, data, sizeof(data), pdMS_TO_TICKS(100));

    return (len == ack_len) && (memcmp(data, expected_ack, ack_len) == 0);
}
