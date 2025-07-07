#include "esp_err.h"

#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

void wifi_init();
esp_err_t wifi_ps_mode_min_modem();
esp_err_t wifi_ps_mode_none();

#ifdef __cplusplus
}
#endif

#endif // WIFI_SERVICE_H