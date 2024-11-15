#include <stdio.h>
#include <stdint.h>
#include "hardware/i2c.h"
#include "lwip/apps/mqtt.h"

#define SCD41_ADDR 0x62
#define MQTT_TOPIC_PUBLISH "pico/scd41/data"

bool is_reading = false;

// Function to read CO2 sensor data
void read_co2_data(mqtt_client_t *client) {
    printf("Starting data read from CO2 sensor...\n");
    // printf("\n");
    printf("------ CO2 SENSOR  ------ \n");

    uint8_t command[2] = {0xEC, 0x05};
    i2c_write_blocking(i2c1, SCD41_ADDR, command, sizeof(command), true); // Updated to use i2c1
    sleep_ms(500);  // Wait for data to be ready

    uint8_t data[6];
     i2c_read_blocking(i2c1, SCD41_ADDR, data, sizeof(data), false); // Updated to use i2c1

    uint16_t co2 = (data[0] << 8) | data[1];
    float temperature = ((data[3] << 8) | data[4]) / 100.0;
    float humidity = ((data[5] << 8) | data[6]) / 100.0;

    printf("CO2: %d ppm, Temp: %.2f C, Humidity: %.2f %%\n", co2, temperature, humidity);

    char payload[128];
    snprintf(payload, sizeof(payload), "{\"co2\": %d, \"temperature\": %.2f, \"humidity\": %.2f}", co2, temperature, humidity);
    
    err_t err = mqtt_publish(client, MQTT_TOPIC_PUBLISH, payload, strlen(payload), 0, 0, NULL, NULL); // hmm setting QoS to 1 give co2 to be 0 but others will give value.. why??
    if (err == ERR_OK) {
        printf("CO2 data published.\n");
    } else {
        printf("Failed to publish CO2 data. Error code: %d\n", err);
    }
}
