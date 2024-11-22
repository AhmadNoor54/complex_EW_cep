// weather_utils.c
#include "weather_utils.h"
#include "email_sender.h"
#include <string.h>
#include <curl/curl.h> 
#include <stdio.h>
#include <curl/curl.h>
#include <time.h>
// A function recieving the data from an HTTP response 
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    char *data = (char *)userp;
    strncat(data, (char *)contents, realsize);
    return realsize;
}
// Function to read data from the file for CURLOPT_READFUNCTION
size_t read_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fread(ptr, size, nmemb, stream);
}

// Function to send email with attachment
int send_email_with_attachment(const char *to, const char *cc, const char *file_path) {
    CURL *curl;
    CURLcode res = CURLE_OK;

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", file_path);
        return 1;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist *recipients = NULL;

        // Specify email server details
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com");
        curl_easy_setopt(curl, CURLOPT_USERPWD, "tariq347146@gmail.com:ckjmheoleadxljcx");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // Add email recipients
        if (to)
            recipients = curl_slist_append(recipients, to);
        if (cc)
            recipients = curl_slist_append(recipients, cc);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Set the read callback function to read the file content
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, file);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Perform the email sending
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        // Clean up
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    // Close the file
    fclose(file);

    curl_global_cleanup();
    return (int)res;
}
void save_raw_data(const char *api_response, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, "%s\n", api_response);
        fclose(file);
        //printf("API response saved to %s\n", filename);
    } else {
        fprintf(stderr, "Error opening file for raw data: %s\n", filename);
    }
}




double *calculate_average_temperature(json_t *root, int *count) {
    double sum = 0.0;

    json_t *temperature_list = json_object_get(json_object_get(root, "main"), "temp");
    size_t index;
    json_t *value;

    *count = json_array_size(temperature_list);
    double *temperatures = malloc(*count * sizeof(double));

    if (temperatures == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    json_array_foreach(temperature_list, index, value) {
        temperatures[index] = json_number_value(value);
        sum += temperatures[index];
    }

    return temperatures;
}




// ...

double *calculate_average_humidity(json_t *root, int *count) {
    json_t *humidity = json_object_get(json_object_get(root, "main"), "humidity");
    size_t index;

    *count = 1;  // Humidity is a single value, not an array
    double *humidities = malloc(*count * sizeof(double));

    if (humidities == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    humidities[0] = json_number_value(humidity);

    return humidities;
}

double *calculate_average_wind_speed(json_t *root, int *count) {
    json_t *wind_speed = json_object_get(json_object_get(root, "wind"), "speed");
    size_t index;

    *count = 1;  // Wind speed is a single value, not an array
    double *wind_speeds = malloc(*count * sizeof(double));

    if (wind_speeds == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    wind_speeds[0] = json_number_value(wind_speed);

    return wind_speeds;
}

double *calculate_average_air_pressure(json_t *root, int *count) {
    json_t *air_pressure = json_object_get(json_object_get(root, "main"), "pressure");
    size_t index;

    *count = 1;  // Air pressure is a single value, not an array
    double *air_pressures = malloc(*count * sizeof(double));

    if (air_pressures == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    air_pressures[0] = json_number_value(air_pressure);

    return air_pressures;
}

// ...


double calculate_average_value(double *values, int count) {
    double sum = 0.0;

    if (count == 1) {
        return values[0];
    }

    for (int i = 0; i < count; ++i) {
        sum += values[i];
    }

    return (count > 0) ? (sum / count) : 0.0;
}
void save_temperature(double average_temperature) {
    FILE *temperature_file = fopen("Temperature.txt", "a");

    if (temperature_file != NULL) {
        fprintf(temperature_file, "%.2f\n", average_temperature);
        fclose(temperature_file);
        //printf("Temperature data saved to Temperature.txt\n");
    } else {
        fprintf(stderr, "Error opening file for temperature data: Temperature.txt\n");
    }
}

// report.c




void generate_report(double average_temperature) {
    FILE *file = fopen("report.txt", "w");  // Open for appending

    if (file != NULL) {
        // Get the current time
        time_t t;
        struct tm *tm_info;

        time(&t);
        tm_info = localtime(&t);

        // Format the current time
        char time_str[26];
        strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        // Calculate an alert threshold (example value, replace with your logic)
        double alert_threshold = 30.0;

        // Customize messages based on environmental conditions
        const char *alert_message;
        if (average_temperature < alert_threshold) {
            alert_message = "ALERT: Average temperature low then the  threshold value !";
        } else {
            alert_message = "No significant alert.";
        }

        // Write the report to report.txt
        fprintf(file, "Environmental Data Report\n");
        fprintf(file, "-------------------------\n");
        fprintf(file, "Report Time: %s\n", time_str);
        fprintf(file, "Average Temperature: %.2f°C\n", average_temperature);
        fprintf(file, "Alert Message: %s\n", alert_message);
        fprintf(file, "\n");  // Add a newline for separation

        fclose(file);
    } else {
        fprintf(stderr, "Error opening file for report: report.txt\n");
    }
}


void generate_process_data(double average_temperature, double average_humidity, double average_wind_speed, double average_air_pressure) {
    

    save_temperature(average_temperature);  
    FILE *file = fopen("process_data.txt", "w");
    FILE *process_data_file = fopen("process_data.txt", "a");  // Open for appending

    if (file != NULL && process_data_file != NULL) {
    


        // Store process_data in process_data.txt
        fprintf(process_data_file, "Environmental Data Report\n");
        fprintf(process_data_file, "-------------------------\n");
        fprintf(process_data_file, "Average Temperature: %.2f°C\n", average_temperature);
        fprintf(process_data_file, "Average Humidity: %.2f%%\n", average_humidity);
        fprintf(process_data_file, "Average Wind Speed: %.2f m/s\n", average_wind_speed);
        fprintf(process_data_file, "Average Air Pressure: %.2f hPa\n", average_air_pressure);
        
        fclose(process_data_file);

    } else {
        fprintf(stderr, "Error opening file for report: report.txt or raw_data.txt\n");
    }
}
void execute_shell_script() {
    system("./monitoring_script.sh");
}
char *api_return_data = NULL;

void display_alert(double threshold) {
    // Read the average temperature from the "Temperature.txt" file
    FILE *file = fopen("Temperature.txt", "r");
    if (file != NULL) {
        double average_temperature;

        // Rewind the file pointer to the beginning
        fseek(file, 0, SEEK_SET);

        if (fscanf(file, "%lf", &average_temperature) == 1) {
            fclose(file);

            //fprintf(stdout, "Average Temperature: %.2f°C\n", average_temperature);

            // Check if the average temperature exceeds the threshold
            if (average_temperature < threshold) {
                //fprintf(stderr, "ALERT: Average temperature exceeds threshold (%.2f°C)!\n", threshold);
                generate_report(average_temperature);

                // Add code for sending an email
                send_email_with_attachment("naqvishehroze@gmail.com", "naqvishehroze2gmail.com", "report.txt");

                // Add any additional alert actions here

                // Example: Display an alert message
                //printf("Alert: High temperature detected!\n");
            }
        } else {
            fprintf(stderr, "Error reading average temperature from file\n");
            fclose(file);
        }
    } else {
        fprintf(stderr, "Error opening file for temperature data: Temperature.txt\n");
    }
}
