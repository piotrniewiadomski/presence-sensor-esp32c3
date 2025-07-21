#include "stdbool.h"

#ifndef MQTT_PUBLISHER_H
#define MQTT_PUBLISHER_H

#ifdef __cplusplus
extern "C" {
#endif

void mqtt_init();
void mqtt_publish_presence(bool presence);
bool mqtt_connected();

#ifdef __cplusplus
}
#endif

#endif // MQTT_PUBLISHER_H