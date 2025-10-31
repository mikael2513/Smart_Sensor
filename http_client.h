#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "utils.h"

typedef struct {
    int status_code;
    char *body;
    size_t body_length;
} HttpResponse;

HttpResponse* http_post(const char *host, int port, const char *path, 
                       const char *json_data);

void free_http_response(HttpResponse *response);

#endif