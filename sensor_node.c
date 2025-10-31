#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "http_client.h"
#include "utils.h"

// Different handlers for response data using function pointers
void print_handler(const char* data, size_t length) {
    printf("=== Response Data (Length: %zu) ===\n", length);
    printf("%.*s\n", (int)length, data);
    printf("=== End of Response ===\n\n");
}

void save_to_file_handler(const char* data, size_t length) {
    FILE *file = fopen("sensor_log.txt", "a");
    if (file) {
        char *timestamp = get_current_timestamp();
        fprintf(file, "[%s] Response: %.*s\n", timestamp, (int)length, data);
        fclose(file);
        free(timestamp);
        printf("Response saved to sensor_log.txt\n");
    }
}

void minimal_handler(const char* data, size_t length) {
    (void)data; 
    printf("Server responded with %zu bytes of data\n", length);
}

int main(int argc, char **argv) {
    printf("=== Smart Sensor Node Starting ===\n");
    
    printf("Program started with %d arguments:\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }
    
    srand(time(NULL));
    
    // Generate sensor data
    char *uuid = generate_uuid();
    char *timestamp = get_current_timestamp();
    float temperature = generate_temperature();
    
    printf("Sensor Data:\n");
    printf("  Device: %s\n", uuid);
    printf("  Time: %s\n", timestamp);
    printf("  Temperature: %.2f°C\n", temperature);
    
    save_sensor_data_locally(uuid, timestamp, temperature);
    
    // Create JSON payload 
    char json_payload[512];
    snprintf(json_payload, sizeof(json_payload),
             "{\n"
             "    \"device\": \"%s\",\n"
             "    \"time\": \"%s\",\n"
             "    \"temperature\": \"%.2f°C\"\n"
             "}", uuid, timestamp, temperature);
    
    printf("\nJSON Payload:\n%s\n", json_payload);
    
    // Select handler based on command line argument or default
    DataHandler selected_handler = print_handler;
    if (argc > 1) {
        if (strcmp(argv[1], "save") == 0) {
            selected_handler = save_to_file_handler;
        } else if (strcmp(argv[1], "minimal") == 0) {
            selected_handler = minimal_handler;
        } else if (strcmp(argv[1], "local") == 0) {
            // New mode: show local data only
            print_local_sensor_data();
            // Clean up and exit
            free(uuid);
            free(timestamp);
            return 0;
        }
    }
    
    printf("\n=== Sending HTTP POST Request ===\n");
    
    // Send HTTP POST request to jsonplaceholder.typicode.com
    HttpResponse *response = http_post("jsonplaceholder.typicode.com", 80, "/posts", json_payload);
    
    if (response) {
        printf("HTTP Status: %d\n", response->status_code);
        
        // Use function pointer to handle response data
        handle_response_data(response->body, response->body_length, selected_handler);
        
        free_http_response(response);
    } else {
        printf("Failed to get response from server\n");
    }
    
    print_local_sensor_data();
    
    free(uuid);
    free(timestamp);
    
    printf("=== Smart Sensor Node Finished ===\n");
    return 0;
}