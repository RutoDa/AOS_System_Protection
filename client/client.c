#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Client configuration
#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

// Test commands
#define TEST_COMMAND1 "create homework2.c rwr---"
#define TEST_COMMAND2 "read homework2.c"
#define TEST_COMMAND3 "write homework2.c o"
#define TEST_COMMAND4 "write homework2.c a"
#define TEST_COMMAND5 "write homework2.c rw----"


int main(void) {
    int sock = 0, c;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char username[50];

    

    // Prompt user to enter username
    printf("Welcome to the file system client\n");
    printf("Enter username: ");
    
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = '\0'; 


    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    // Set server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    
    printf("Connected to server. Enter commands:\n");
    printf("-----------------------Supported commands-----------------------\n");
    printf("|1. create <filename> <permissions> - Create a file            |\n");
    printf("|2. read <filename> - Read a file                              |\n");
    printf("|3. write <filename> o/a - Write to a file (overwrite/append)  |\n");
    printf("|4. mode <filename> <new_permissions> - Change file permissions|\n");
    printf("----------------------------------------------------------------\n");

    while(1) {
        char command[BUFFER_SIZE];
        printf("> ");
        fflush(stdout);
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = '\0'; 

        
        char full_command[BUFFER_SIZE];
        snprintf(full_command, sizeof(full_command), "%s|}{|%s", 
                 command, username);

        
        send(sock, full_command, strlen(full_command), 0);

        //break;
        //memset(buffer, 0, BUFFER_SIZE);
        //read(sock, buffer, BUFFER_SIZE);
        //printf("Server response: %s\n", buffer);
    }

    close(sock);
    return 0;
}