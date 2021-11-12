#include "dht22.h"

#define LOCAL_LOG_LEVEL ESP_LOG_VERBOSE

#define MAX_DHT_DATA 5

static const char* DHT_TAG = "DHT";

int dhtGpio = 23;
float humidity = 0.;
float temperature = 0.;

void setDhtGpio( int gpio ) 
{
    dhtGpio = gpio;
}

float getHumidity() { return humidity; }
float getTemperature() { return temperature; }

void dht_error_handler(int response)
{
	switch(response) 
    {
		case DHT_TIMEOUT_ERROR :
			ESP_LOGE( DHT_TAG, "Sensor Timeout\n" );
			break;

		case DHT_CHECKSUM_ERROR:
			ESP_LOGE( DHT_TAG, "CheckSum error\n" );
			break;

		case DHT_OK:
			break;

		default :
			ESP_LOGE( DHT_TAG, "Unknown error\n" );
	}
}

int getSignalLevel( int usTimeOut, bool state )
{

	int uSec = 0;
	while( gpio_get_level(dhtGpio)==state ) 
    {
		if( uSec > usTimeOut ) 
			return -1;
		
		++uSec;
		ets_delay_us(1);
	}
	
	return uSec;
}

int read_dht()
{
    int uSec = 0;

    uint8_t dhtData[MAX_DHT_DATA];
    uint8_t byteIndex = 0;
    uint8_t bitIndex = 7;

	for (int i = 0; i < MAX_DHT_DATA; i++) 
		dhtData[i] = 0;
    
    // Send start signal to sensor.
	gpio_set_direction( dhtGpio, GPIO_MODE_OUTPUT );

	gpio_set_level( dhtGpio, 0 );
	ets_delay_us( 3000 );			

	gpio_set_level( dhtGpio, 1 );
	ets_delay_us( 25 );

    // Get sensor response signal, low for 80us and then high for 80us.
	gpio_set_direction( dhtGpio, GPIO_MODE_INPUT );	
  
	uSec = getSignalLevel( 85, 0 );
	if (uSec < 0) return DHT_TIMEOUT_ERROR; 

	uSec = getSignalLevel( 85, 1 );
	if (uSec < 0) return DHT_TIMEOUT_ERROR;

    // If initial communication with sensor was successful, 
    // start data transfer.
	for (int i = 0; i < 40; i++ ) 
    {
        // Start new transmission by sending 50us signal in low.
		uSec = getSignalLevel( 56, 0 );
		if (uSec < 0) return DHT_TIMEOUT_ERROR;

		uSec = getSignalLevel( 75, 1 );
		if (uSec < 0) return DHT_TIMEOUT_ERROR;

		// add the current read to the output data
		// since all dhtData array where set to 0 at the start, 
		// only looi for "1" (>28us us)
        
        // If sensor response signal was high for more than 28us, 
        // data bit is 1.
		if (uSec > 40) 
        {
			dhtData[ byteIndex ] |= (1 << bitIndex);
        }

		if (bitIndex == 0) 
        { 
            bitIndex = 7; ++byteIndex; 
        }
		else bitIndex--;
	}

    // Get humidity from bytes 0 and 1.
	humidity = dhtData[0];
	humidity *= 0x100;
	humidity += dhtData[1];
	humidity /= 10;

	// Get temperature from bytes 2 and 3.
	temperature = dhtData[2] & 0x7F;	
	temperature *= 0x100;
	temperature += dhtData[3];
	temperature /= 10;

    // Make temperature negative if sign bit is high.
	if( dhtData[2] & 0x80 ) 
		temperature *= -1;

    // Verify data checksum, which is the sum of data 8 bits masked out 0FF.
	if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF)) 
    {
		return DHT_OK;
    }
    else
    {
        return DHT_CHECKSUM_ERROR;
    } 
}
