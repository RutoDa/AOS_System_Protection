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
#include "init.h"

// Server configuration
#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Global variables
Groups *groups;
Users *users;

void* handle_client(void* client_socket) {
    int sock = *(int*)client_socket;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        
        if (bytes_received <= 0) {
            break;
        }

        char command[50], filename[100], param[50], username[50];
        sscanf(buffer, "%s %s %s %s", command, filename, param, username);

        if (strcmp(command, "create") == 0) {
            int result = create_file(filename, username, param);
            sprintf(response, result == 0 ? "File created successfully" : "File creation failed");
        }
        // TODO: 實現 read, write, mode 等其他命令

        send(sock, response, strlen(response), 0);
    }

    close(sock);
}


int main(void) {
    // Initialize groups and users
    Groups *groups = init_groups();
    Users *users = init_users();
    init_system(groups, users);

    
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

        
        if (pthread_create(&threads[thread_count], NULL, handle_client, (void*)&client_socket) < 0) {
            perror("could not create thread");
            continue;
        }

        thread_count = (thread_count + 1) % MAX_CLIENTS;
    }


    free_system(groups, users);
    return 0;
}