#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stdlib.h>

typedef void (*DataHandler)(const char* data, size_t length);

char* get_current_timestamp();

float generate_temperature();

char* generate_uuid();

void handle_response_data(const char* data, size_t length, DataHandler handler);

void save_sensor_data_locally(const char* device_id, const char* timestamp, float temperature);

void print_local_sensor_data();

#endif