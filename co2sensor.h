// co2sensor.h

#ifndef CO2SENSOR_H
#define CO2SENSOR_H

#include <stdint.h>
#include "lwip/apps/mqtt.h"

// I2C address and MQTT topic for CO2 sensor
#define SCD41_ADDR 0x62
#define MQTT_TOPIC_PUBLISH "pico/scd41/data"

// Function prototypes for CO2 sensor
void read_co2_data(mqtt_client_t *client);

#endif // CO2SENSOR_H
