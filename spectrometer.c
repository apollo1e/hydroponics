#include <stdio.h>
#include "hardware/i2c.h"
#include "lwip/apps/mqtt.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define AS7341_ADDR 0x39
#define MQTT_TOPIC "pico/as7341/data"

// // Forward declaration of the write_register function
// void write_register(uint8_t reg, uint8_t value);

// Function to write to a register on the AS7341 sensor
void write_register(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_write_blocking(i2c0, AS7341_ADDR, data, 2, false);
}

// Function to initialize the AS7341 spectrometer
void as7341_init() {
    printf("Initializing AS7341 Spectrometer...\n");

    // Clear any previous settings
    write_register(0x80, 0x00);
    sleep_ms(10);

    // Power on the sensor
    write_register(0x80, 0x01); // PON = 1, SP_EN = 0
    sleep_ms(10);

    // Enable spectral measurement
    write_register(0x80, 0x03); // PON = 1, SP_EN = 1
    sleep_ms(100); // Wait for the sensor to initialize

    // Set gain for moderate sensitivity
    write_register(0xAA, 0x05);  // Set gain to 16x

    // Set integration time (adjust if needed for specific application)
    write_register(0x81, 0x40);  // Set integration time register

    printf("AS7341 Sensor Initialized.\n");
}



// Function to read a channel from the AS7341 spectrometer
uint16_t read_channel(uint8_t low_reg, uint8_t high_reg) {
    uint8_t low_byte, high_byte;
    i2c_write_blocking(i2c0, AS7341_ADDR, &low_reg, 1, true);
    i2c_read_blocking(i2c0, AS7341_ADDR, &low_byte, 1, false);
    i2c_write_blocking(i2c0, AS7341_ADDR, &high_reg, 1, true);
    i2c_read_blocking(i2c0, AS7341_ADDR, &high_byte, 1, false);

    return (high_byte << 8) | low_byte;
}

// Function to read and publish AS7341 data via MQTT
void read_and_publish_as7341_data(mqtt_client_t *client) {
    printf("\n");
    printf("------ SPECTROMETER ------ \n");

    as7341_init(); // Initialize the AS7341 sensor.

    // Read each channel's data
    uint16_t ch0 = read_channel(0x95, 0x96);
    uint16_t ch1 = read_channel(0x97, 0x98);
    uint16_t ch2 = read_channel(0x99, 0x9A);
    uint16_t ch3 = read_channel(0x9B, 0x9C);
    uint16_t ch4 = read_channel(0x9D, 0x9E);
    uint16_t ch5 = read_channel(0x9F, 0xA0);

    // Print data for debugging
    printf("AS7341 Data - CH0: %u, CH1: %u, CH2: %u, CH3: %u, CH4: %u, CH5: %u\n", ch0, ch1, ch2, ch3, ch4, ch5);

    // Prepare payload and publish data via MQTT
    char payload[256];
    snprintf(payload, sizeof(payload), "{\"ch0\": %u, \"ch1\": %u, \"ch2\": %u, \"ch3\": %u, \"ch4\": %u, \"ch5\": %u}", ch0, ch1, ch2, ch3, ch4, ch5);

    err_t err = mqtt_publish(client, MQTT_TOPIC, payload, strlen(payload), 0, 0, NULL, NULL);
    if (err == ERR_OK) {
        printf("Spectrometer data published.\n");
    } else {
        printf("Failed to publish spectrometer data. Error code: %d\n", err);
    }
    printf("\n");
}
