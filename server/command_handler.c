#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <pthread.h>
#include "capability.h"
#include "command_handler.h"

#define BUFFER_SIZE 1024

/**
 * Parse the data received from the client
 * @param buffer The data received from the client
 * @param command The command received from the client
 * @param username The username received from the client
 * @return 1 if the command is a register command, 0 otherwise
 */
int parse_data(char* buffer, char *command, char* username) {
    char *token = strtok(buffer, "|}{|");
    strcpy(command, token);
    token = strtok(NULL, "|}{|");
    strcpy(username, token);

    if (strncmp(command, "register", 8) == 0) return 1; // Register command
    return 0; // Other command
}

/**
 * Register a new user
 * @param users The list of users
 * @param groups The list of groups
 * @param command The command received from the client
 * @param response The response to send to the client
 * @return 0 if the user is registered successfully, -3 if the format is invalid, -4 if the user already exists
 */
int register_user(Users *users, Groups *groups, char* command, char* response) {
    char username[50];
    char groupname[50];

    // Parse the command
    char *token = strtok(command, " ");
    if (token == NULL) return -3; // Invalid format
    token = strtok(NULL, " ");
    if (token == NULL) return -3; // Invalid format
    strcpy(username, token);
    token = strtok(NULL, " ");
    if (token == NULL) return -3; // Invalid format
    strcpy(groupname, token);
    
    // Check if the user already exists
    if (find_user_by_name(users, username) != NULL) return -4; // User already exists
    // Check if the group exists
    if (find_group_by_name(groups, groupname) == NULL) {
        // Create the group if it does not exist
        Group *new_group = create_group(groups, groupname);
    }
    // Create the user
    User *new_user = create_user(users, groups, username, groupname);
    strncpy(response, "User registered successfully", strlen("User registered successfully\n")+1);
    return 0;
}

/**
 * Handle a command received from the client
 * @param sock The socket to send data to the client
 * @param users The list of users
 * @param files The list of files
 * @param command The command received from the client
 * @param username The username of the client
 * @param response The response to send to the client
 * @return 0 if the command is successful, -1 if the user is not found, -2 if the file is not found, -3 if the format is invalid, -4 if the file already exists, -5 if the user does not have permission, -6 if the file is read successfully, -7 if the file is written successfully, -8 if the user is not the owner of the file, -9 if the client exits, -10 if the file is being read or written, -11 if the file is being written
 */
int handle_command(int sock, Users* users, Files* files, char* command, char* username, char* response) {
    User *user = find_user_by_name(users, username);
    if (user == NULL) return -1; // User not found

    char operation[10], filename[256], parameter[50];

    char *token = strtok(command, " ");
    if (token == NULL) return -3; // Invalid format
    if (!strcmp(token, "create")) {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        if (token == NULL) return -3; // Invalid format
        strcpy(filename, token);
        token = strtok(NULL, " ");
        if (token == NULL) return -3; // Invalid format
        strcpy(parameter, token);
        return handle_create(filename, parameter, users, user, files, response);
    } else if (!strcmp(token, "read")) {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        if (token == NULL) return -3; // Invalid format
        strcpy(filename, token);
        return handle_read(sock, filename, users, user, files, response);
    } else if (!strcmp(token, "write")) {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        if (token == NULL) return -3; // Invalid format
        strcpy(filename, token);
        token = strtok(NULL, " ");
        if (token == NULL) return -3; // Invalid format
        strcpy(parameter, token);
        return handle_write(sock, filename, parameter, users, user, files, response);
    } else if (!strcmp(token, "mode")) {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        if (token == NULL) return -3; // Invalid format
        strcpy(filename, token);
        token = strtok(NULL, " ");
        if (token == NULL) return -3; // Invalid format
        strcpy(parameter, token);
        return handle_mode(filename, parameter, users, user, files, response);
    } else if (!strcmp(token, "exit")) {
        close(sock);
        return -9; // Exit
    } else return -3; // Invalid format
}

/**
 * Create a new file
 * @param filename The name of the file
 * @param parameter The permissions for the file
 * @param users The list of users
 * @param user The user creating the file
 * @param files The list of files
 * @param response The response to send to the client
 * @return 0 if the file is created successfully, -3 if the format is invalid, -4 if the file already exists
 */
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

    File *file = create_file(files, filename, user);
    if (owner_read || owner_write) 
        add_owner_capability(file, user, owner_read, owner_write);
    if (group_read || group_write) 
        add_group_capability(file, group, group_read, group_write);
    if (others_read || others_write)
        add_others_capability(file, users, user, others_read, others_write);
    strncpy(response, "File created successfully", strlen("File created successfully\n")+1);
    return 0;
}

/**
 * User reads(download) a file: send the file content to the client
 * @param sock The socket to send data to the client
 * @param filename The name of the file
 * @param users The list of users
 * @param user The user reading the file
 * @param files The list of files
 * @param response The response to send to the client
 * @return 0 if the file is read successfully, -2 if the file is not found, -5 if the user does not have permission, -6 if the file is read successfully, -10 if the file is being written, -11 if the file is being read
 */
int handle_read(int sock, char* filename, Users* users, User* user, Files* files, char* response) {
    File *file = find_file_by_name(files, filename);
    if (file == NULL) return -2; // File not found
    if (user_has_capability(user, file, "read")) {
        if (pthread_rwlock_tryrdlock(&file->rwlock) != 0) return -11; // File is being written

        char path[300];
        snprintf(path, sizeof(path), "files/%s", file->name);
        FILE *fp = fopen(path, "r");
        while (1) {
            int bytes_read = fread(response, 1, sizeof(response), fp);
            if (bytes_read == 0) break;
            send(sock, response, bytes_read, 0);
        }
        fclose(fp);

        // Unlock the file
        pthread_rwlock_unlock(&file->rwlock);
        close(sock);
        return -6;    // File read successfully
    }
    return -5; // User does not have permission to read file
}

/**
 * User writes(upload) to a file: receive the file content from the client
 * @param sock The socket to receive data from the client
 * @param filename The name of the file
 * @param parameter The parameter for the write operation
 * @param users The list of users
 * @param user The user writing to the file
 * @param files The list of files
 * @param response The response to send to the client
 * @return 0 if the file is written successfully, -2 if the file is not found, -3 if the format is invalid, -5 if the user does not have permission, -7 if the file is written successfully, -10 if the file is being read or written
 */
int handle_write(int sock, char* filename, char* parameter, Users* users, User* user, Files* files, char* response) {
    if (strncmp(parameter, "o", 1) != 0 && strncmp(parameter, "a", 1) != 0) return -3; // Invalid format
    File *file = find_file_by_name(files, filename);
    if (file == NULL) return -2; // File not found

    if (user_has_capability(user, file, "write")) {
        // Check if the file is being read or written
        if (pthread_rwlock_trywrlock(&file->rwlock) != 0) return -10; // File is being read or written

        strncpy(response, "Uploading file...", strlen("Uploading file...")+1);
        send(sock, response, strlen(response), 0);

        char path[300];
        snprintf(path, sizeof(path), "files/%s", file->name);
        FILE *fp = fopen(path, strncmp(parameter, "o", 1) == 0 ? "w" : "a");
        char buffer[BUFFER_SIZE];


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
        
        // Unlock the file
        pthread_rwlock_unlock(&file->rwlock);

        fclose(fp);
        return -7; // File write successfully
    }
    return -5; // User does not have permission to write file
}

/**
 * Change the permissions of a file
 * @param filename The name of the file
 * @param parameter The new permissions for the file
 * @param users The list of users
 * @param user The user changing the permissions
 * @param files The list of files
 * @param response The response to send to the client
 * @return 0 if the permissions are changed successfully, -2 if the file is not found, -3 if the format is invalid, -8 if the user is not the owner of the file
 */
int handle_mode(char* filename, char* parameter, Users* users, User* user, Files* files, char* response) {
    if (strlen(parameter) != 6) return -3; // Invalid format
    File *file = find_file_by_name(files, filename);
    if (file == NULL) return -2; // File not found
    if (!is_owner(user, file)) return -8; // User is not the owner of the file

    Group *group = user->group;

    bool owner_read = parameter[0] == 'r';
    bool owner_write = parameter[1] == 'w';
    bool group_read = parameter[2] == 'r';
    bool group_write = parameter[3] == 'w';
    bool others_read = parameter[4] == 'r';
    bool others_write = parameter[5] == 'w';

    modify_capability(file, users, user, owner_read, owner_write, group_read, group_write, others_read, others_write);
    strncpy(response, "File permissions changed successfully", strlen("File permissions changed successfully\n")+1);
    
    return 0;
}