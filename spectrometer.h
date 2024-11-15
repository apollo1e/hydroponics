// spectrometer.h

#ifndef SPECTROMETER_H
#define SPECTROMETER_H

#include <stdint.h>
#include "lwip/apps/mqtt.h"

// I2C address and MQTT topic for spectrometer
#define AS7341_ADDR 0x39
#define MQTT_TOPIC "pico/as7341/data"

// Function prototypes for spectrometer
void write_register(uint8_t reg, uint8_t value);
void as7341_init();
void read_and_publish_as7341_data(mqtt_client_t *client);

#endif // SPECTROMETER_H
