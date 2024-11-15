#ifndef PLANT_CONFIG_H
#define PLANT_CONFIG_H

typedef struct {
    char* plant_name;
    int max_temperature;    // °C
    int min_temperature;    // °C
    int optimal_co2;        // ppm
    int min_moisture;       // %
    int max_moisture;       // %
    int optimal_light;      // lux
    float max_velocity;     // m/s
    int fan_activation;     // 0 = false, 1 = true
    int water_pump_control; // 0 = false, 1 = true
} PlantConfig;

// Declaration of the plant configuration array
extern PlantConfig plantConfigs[];

// Declaration of the display function
void displayPlantConfig(PlantConfig *config);

#endif // PLANT_CONFIG_H
