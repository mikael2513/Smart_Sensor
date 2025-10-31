#include "http_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

HttpResponse* http_post(const char *host, int port, const char *path, 
                       const char *json_data) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return NULL;
    }
    
    // Get server address
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        close(sockfd);
        return NULL;
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Copy the adress
    if (server->h_addr_list[0] != NULL) {
        memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    } else {
        fprintf(stderr, "Error: No address found for host\n");
        close(sockfd);
        return NULL;
    }
    
    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return NULL;
    }
    
    // Create HTTP request
    char request[2048];
    int content_length = strlen(json_data);
    
    snprintf(request, sizeof(request),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             path, host, content_length, json_data);
    
    // Send request
    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Send failed");
        close(sockfd);
        return NULL;
    }
    
    // Receive response
    char response_buffer[4096];
    HttpResponse *response = malloc(sizeof(HttpResponse));
    if (!response) {
        close(sockfd);
        return NULL;
    }
    
    response->body = NULL;
    response->body_length = 0;
    response->status_code = 0;
    
    ssize_t total_received = 0;
    ssize_t bytes_received;
    
    while ((bytes_received = recv(sockfd, response_buffer, sizeof(response_buffer) - 1, 0)) > 0) {
        response_buffer[bytes_received] = '\0';
        
        // Reallocate and append to response body
        char *new_body = realloc(response->body, total_received + bytes_received + 1);
        if (!new_body) {
            free(response->body);
            free(response);
            close(sockfd);
            return NULL;
        }
        
        response->body = new_body;
        memcpy(response->body + total_received, response_buffer, bytes_received);
        total_received += bytes_received;
        response->body[total_received] = '\0';
    }
    
    response->body_length = total_received;
    
    // Parse status code (simple parsing)
    if (response->body && total_received > 0) {
        sscanf(response->body, "HTTP/1.1 %d", &response->status_code);
    }
    
    close(sockfd);
    return response;
}

void free_http_response(HttpResponse *response) {
    if (response) {
        free(response->body);
        free(response);
    }
}