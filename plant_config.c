#include "plant_config.h"
#include <stdio.h>

PlantConfig plantConfigs[] = {
    //   Plant Name   | Max Temp | Min Temp | Optimal CO2 | Min Moisture | Max Moisture | Optimal Light | Max Velocity | Fan | Water Pump
    { "Lettuce",         24,         7,           400,          50,            70,            5000,           0.5,         1,    1 }, // Lettuce config
    { "Tomato",          30,         18,          450,          55,            75,            6000,           0.7,         1,    1 }, // Tomato config
    { "Basil",           28,         15,          400,          50,            65,            5500,           0.6,         1,    1 }  // Basil config
};


const int numPlants = sizeof(plantConfigs) / sizeof(plantConfigs[0]);

void displayPlantConfig(PlantConfig *config) {
    printf("Plant Name: %s\n", config->plant_name);
    printf("Max Temperature: %d\n", config->max_temperature);
    printf("Min Temperature: %d\n", config->min_temperature);
    printf("Optimal CO2: %d\n", config->optimal_co2);
    printf("Min Moisture: %d\n", config->min_moisture);
    printf("Max Moisture: %d\n", config->max_moisture);
    printf("Optimal Light: %d\n", config->optimal_light);
    printf("Max Velocity: %.2f\n", config->max_velocity);
    printf("Fan Activation: %d\n", config->fan_activation);
    printf("Water Pump Control: %d\n", config->water_pump_control);
}
