#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "driver/gpio.h"

#ifndef DHT22_H_
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR 1
#define DHT_TIMEOUT_ERROR 2
#define DHT_DATA_FORMAT_ERROR 3

#define LOCAL_LOG_LEVEL ESP_LOG_VERBOSE

#define MAX_DHT_DATA 5

/**
 * @brief Set gpio for sensor data transmission
 * 
 * @param gpio_num_t: GPIO number
 */
void set_dht_gpio(gpio_num_t gpio_num);

/**
 * @brief Check if read_dht response is ok
 * 
 * @param int: Result from read_dht
 */ 
void dht_error_handler(int result);

/**
 * @brief Establish connection with DHT sensor, then receive data.
 * 
 * If sensor connection and read are successful, update 
 * temperature and humidity with new values.
 * 
 * @return DHT_OK: success
 *         DHT_CHECKSUM_ERROR: data is not correct
 *         DHT_TIMEOUT_ERROR: sensor failed to respond
 */ 
int read_dht();

/**
 * @brief Get the current humidity
 * 
 * @return float: current humidity
 */
float get_humidity();

/**
 * @brief Get the current temperature
 * 
 * @return float: current temperature
 */
float get_temperature();

/**
 * @brief Get the current sensor values and serialize them to JSON
 * 
 * The JSON string follows the format described in 
 * the Ubidots Data API Reference.
 * 
 * The last char element of the resulting buffer will be a null 
 * termination character.
 * 
 * @param char*: data_buffer for JSON string
 * @param size_t: length of the buffer
 */
int sensor_json_data(char* data_buffer, size_t buff_len);

int get_signal_level( int usTimeout, bool state );

#endif