/*
    1. This file contains the declaration of the capability list data structure.
    2. The capability list's structure is as follows:
        User 
            - name: The name of the user
            - group: Pointer to the group the user belongs to
            - capability_list: Pointer to the capability list for the user
            - next: Pointer to the next user in the list
        Users
            - head: Pointer to the first user in the list
            - tail: Pointer to the last user in the list
            - count: The number of users in the list
        Group
            - name: The name of the group
            - capability_list: Pointer to the capability list for the group
            - next: Pointer to the next group in the list
        Groups
            - head: Pointer to the first group in the list
            - tail: Pointer to the last group in the list
            - count: The number of groups in the list
        Capability_List
            - head: Pointer to the first capability in the list
            - tail: Pointer to the last capability in the list
            - count: The number of capabilities in the list
        Capability
            - file: The file the capability is for
            - read_permission: Permissions for the owner, group, and others
            - write_permission: Permissions for the owner, group, and others
            - next: Pointer to the next capability in the list
        File
            - file_name: The name of the file
            - owner: The owner of the file
            - rwlock: The lock for the file
        Files
            - head: Pointer to the first file in the list
            - tail: Pointer to the last file in the list
            - count: The number of files in the list
    3. Another diagram is as follows:
        user -> group -> capability
        |
        --> capability
    4.Server will first check if the user has the capability to perform the operation on the file.
        If the user has the capability, the server will perform the operation.
        If the user does not have the capability and the user is owner, the server will send an error message.
        If the user does not have the capability and the user is not the owner, the server will check the group capability.
    5. In my design, one user just belongs to one group.
*/

#ifndef CAPABILITY_H
#define CAPABILITY_H
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#define FILE_FOLDER "files/"

typedef struct File {
    // The name of the file
    char name[256];
    // The owner of the file
    char owner[256];
    // The lock for the file
    pthread_rwlock_t rwlock;
    // Pointer to the next file in the list
    struct File *next;
} File;


typedef struct Files {
    // Pointer to the first file in the list
    File *head;
    // Pointer to the last file in the list
    File *tail;
    // The number of files in the list
    int count;
} Files;


typedef struct Capability {
    // The file the capability is for
    File *file;
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
    char name[256];
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
    char name[256];
    // Pointer to the group the user belongs to
    Group *group;
    // Pointer to the capability list for the user
    CapabilityList *capability_list;
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

User* create_user(Users *users, Groups *groups, char *username, char *group_name);

Group* find_group_by_name(Groups *groups, const char *group_name);

User* find_user_by_name(Users *users, const char *user_name);

File* find_file_by_name(Files *files, const char *file_name);

Group* create_group(Groups *groups, const char *group_name);

File* create_file(Files* files, const char *file_name, User *owner);

void add_owner_capability(File *file, User *user, bool read_permission, bool write_permission);

void add_group_capability(File *file, Group *group, bool read_permission, bool write_permission);

void add_others_capability(File *file, Users *users, User *owner, bool read_permission, bool write_permission);

void modify_capability(File *file, Users *users, User *owner, bool owner_read, bool owner_write, bool group_read, bool group_write, bool others_read, bool others_write);

bool user_has_capability(User *user, File *file, const char *operation);

bool is_owner(User *user, File *file);

Files* init_files(void);

Groups* init_groups(void);

Users* init_users(void);

void free_system(Groups *groups, Users *users, Files *files);

#endif 