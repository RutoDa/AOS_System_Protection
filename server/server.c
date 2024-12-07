#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "capability.h"
#include "command_handler.h"
#include "init.h"

// Thread data
typedef struct {
    int client_socket;
    Users *users;
    Groups *groups;
    Files *files;
} thread_data_t;

// Server configuration
#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void* handle_client(void* arg) {
    thread_data_t *data = (thread_data_t*)arg;
    int sock = data->client_socket;
    Users *users = data->users;
    Groups *groups = data->groups;
    Files *files = data->files;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;

        char command[50], username[50];
        int status = parse_data(buffer, command, username);  
        printf("Recevied: {User: %s, Command: %s}\n", username, command);

        // Register user
        if (status == 1) {
            memset(response, 0, BUFFER_SIZE);
            int result = register_user(users, groups, command, response);
            if (result == -3) strcpy(response, "Error: Invalid format");
            else if (result == -4) strcpy(response, "Error: User already exists");
            send(sock, response, strlen(response), 0);
            continue;
        }
        
        // Handle command
        memset(response, 0, BUFFER_SIZE);
        int result = handle_command(sock, users, files, command, username, response);
        
        if (result == -1) strcpy(response, "Error: User not found");
        else if (result == -2) strcpy(response, "Error: File not found");
        else if (result == -3) strcpy(response, "Error: Invalid format");
        else if (result == -4) strcpy(response, "Error: File already exists");
        else if (result == -5) strcpy(response, "Error: User does not have permission");
        else if (result == -6 || result == -7)  return NULL; // File read or write successfully
        else if (result == -8) strcpy(response, "Error: User is not the owner of the file");
        else if (result == -9) break; // Exit
        else if (result == -10) strcpy(response, "Error: File is being read or written");
        else if (result == -11) strcpy(response, "Error: File is being written");
        
        send(sock, response, strlen(response), 0);
    }
    
    free(data);
    close(sock);
}


int main(void) {
    // Initialize groups and users
    Groups *groups = init_groups();
    Users *users = init_users();
    Files *files = init_files();
    init_system(groups, users, files);
    

    
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t threads[MAX_CLIENTS];
    int thread_count = 0;
    
    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

        
    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        thread_data_t *data = malloc(sizeof(thread_data_t));
        data->client_socket = client_socket;
        data->users = users;
        data->groups = groups;
        data->files = files;
        
        if (pthread_create(&threads[thread_count], NULL, handle_client, (void*)data) < 0) {
            perror("could not create thread");
            continue;
        }

        thread_count = (thread_count + 1) % MAX_CLIENTS;
    }


    free_system(groups, users, files);
    return 0;
}