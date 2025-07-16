#include <stdbool.h>

#ifndef UART_HLK_LD2410S_H
#define UART_HLK_LD2410S_H


#ifdef __cplusplus
extern "C" {
#endif

int presence_sensor_init();
bool presence_sensor_state();


#ifdef __cplusplus
}
#endif

#endif // UART_HLK_LD2410S_H