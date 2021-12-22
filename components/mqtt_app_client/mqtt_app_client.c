#include "mqtt_app_client.h"

static esp_mqtt_client_handle_t client = NULL;

void log_err_if_nonzero(const char* message, int err_code)
{
    if (err_code != 0)
    {
        ESP_LOGE(MQTT_TAG, "Last error %s: 0x%x", message, err_code);
    }
}

void mqtt_event_handler(
    void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data
)
{
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);

    esp_mqtt_event_handle_t event = event_data;
    client = event->client;

    int msg_id;

    switch ((esp_mqtt_event_id_t) event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");
            // msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
            ESP_LOGI(MQTT_TAG, "Sent publish successful, msg_id=%d", event->msg_id);

            // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(MQTT_TAG, "Sent subscribe successful, msg_id=%d", event->msg_id);

            // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(MQTT_TAG, "Sent subscribe successful, msg_id=%d", event->msg_id);

            // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(MQTT_TAG, "Sent unsubscribe successful, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED");
            client = NULL;
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(MQTT_TAG, "Sent publish successful, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
            printf("Topic = %.*s\r\n", event->topic_len, event->topic);
            printf("Data = %.*s\r\n", event->data_len, event->data);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
            {
                log_err_if_nonzero("from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_err_if_nonzero("from tls stack", event->error_handle->esp_tls_stack_err);
                log_err_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(MQTT_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        
        default:
            ESP_LOGI(MQTT_TAG, "Other event id:%d", event_id);
            break;
    }

}

int mqtt_publish_msg(const char* topic, const char* message, int qos)
{
    if (client == NULL) 
    {
        ESP_LOGE(MQTT_TAG, "Client is not assigned.");
        return -1;
    }

    int msg_id = esp_mqtt_client_publish(client, topic, message, 0, qos, 0);

    ESP_LOGI(MQTT_TAG, "Published to topic '%d', message: %s", msg_id, message);

    return msg_id;
} 

void mqtt_client_init(const char* broker_uri, const char* user_name)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = broker_uri,
        .username = user_name,
        .port = 1883,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    esp_mqtt_client_start(client);
}