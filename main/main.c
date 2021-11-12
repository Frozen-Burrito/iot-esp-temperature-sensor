#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

#include "wifi_setup.h"
#include "dht22.h"

#define DHT_GPIO GPIO_NUM_22

void sensor_task(void* pvParameter)
{
    setDhtGpio(DHT_GPIO);

    while (1)
    {
        int result = read_dht();

        dht_error_handler(result);

        printf("Temperature: %.1f °C, ", getTemperature());
        printf("Humidity: %.1f %%\n", getHumidity());

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

    ESP_ERROR_CHECK(nvs_status);

    esp_err_t wifi_start_status = wifi_sta_init();

    ESP_ERROR_CHECK(wifi_start_status);

    xTaskCreate(&sensor_task, "DHT_Task", 2048, NULL, 3, NULL);
}
