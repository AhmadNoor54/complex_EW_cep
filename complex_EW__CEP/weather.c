// weather.c
#include "weather_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <unistd.h>
#include "email_sender.h" 


int main() {
    const char *api_key = "ee5de1217e2254d61409fffa4abad982";
    char city[100];

    // Hardcode the city name to "karachi"
    strcpy(city, "karachi");
    //get_weather(api_key, city);
    //obtained API response
    const char *api_response = api_return_data;

    // Save API response to raw_data.txt
    save_raw_data(api_response, "raw_data.txt");

    
    //These lines of code set up the necessary variables for making HTTP requests
    CURL *curl;
    //stores the result of CURL operations
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a curl handle as well as This function returns a pointer to the newly initialized CURL handle
    curl = curl_easy_init();
    if (curl) {
        // Set the API endpoint and parameters as well as This array is used to store the URL (Uniform Resource Locator) for an HTTP request
        char url[256];
        sprintf(url, "https://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric", city, api_key);

        // Create a buffer to store the received data
        char data[4096] = {0};

        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the write callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

        // Perform the HTTP request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse and display the weather information
            json_t *root;
            json_error_t error;
            root = json_loads(data, 0, &error);

            if (!root) {
                fprintf(stderr, "JSON parsing error: %s\n", error.text);
            } else {
            
                double temperature = json_number_value(json_object_get(json_object_get(root, "main"), "temp"));
                

                // Additional variables for humidity, wind speed, and air pressure
                double *humidity_values, *wind_speed_values, *air_pressure_values;
                int count;

                // Calculate average humidity
                humidity_values = calculate_average_humidity(root, &count);
                if (humidity_values == NULL) {
                    // Handle memory allocation error
                    return 1;
                }
                double average_humidity = calculate_average_value(humidity_values, count);

                // Calculate average wind speed
                wind_speed_values = calculate_average_wind_speed(root, &count);
                if (wind_speed_values == NULL) {
                    // Handle memory allocation error
                    return 1;
                }
                double average_wind_speed = calculate_average_value(wind_speed_values, count);

                // Calculate average air pressure
                air_pressure_values = calculate_average_air_pressure(root, &count);
                if (air_pressure_values == NULL) {
                    // Handle memory allocation error
                    return 1;
                }
                double average_air_pressure = calculate_average_value(air_pressure_values, count);

                // Generate report
                generate_process_data(temperature, average_humidity, average_wind_speed, average_air_pressure);
                // Display real-time alert
                display_alert(THRESHOLD);

                // Execute the shell script
                execute_shell_script();
                // Free allocated memory for humidity, wind speed, and air pressure
                free(humidity_values);
                free(wind_speed_values);
                free(air_pressure_values);

                // Cleanup JSON object
                json_decref(root);
            }
        }

        // Cleanup curl handle
        curl_easy_cleanup(curl);
    }

    // Cleanup global state
    curl_global_cleanup();

    return 0;
}
