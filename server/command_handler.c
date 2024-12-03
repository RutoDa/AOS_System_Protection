#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "capability.h"
#include "command_handler.h"

void* parse_data(char* buffer, char *command, char* username) {
    char *token = strtok(buffer, "|}{|");
    strcpy(command, token);
    token = strtok(NULL, "|}{|");
    strcpy(username, token);
}

void* handle_command(char* command, char* username) {
    char operation[10], filename[256], parameter[50];

    char *token = strtok(command, " ");
    if (!strcmp(token, "create")) {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        strcpy(filename, token);
        token = strtok(NULL, " ");
        strcpy(parameter, token);
    }
}

