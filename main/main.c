#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

#include "wifi_setup.h"
#include "dht22.h"
#include "http_client.h"

#define DHT_GPIO GPIO_NUM_22

// Ubidots api endpoint and token. Replace 'my-esp32' with 
// your device label.
#define API_URL "https://industrial.ubidots.com/api/v1.6/devices/my-esp32"
#define UBIDOTS_TOKEN "your-ubidots-token"

void sensor_task(void* pvParameter)
{
    set_dht_gpio(DHT_GPIO);

    while (1)
    {
        // Get sensor read, check for possible error.
        int result = read_dht();

        dht_error_handler(result);

        // Log values to terminal
        printf("Temperature: %.1f °C, ", get_temperature());
        printf("Humidity: %.1f %%\n", get_humidity());

        // Get sensor values as JSON string.
        char json_str_buffer[150];
        result = sensor_json_data(json_str_buffer, sizeof(json_str_buffer));

        dht_error_handler(result);

        // Make POST request to Ubidots Data API
        ESP_ERROR_CHECK(http_post_request(API_URL, UBIDOTS_TOKEN, json_str_buffer));
        
        // Delay between reads must be of at least 2 seconds.
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t nvs_status = nvs_flash_init();

    if (nvs_status == ESP_ERR_NVS_NO_FREE_PAGES || nvs_status == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      nvs_status = nvs_flash_init();
    }

    // Verify that nvs is initialized before starting WiFi driver.
    ESP_ERROR_CHECK(nvs_status);

    esp_err_t wifi_start_status = wifi_sta_init();

    ESP_ERROR_CHECK(wifi_start_status);

    // If WiFi connection was successful, start sensor task.
    xTaskCreate(&sensor_task, "dht_task", 8096, NULL, 5, NULL);
}
