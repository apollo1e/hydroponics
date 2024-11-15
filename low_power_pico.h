// low_power_pico.h

#ifndef LOW_POWER_PICO_H
#define LOW_POWER_PICO_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "pico/cyw43_arch.h"


// Wi-Fi functions
void wifi_init();
void wifi_stop();

// I2C functions
void i2c_init_comm(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate);
void i2c_deinit_comm(i2c_inst_t *i2c);

// UART functions
void uart_init_comm(uart_inst_t *uart, uint tx_pin, uint rx_pin, uint baudrate);
void uart_deinit_comm(uart_inst_t *uart);

// Utility functions for low-power operation
// void set_low_power_mode();   // cnnt use this cus directly affect CPU clocl rate 
void set_polling_interval(int interval);

// void set_mode() ;

#endif
