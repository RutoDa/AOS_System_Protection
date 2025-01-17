#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

int parse_data(char* buffer, char *command, char* username);

int register_user(Users *users, Groups *groups, char* command, char* response);

int handle_command(int sock, Users* users, Files* files, char* command, char* username, char* response);

int handle_create(char* filename, char* parameter, Users* users, User* user, Files* files, char* response);

int handle_read(int sock, char* filename, Users* users, User* user, Files* files, char* response);

int handle_write(int sock, char* filename, char* parameter, Users* users, User* user, Files* files, char* response);

int handle_mode(char* filename, char* parameter, Users* users, User* user, Files* files, char* response);

#endif