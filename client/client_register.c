#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Client configuration
#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int connect_to_server(void)
{
    struct sockaddr_in serv_addr;
    int sock = 0;
    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    // Set server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    return sock;
}

int main(int argc, char *argv[])
{
    char username[50];
    char groupname[50];

    // Check for correct number of arguments and length of username and groupname
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <username> <groupname>\n", argv[0]);
        exit(1);
    } else if (strlen(argv[1]) > 50) {
        fprintf(stderr, "Error: Username too long\n");
        exit(1);
    } else if (strlen(argv[2]) > 50) {
        fprintf(stderr, "Error: Groupname too long\n");
        exit(1);
    } else {
        strcpy(username, argv[1]);
        strcpy(groupname, argv[2]);
    }   

    int sock = 0;
    char buffer[BUFFER_SIZE] = {0};

    // Create a socket and connect to the server
    char full_command[BUFFER_SIZE];
    int ret = snprintf(full_command, sizeof(full_command), "register %s %s|}{|%s",
                        username, groupname, username);
    if (ret >= sizeof(full_command))
    {
        fprintf(stderr, "Error: command buffer overflow\n");
        exit(1);
    }
    // Connect to server
    sock = connect_to_server();
    // Send command to server
    send(sock, full_command, strlen(full_command), 0);
    // Read response from server
    memset(buffer, 0, BUFFER_SIZE);
    read(sock, buffer, BUFFER_SIZE);
    // Print response from server
    printf("[server] %s\n", buffer);
    close(sock);

    
    return 0;
}