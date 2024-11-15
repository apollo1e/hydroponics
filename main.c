#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "co2sensor.h"         // Header for CO2 sensor functions
#include "spectrometer.h"      // Header for Spectrometer sensor functions
#include "plant_config.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "hardware/i2c.h"
#include "low_power_pico.h"

#define MQTT_BROKER_IP "5.196.78.28"
// #define MQTT_BROKER_IP "172.20.10.3"

#define MQTT_PORT 1883

mqtt_client_t *co2_client;
mqtt_client_t *spectro_client;
bool mqtt_connected = false;

// Forward declarations for sensor functions
void read_co2_data(mqtt_client_t *client);
void read_and_publish_as7341_data(mqtt_client_t *client);

// Function to reset MQTT client state
void reset_mqtt_client(mqtt_client_t *client) {
    if (client && mqtt_connected) {
        mqtt_disconnect(client);
        mqtt_connected = false;
        printf("Disconnected MQTT client to clear cache.\n");
    }
    connect_to_mqtt_broker(client); // Reconnect to broker
}

// I2C initialization function
void i2c_init_pico() {
    // Initialize i2c0 for the Spectrometer (SDA = GP0, SCL = GP1)
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    // Initialize i2c1 for the CO2 sensor (SDA = GP2, SCL = GP3)
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
}

// Wi-Fi connection function with IP display
void connect_wifi() {
    if (cyw43_arch_init() != 0) {
        printf("Failed to initialize Wi-Fi.\n");
        return;
    }
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 60000)) {
        printf("Failed to connect to Wi-Fi.\n");
    } else {
        printf("Connected to Wi-Fi.\n");

        // Display IP address using netif_default
        if (netif_default != NULL) {
            printf("Pico W IP address: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
        } else {
            printf("Failed to obtain IP address.\n");
        }
    }
}


// Callback function to handle MQTT connection status
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Successfully connected to MQTT broker.\n");
        mqtt_connected = true;
    } else {
        // Failed connection; print detailed information
        printf("Failed to connect to MQTT broker with status: %d\n", status);
        
        // Additional status check
        if (status == 256) {
            printf("Status 256: Potential network configuration or broker unreachable.\n");
        } else {
            printf("Other error: Check Wi-Fi, broker settings, and credentials.\n");
        }

        mqtt_connected = false;
    }
}

// MQTT connection function with keep-alive and persistent connection
void connect_to_mqtt_broker(mqtt_client_t *client) {
    struct mqtt_connect_client_info_t client_info = {
        .client_id = "PicoSensorClient",
        .keep_alive = 60  // 60 seconds keep-alive interval
    };

    ip4_addr_t broker_ip;
    if (!ip4addr_aton(MQTT_BROKER_IP, &broker_ip)) {
        printf("Failed to convert MQTT broker IP.\n");
        return;
    }

    printf("Connecting to MQTT broker at %s:%d...\n", MQTT_BROKER_IP, MQTT_PORT);
    mqtt_client_connect(client, &broker_ip, MQTT_PORT, mqtt_connection_cb, NULL, &client_info);
    printf("MQTT connection initiated.\n");
}

// Check and reconnect to MQTT broker if disconnected
void check_mqtt_connection(mqtt_client_t *client) {
    if (!mqtt_connected) {
        printf("Attempting to reconnect to MQTT broker...\n");
        connect_to_mqtt_broker(client);
    }
}

// // Define configurations for different plants
// PlantConfig plantConfigs[] = {
//     { "Lettuce", 24, 7, 400, 50, 70, 5000, 0.5, 1, 1 },   // Lettuce config
//     { "Tomato", 30, 18, 450, 55, 75, 6000, 0.7, 1, 1 },   // Tomato config
//     { "Basil", 28, 15, 400, 50, 65, 5500, 0.6, 1, 1 }     // Basil config
// };

// const int numPlants = sizeof(plantConfigs) / sizeof(plantConfigs[0]);

// void displayPlantConfig(PlantConfig *config) {
//     printf("Plant Name: %s\n", config->plant_name);
//     printf("Max Temperature: %d\n", config->max_temperature);
//     printf("Min Temperature: %d\n", config->min_temperature);
//     printf("Optimal CO2: %d\n", config->optimal_co2);
//     printf("Min Moisture: %d\n", config->min_moisture);
//     printf("Max Moisture: %d\n", config->max_moisture);
//     printf("Optimal Light: %d\n", config->optimal_light);
//     printf("Max Velocity: %.2f\n", config->max_velocity);
//     printf("Fan Activation: %d\n", config->fan_activation);
//     printf("Water Pump Control: %d\n", config->water_pump_control);
// }

int main() {
    stdio_init_all();

    sleep_ms(10000); // wait 10 seconds .. 
    PlantConfig config = {0}; // Initialize all fields to zero

    while (true) {
        printf("Select Plant Configuration:\n1. Lettuce\n2. Tomato\n3. Basil\n");
        int plant_choice = getchar_timeout_us(8000000); // wait 8 seconds for input

        if (plant_choice == PICO_ERROR_TIMEOUT) {
            printf("No input received. Please enter a selection.\n");
        } else if (plant_choice == '1') {
            printf("Lettuce configuration selected.\n");
            displayPlantConfig(&plantConfigs[0]);
            break;
        } else if (plant_choice == '2') {
            printf("Tomato configuration selected.\n");
            displayPlantConfig(&plantConfigs[1]);
            break;
        } else if (plant_choice == '3') {
            printf("Basil configuration selected.\n");
            displayPlantConfig(&plantConfigs[2]);
            break;
        } else {
            printf("Invalid selection. Please choose 1, 2, or 3.\n");
        }
        
        sleep_ms(2000);  // Delay between prompts
    }
    printf("Starting initialization...\n");
    i2c_init_pico();
    connect_wifi();
    sleep_ms(10000);
    // Initialize clients only once at the start
    co2_client = mqtt_client_new();
    spectro_client = mqtt_client_new();

    connect_to_mqtt_broker(co2_client);
    connect_to_mqtt_broker(spectro_client);

    // printf("Select Plant Configuration:\n1. Potato\n2. Carrot\n");

    while (true) {
        printf("\n**** BEGINNING OF WHILE LOOP ****\n");
        printf("Checking MQTT connection status...\n");
        printf("co2 status :\n");
        check_mqtt_connection(co2_client);
        printf("Spectrometer status :\n");
        check_mqtt_connection(spectro_client);
        printf("\n");

        if (mqtt_connected) {
            printf("######################################################################\n");
            printf("MQTT connection looks good...\n");
            printf("Reading and publishing CO2 sensor data...  ***\n");
            read_co2_data(co2_client);
            
            printf("-----\n");
            // Clear cache and reset the MQTT client before publishing the second sensor data
            reset_mqtt_client(spectro_client);
            printf("-----\n");
            sleep_ms(15000); 
            printf("Reading and publishing spectrometer sensor data... ***\n");
            read_and_publish_as7341_data(spectro_client);

            reset_mqtt_client(co2_client);
            printf("######################################################################\n");

              
        } else {
            printf("MQTT not connected. Skipping data publish.\n");
        }

        sleep_ms(18000);  // Delay to avoid rapid rechecking
    }

    return 0;
}
