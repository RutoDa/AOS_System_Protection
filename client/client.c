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

bool has_error(char *buffer)
{
    return (strncmp(buffer, "Error: User not found", 21) == 0 ||
            strncmp(buffer, "Error: File not found", 21) == 0 ||
            strncmp(buffer, "Error: Invalid format", 21) == 0 ||
            strncmp(buffer, "Error: File already exists", 26) == 0 ||
            strncmp(buffer, "Error: User does not have permission", 37) == 0 ||
            strncmp(buffer, "Error: User is not the owner of the file", 42) == 0 ||
            strncmp(buffer, "Error: File is being read or written", 37) == 0 || 
            strncmp(buffer, "Error: File is being written", 30) == 0);
}

int main(void)
{
    int sock = 0;

    char buffer[BUFFER_SIZE] = {0};
    char username[50];

    // Prompt user to enter username
    printf("Welcome to the file system client\n");
    printf("Enter username: ");
    // Read username from stdin
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Connected to server. Enter commands:\n");
    printf("-----------------------Supported commands-----------------------\n");
    printf("|1. create <filename> <permissions> - Create a file            |\n");
    printf("|2. read <filename> - Read a file                              |\n");
    printf("|3. write <filename> o/a - Write to a file (overwrite/append)  |\n");
    printf("|4. mode <filename> <new_permissions> - Change file permissions|\n");
    printf("|5. exit - Exit the client                                     |\n");
    printf("----------------------------------------------------------------\n");

    while (1)
    {
        // Read command from stdin
        char command[BUFFER_SIZE];
        printf("> ");
        fflush(stdout);
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = '\0';

        char full_command[BUFFER_SIZE];
        int ret = snprintf(full_command, sizeof(full_command), "%s|}{|%s",
                           command, username);
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

        char *token = strtok(command, " ");
        
        // Handle server response
        if (!strcmp(token, "read") && !has_error(buffer))
        {
            // Download file from server
            char filename[256];
            token = strtok(NULL, " ");
            strcpy(filename, token);
            printf("[Server] File (%s) download started...\n", filename);
            FILE *fp = fopen(filename, "w");
            fwrite(buffer, 1, strlen(buffer), fp);
            while (1)
            {
                int bytes_read = recv(sock, buffer, BUFFER_SIZE, 0);
                if (bytes_read < 0)
                {
                    perror("recv");
                    break;
                }
                if (bytes_read == 0)
                    break;

                fwrite(buffer, 1, bytes_read, fp);
            }
            printf("[Server] File (%s) download completed!\n", filename);
            fflush(stdout);
            fclose(fp);
        }
        else if (!strcmp(token, "write") && !has_error(buffer))
        {
            // Upload file to server
            char filename[256];
            token = strtok(NULL, " ");
            strcpy(filename, token);
            printf("[Server] File (%s) upload started...\n", filename);
            FILE *fp = fopen(filename, "r");
            if (fp == NULL)
            {
                perror("fopen");
                close(sock);
                continue;
            }
            while (1) {
                int bytes_read = fread(buffer, 1, sizeof(buffer), fp);
                if (bytes_read == 0) break;
                send(sock, buffer, bytes_read, 0);
            }
            fclose(fp);
            printf("[Server] File (%s) upload completed!\n", filename);
        } else if (!strcmp(token, "exit"))
        {
            // Exit client
            close(sock);
            break;
        } else
        {
            // Print server response
            printf("[Server] %s\n", buffer);
            fflush(stdout);
        }
        close(sock);
    }

    
    return 0;
}