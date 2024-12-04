#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "capability.h"
#include "command_handler.h"

#define BUFFER_SIZE 1024

void parse_data(char* buffer, char *command, char* username) {
    char *token = strtok(buffer, "|}{|");
    strcpy(command, token);
    token = strtok(NULL, "|}{|");
    strcpy(username, token);
}

int handle_command(int sock, Users* users, Files* files, char* command, char* username, char* response) {
    User *user = find_user_by_name(users, username);
    if (user == NULL) return -1; // User not found

    char operation[10], filename[256], parameter[50];

    char *token = strtok(command, " ");
    if (!strcmp(token, "create")) {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        strcpy(filename, token);
        token = strtok(NULL, " ");
        if (token == NULL) return -3; // Invalid format
        strcpy(parameter, token);
        return handle_create(filename, parameter, users, user, files, response);
    } else if (!strcmp(token, "read")) {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        strcpy(filename, token);
        return handle_read(sock, filename, users, user, files, response);
    } 
}

int handle_create(char* filename, char* parameter, Users* users, User* user, Files* files, char* response) {
    if (strlen(parameter) != 6) return -3; // Invalid format
    if (find_file_by_name(files, filename) != NULL) return -4; // File already exists

    Group *group = user->group;

    bool owner_read = parameter[0] == 'r';
    bool owner_write = parameter[1] == 'w';
    bool group_read = parameter[2] == 'r';
    bool group_write = parameter[3] == 'w';
    bool others_read = parameter[4] == 'r';
    bool others_write = parameter[5] == 'w';

    File *file = create_file(files, filename);
    if (owner_read || owner_write) 
        add_owner_capability(file, user, owner_read, owner_write);
    if (group_read || group_write) 
        add_group_capability(file, group, group_read, group_write);
    if (others_read || others_write)
        add_others_capability(file, users, user, others_read, others_write);
    strncpy(response, "File created successfully", strlen("File created successfully\n")+1);
    return 0;
}


int handle_read(int sock, char* filename, Users* users, User* user, Files* files, char* response) {
    File *file = find_file_by_name(files, filename);
    if (file == NULL) return -2; // File not found
    if (user_has_capability(user, file)) {
        
        char path[300];
        snprintf(path, sizeof(path), "files/%s", file->name);
        FILE *fp = fopen(path, "r");
        while (1) {
            int bytes_read = fread(response, 1, sizeof(response), fp);
            if (bytes_read == 0) break;
            send(sock, response, bytes_read, 0);
        }
        fclose(fp);
        close(sock);
        return -6;    // File read successfully
    }
        

    return -5; // User does not have permission to read file
}

