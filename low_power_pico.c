// low_power_pico.c  LIBRARY FOR THE POWER MODES 

#include "low_power_pico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pll.h" // Include the header file that defines CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_PLL_SYS
#include "lwip/apps/mqtt.h"
#include "lwip/ip4_addr.h"

// I2C address for the SCD41 sensor
// #define SCD41_ADDR 0x62
#define MQTT_BROKER_IP "5.196.78.28"
#define MQTT_TOPIC_PUBLISH "pico/scd41/data"       // Topic for publishing sensor data
#define MQTT_TOPIC_SUBSCRIBE "pico/scd41/control"  // Topic for receiving control messages
#define MQTT_PORT 1883



// Wi-Fi initialization function
void wifi_init() {
    // Initialize Wi-Fi with the default settings
    if (cyw43_arch_init()) {
        printf("Failed to initialize Wi-Fi\n");
    } else {
        printf("Wi-Fi initialized.. you can surf the web now.. \n");
    }
}

// Wi-Fi stop function
void wifi_stop() {
    // Deinitialize Wi-Fi to save power
    cyw43_arch_deinit();
    printf("Wi-Fi has stopped!\n");
}

// I2C initialization function
void i2c_init_comm(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate) {
    i2c_init(i2c, baudrate);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    printf("I2C initialized on SDA: %d, SCL: %d, at baudrate: %d\n", sda_pin, scl_pin, baudrate);
}

// I2C deinitialization function
void i2c_deinit_comm(i2c_inst_t *i2c) {
    i2c_deinit(i2c); // Disables the I2C hardware instance
    printf("I2C deinitialized\n");
}

// UART initialization function
void uart_init_comm(uart_inst_t *uart, uint tx_pin, uint rx_pin, uint baudrate) {
    uart_init(uart, baudrate);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    printf("UART initialized on TX: %d, RX: %d, at baudrate: %d\n", tx_pin, rx_pin, baudrate);
}

// UART deinitialization function
void uart_deinit_comm(uart_inst_t *uart) {
    uart_deinit(uart); // Disables the UART hardware instance
    printf("UART deinitialized\n");
}


// Function to trim whitespace from a string
char* trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (*str == ' ') str++;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n' || *end == '\r')) end--;

    // Write new null terminator character
    *(end+1) = '\0';

    return str;
}

// ########################################################################
// setting mode function            
// ########################################################################
void set_mode(int mode) {
    if (mode == 1) {
        // Low Power Mode
        printf("Setting to Low Power Mode...\n");
        set_polling_interval(600); // Set polling interval to every 10 minutes
        // enter_low_power_state();
    } else if (mode == 2) {
        // Normal Mode
        printf("Setting to Normal Mode...\n");
        set_polling_interval(60); // Set polling interval to every 1 minute
        // enable_peripherals();
    } else if (mode == 3) {


        // High-Performance Mode
        printf("Setting to High-Performance Mode...\n");
        set_polling_interval(1); // Set polling interval to every second
        // activate_continuous_monitoring();
    }
}



