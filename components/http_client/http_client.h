#include <string.h>

#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_log.h"

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#define MAX_HTTP_OUTPUT_BUFFER 2048

static const char* LOG_TAG = "HTTP";

/**
 * @brief Event handler for the http client.
 * 
 * @param event HTTP Event.
 */ 
esp_err_t _http_event_handler(esp_http_client_event_t* event);

/**
 * @brief Creates an HTTP client and performs a POST request. 
 * 
 * Initializes an HTTP client using [url],  _http_event_handler, a local
 * response buffer and a maximum retry count as configuration. 
 * 
 * Then, it builds a POST request with "Content-Type": "application/json",  
 * the authorization_token and the request_body. 
 * 
 * Finally, it performs (sends) the request and logs its status. 
 *  
 * Always cleans up the client after the request is sent, regardless of response
 * status code.
 * 
 * @param url target URL for the request.
 * @param auth_token authorization header for the request. (Sent in X-Auth-Token header).
 * @param request_body JSON string to be sent along the request.
 * 
 * @returns ESP_OK: request sent successfully.
 *          ESP_FAIL: client failed to initialize or request failed.
 */ 
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