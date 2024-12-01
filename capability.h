/*
    1. This file contains the declaration of the capability list data structure.
    2. The capability list's structure is as follows:
        Users (linked list)
            - username
            - groups (linked list)
                - group_name
                - capabilities (linked list)
                    - file_name
                    - read_permission
                    - write_permission
            - capabilities (linked list)
                - file_name
                - read_permission
                - write_permission
    3. Another diagram is as follows:
        user -> group -> capability
        |
        --> capability
    4.Server will first check if the group that the user belongs to has the capability.
      If the group does not have the capability, the server will check if the user has the capability.
    5. In my design, one user just belongs to one group.
*/

#ifndef CAPABILITY_H
#define CAPABILITY_H
#include <stdbool.h>

typedef struct Capability {
    // The name of the file
    char file_name[256];
    // Permissions for the owner, group, and others
    bool read_permission;
    bool write_permission;
    // Pointer to the next capability in the list
    struct Capability *next;
} Capability;


typedef struct CapabilityList {
    // Pointer to the first capability in the list
    Capability *head;
    // Pointer to the last capability in the list
    Capability *tail;
    // The number of capabilities in the list
    int count;
} CapabilityList;

typedef struct Group {
    // The name of the group
    char group_name[256];
    // Pointer to the capability list for the group
    CapabilityList *capability_list;
    // Pointer to the next group in the list
    struct Group *next;
} Group;

typedef struct Groups {
    // Pointer to the first group in the list
    Group *head;
    // Pointer to the last group in the list
    Group *tail;
    // The number of groups in the list
    int count;
} Groups;

typedef struct User {
    // The name of the user
    char username[256];
    // Pointer to the group the user belongs to
    Groups *groups;
    // Pointer to the capability list for the user
    CapabilityList *capabilities;
    // Pointer to the next user in the list
    struct User *next;
} User;

typedef struct Users {
    // Pointer to the first user in the list
    User *head;
    // Pointer to the last user in the list
    User *tail;
    // The number of users in the list
    int count;
} Users;

bool create_user(User *user, char *username, char *group_name);

Group* create_group(Groups *groups, const char *group_name);

Groups* init_groups(void);

Users* init_users(void);

#endif 