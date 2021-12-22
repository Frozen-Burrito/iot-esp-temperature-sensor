#include "esp_event.h"
#include "esp_log.h"

#include "mqtt_client.h"

#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

static const char* MQTT_TAG = "MQTT_CLIENT";

void mqtt_event_handler(
    void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data
);

int mqtt_publish_msg(const char* topic, const char* message, int qos);

void mqtt_client_init(const char* broker_uri, const char* user_name);

#endif