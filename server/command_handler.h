#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

void* parse_data(char* buffer, char *command, char* username);

void* handle_command(char* command, char* username);

#endif