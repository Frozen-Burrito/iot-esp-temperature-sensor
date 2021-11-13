#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#ifndef WIFI_SETUP_H_
#define WIFI_SETUP_H_

#define WIFI_SSID "TP-Link_B28D"
#define WIFI_PSWD "1234567890"
#define WIFI_MAX_RETRY 3

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

esp_err_t wifi_sta_init(void);

#endif