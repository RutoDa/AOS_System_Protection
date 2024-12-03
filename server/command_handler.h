#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

void parse_data(char* buffer, char *command, char* username);

int handle_command(Users* users, Files* files, char* command, char* username, char* response);

int handle_create(char* filename, char* parameter, Users* users, User* user, Files* files, char* response);

#endif