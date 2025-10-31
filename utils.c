#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define LOCAL_DATA_FILE "sensor_data.csv"
#define MAX_ENTRIES 1000

typedef struct {
    char device_id[64];
    char timestamp[32];
    float temperature;
} SensorData;

static SensorData local_data[MAX_ENTRIES];
static int data_count = 0;

char* get_current_timestamp() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    char *timestamp = malloc(20 * sizeof(char));
    if (timestamp) {
        strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", t);
    }
    return timestamp;
}

float generate_temperature() {
    return 15.0 + ((float)rand() / RAND_MAX) * 15.0;
}

char* generate_uuid() {
    char *uuid = malloc(64 * sizeof(char));
    if (uuid) {
        snprintf(uuid, 64, "sensor-%08lx-%04x-%04x-%04x-%08lx",
                 (unsigned long)rand() << 16 | rand(),
                 rand() & 0xffff, rand() & 0xffff,
                 rand() & 0xffff,
                 (unsigned long)rand() << 16 | rand());
    }
    return uuid;
}

void handle_response_data(const char* data, size_t length, DataHandler handler) {
    if (handler && data) {
        handler(data, length);
    }
}

void save_sensor_data_locally(const char* device_id, const char* timestamp, float temperature) {
    if (data_count < MAX_ENTRIES) {
        strncpy(local_data[data_count].device_id, device_id, sizeof(local_data[data_count].device_id) - 1);
        strncpy(local_data[data_count].timestamp, timestamp, sizeof(local_data[data_count].timestamp) - 1);
        local_data[data_count].temperature = temperature;
        data_count++;
    }
    
    // Save to CSV file
    FILE *file = fopen(LOCAL_DATA_FILE, "a");
    if (file) {
        if (ftell(file) == 0) {
            // Write header if file is empty
            fprintf(file, "device_id,timestamp,temperature_c\n");
        }
        fprintf(file, "\"%s\",\"%s\",%.2f\n", device_id, timestamp, temperature);
        fclose(file);
        printf("Data saved locally to %s\n", LOCAL_DATA_FILE);
    }
}

void print_local_sensor_data() {
    printf("\n=== Local Sensor Data Storage ===\n");
    printf("Total entries: %d\n", data_count);
    printf("Latest entries:\n");
    
    int start = (data_count > 5) ? data_count - 5 : 0;
    for (int i = start; i < data_count; i++) {
        printf("  [%d] %s | %s | %.2f°C\n", 
               i + 1, 
               local_data[i].device_id, 
               local_data[i].timestamp, 
               local_data[i].temperature);
    }
    
    FILE *file = fopen(LOCAL_DATA_FILE, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fclose(file);
        printf("File: %s (%.1f KB)\n", LOCAL_DATA_FILE, size / 1024.0);
    }
}