#include <string.h>

#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_log.h"

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#define MAX_HTTP_OUTPUT_BUFFER 2048

static const char* LOG_TAG = "HTTP";

esp_err_t _http_event_handler(esp_http_client_event_t* event);

static esp_err_t http_post_request(const char* url, const char* auth_token, const char* request_body)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,     // Address of local buffer for response
        .max_redirection_count = 5,             // Redirection count for Ubidots API
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    if (client == NULL) return ESP_FAIL;

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "X-Auth-Token", auth_token);
    esp_http_client_set_header(client, "User-Agent", "ESP32");
    esp_http_client_set_post_field(client, request_body, strlen(request_body));

    ESP_LOGI(LOG_TAG, "Request: %s", request_body);

    esp_err_t req_status = esp_http_client_perform(client);

    if (req_status == ESP_OK)
    {
        ESP_LOGI(LOG_TAG, "HTTP POST Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    }
    else 
    {
        ESP_LOGE(LOG_TAG, "HTTP POST request failed: %s", esp_err_to_name(req_status));
    }

    esp_http_client_cleanup(client);

    return req_status;
}

#endif