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

// FreeRTOS event group to signal wifi driver status.
static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char* WIFI_TAG = "WiFi Setup";

static int s_retry_count = 0;

esp_err_t wifi_sta_init(void);

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                // The WiFi driver was successfully initialized,
                // try to connect.
                esp_wifi_connect();
                break;

            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(WIFI_TAG, "WiFi connected");
                break;
            
            case WIFI_EVENT_STA_DISCONNECTED:
                if (s_retry_count < WIFI_MAX_RETRY)
                {
                    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                    esp_wifi_connect();
                    s_retry_count++;
                    ESP_LOGI(WIFI_TAG, "Retrying connection to AP...");
                }
                else 
                {   
                    // If the max amount of retries was reached,
                    // connection failed completely.
                    xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
                }

                ESP_LOGI(WIFI_TAG, "Connection to AP failed");
                break;

            default:
                ESP_LOGW(WIFI_TAG, "Unhandled WIFI_EVENT: %d", event_id);
                break;
        }
    }    
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_count = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

#endif