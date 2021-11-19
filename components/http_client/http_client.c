#include "http_client.h"

esp_err_t _http_event_handler(esp_http_client_event_t* event)
{
    static char* out_buffer;
    static int out_len;

    switch (event->event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(LOG_TAG, "HTTP_EVENT_ERROR");
            break;
        
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(LOG_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_HEADER");
            break;

        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_DATA, len=%d", event->data_len);

            if (!esp_http_client_is_chunked_response(event->client))
            {
                if (event->user_data)
                {
                    memcpy(event->user_data + out_len, event->data, event->data_len);
                } else 
                {
                    if (out_buffer == NULL)
                    {
                        out_buffer = (char*) malloc(esp_http_client_get_content_length(event->client));
                        out_len = 0;

                        if (out_buffer == NULL)
                        {
                            ESP_LOGE(LOG_TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }

                    memcpy(out_buffer + out_len, event->data, event->data_len);
                }

                out_len += event->data_len;
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_FINISH");

            if (out_buffer != NULL)
            {
                free(out_buffer);
                out_buffer = NULL;
            }

            out_len = 0;
            break;

        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(LOG_TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(event->data, &mbedtls_err, NULL);

            if (err != ESP_OK)
            {
                if (out_buffer != NULL)
                {
                    free(out_buffer);
                    out_buffer = NULL;
                }

                out_len = 0;
                ESP_LOGI(LOG_TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(LOG_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
    }

    return ESP_OK;
}