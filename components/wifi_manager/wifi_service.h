#include <stdbool.h>

#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

void wifi_init();
bool wifi_is_connected();

#ifdef __cplusplus
}
#endif

#endif // WIFI_SERVICE_H