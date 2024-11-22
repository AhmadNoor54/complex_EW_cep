// weather_utils.h

#ifndef WEATHER_UTILS_H
#define WEATHER_UTILS_H

#include <jansson.h>

#include "email_sender.h" 
// Define THRESHOLD
#define THRESHOLD 10.0  // You can adjust this value based on your requirements


extern char *api_return_data;

// Declare the write_callback function
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
void save_raw_data(const char *api_response, const char *filename);
double *calculate_average_temperature(json_t *root, int *count);
void generate_process_data(double average_temperature, double average_humidity, double average_wind_speed, double average_air_pressure);
void execute_shell_script();
void display_alert(double threshold);
//void get_weather(const char *api_key, const char *city);

// Additional functions (update weather_utils.c accordingly)
double *calculate_average_humidity(json_t *root, int *count);
double *calculate_average_wind_speed(json_t *root, int *count);
double *calculate_average_air_pressure(json_t *root, int *count);
double calculate_average_value(double *values, int count);

#endif  // WEATHER_UTILS_H
