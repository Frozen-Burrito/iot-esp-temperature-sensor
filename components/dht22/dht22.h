#include <stdio.h>

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

void setDhtGpio(int gpio);
void dht_error_handler(int result);

int read_dht();

float getHumidity();
float getTemperature();
int getSignalLevel( int usTimeout, bool state );

#endif