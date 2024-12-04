#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "capability.h"

pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t groups_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t files_mutex = PTHREAD_MUTEX_INITIALIZER;


Group* create_group(Groups *groups, const char *group_name) {
    Group *group = malloc(sizeof(Group));
    if (group == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for group\n");
        exit(1);
    }
    strncpy(group->name, group_name, strlen(group_name)+1);
    group->capability_list = malloc(sizeof(CapabilityList));
    if (group->capability_list == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for group capabilities\n");
        exit(1);
    }
    group->capability_list->head = NULL;
    group->capability_list->tail = NULL;
    group->capability_list->count = 0;
    group->next = NULL;

    pthread_mutex_lock(&groups_mutex);

    if (groups->head == NULL) {
        groups->head = group;
        groups->tail = group;
    } else {
        groups->tail->next = group;
        groups->tail = group;
    }
    groups->count++;

    pthread_mutex_unlock(&groups_mutex);

    return group;
}

Group* find_group_by_name(Groups *groups, const char *group_name) {
    pthread_mutex_lock(&groups_mutex);
    for (Group* group = groups->head; group != NULL; group = group->next) {
        if (!strcmp(group->name, group_name)) {
            pthread_mutex_unlock(&groups_mutex);
            return group;
        }
            
    }
    pthread_mutex_unlock(&groups_mutex);
    return NULL;
}

User* find_user_by_name(Users *users, const char *user_name) {
    pthread_mutex_lock(&users_mutex);
    for (User* user = users->head; user != NULL; user = user->next) {
        if (!strcmp(user->name, user_name)) {
            pthread_mutex_unlock(&users_mutex);
            return user;
        }
    }
    pthread_mutex_unlock(&users_mutex);
    return NULL;
}

File* find_file_by_name(Files *files, const char *file_name) {
    pthread_mutex_lock(&files_mutex);
    for (File* file = files->head; file != NULL; file = file->next) {
        if (!strcmp(file->name, file_name)) {
            pthread_mutex_unlock(&files_mutex);
            return file;
        }
    }
    pthread_mutex_unlock(&files_mutex);
    return NULL;
}

User* create_user(Users *users, Groups *groups, char *username, char *group_name) {
    User *user = malloc(sizeof(User));
    if (user == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for user\n");
        exit(1);
    }
    strncpy(user->name, username, strlen(username)+1);
    
    Group* group = find_group_by_name(groups, group_name);
    if (group == NULL) {
        fprintf(stderr, "Error: Group %s not found\n", group_name);
        exit(1);
    }

    user->group = group;
    user->capability_list = malloc(sizeof(CapabilityList));
    if (user->capability_list == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for user capabilities\n");
        exit(1);
    }
    user->capability_list->head = NULL;
    user->capability_list->tail = NULL;
    user->capability_list->count = 0;
    user->next = NULL;
    
    pthread_mutex_lock(&users_mutex);

    if (users->head == NULL) {
        users->head = user;
        users->tail = user;
    } else {
        users->tail->next = user;
        users->tail = user;
    }
    users->count++;

    pthread_mutex_unlock(&users_mutex);

    return user;
}

File* create_file(Files* files, const char *file_name) {
    File *file = malloc(sizeof(File));
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for file\n");
        exit(1);
    }
    strncpy(file->name, file_name, strlen(file_name)+1);
    file->next = NULL;
    if (pthread_rwlock_init(&file->rwlock, NULL) != 0) {
        perror("Failed to initialize rwlock");
        exit(1);
    }
    
    char path[256];
    snprintf(path, sizeof(path), "files/%s", file_name);
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Unable to create file %s\n", path);
        free(file);
        exit(1);
    }
    fclose(fp);

    pthread_mutex_lock(&files_mutex);

    if (files->head == NULL) {
        files->head = file;
        files->tail = file;
    } else {
        files->tail->next = file;
        files->tail = file;
    }
    files->count++;

    pthread_mutex_unlock(&files_mutex);

    return file;
}

void add_owner_capability(File *file, User *user, bool read_permission, bool write_permission) {
    Capability *capability = malloc(sizeof(Capability));
    if (capability == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for capability\n");
        exit(1);
    }
    capability->file = file;
    capability->read_permission = read_permission;
    capability->write_permission = write_permission;
    capability->next = NULL;

    pthread_mutex_lock(&users_mutex);

    
    if (user->capability_list->head == NULL) {
        user->capability_list->head = capability;
        user->capability_list->tail = capability;
    } else {
        user->capability_list->tail->next = capability;
        user->capability_list->tail = capability;
    }
    user->capability_list->count++;

    pthread_mutex_unlock(&users_mutex);
}

void add_group_capability(File *file, Group *group, bool read_permission, bool write_permission) {
    Capability *capability = malloc(sizeof(Capability));
    if (capability == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for capability\n");
        exit(1);
    }
    capability->file = file;
    capability->read_permission = read_permission;
    capability->write_permission = write_permission;
    capability->next = NULL;

    pthread_mutex_lock(&groups_mutex);

    if (group->capability_list->head == NULL) {
        group->capability_list->head = capability;
        group->capability_list->tail = capability;
    } else {
        group->capability_list->tail->next = capability;
        group->capability_list->tail = capability;
    }
    group->capability_list->count++;

    pthread_mutex_unlock(&groups_mutex);
}

void add_others_capability(File *file, Users *users, User *owner, bool read_permission, bool write_permission) {
    Capability *capability = malloc(sizeof(Capability));
    if (capability == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for capability\n");
        exit(1);
    }
    capability->file = file;
    capability->read_permission = read_permission;
    capability->write_permission = write_permission;
    capability->next = NULL;

    pthread_mutex_lock(&users_mutex);

    for (User* user = users->head; user != NULL; user = user->next) {
        if (strcmp(user->name, owner->name)) {
            if (user->capability_list->head == NULL) {
                user->capability_list->head = capability;
                user->capability_list->tail = capability;
            } else {
                user->capability_list->tail->next = capability;
                user->capability_list->tail = capability;
            }
            user->capability_list->count++;
        }
    }

    pthread_mutex_unlock(&users_mutex);
}

bool user_has_capability(User *user, File *file) {
    CapabilityList *group_cap = user->group->capability_list;

    for (Capability *cap = group_cap->head; cap != NULL; cap = cap->next) {
        if (!strcmp(cap->file->name, file->name)) {
            if (cap->read_permission)
                return true;
            else break;
        }
    }
    
    CapabilityList *user_cap = user->capability_list;

    for (Capability *cap = user_cap->head; cap != NULL; cap = cap->next) {
        if (!strcmp(cap->file->name, file->name)) {
            if (cap->read_permission)
                return true;
            else break;
        }
    }

    return false;
}

Groups* init_groups(void) {
    Groups *groups = malloc(sizeof(Groups));
    if (groups == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for groups\n");
        exit(1);
    }
    groups->head = NULL;
    groups->tail = NULL;
    groups->count = 0;
    return groups;
}

Users* init_users(void) {
    Users *users = malloc(sizeof(Users));
    if (users == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for users\n");
        exit(1);
    }
    users->head = NULL;
    users->tail = NULL;
    users->count = 0;
    return users;
}

Files* init_files(void) {
    Files *files = malloc(sizeof(Files));
    if (files == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for files\n");
        exit(1);
    }
    files->head = NULL;
    files->tail = NULL;
    files->count = 0;
    return files;
}

void free_system(Groups *groups, Users *users, Files *files) {
    Group *group = groups->head;
    while (group != NULL) {
        Group *next_group = group->next;
        
        while (group->capability_list->head != NULL) {
            Capability *capability = group->capability_list->head;
            group->capability_list->head = capability->next;
            free(capability);
        }

        free(group->capability_list);
        free(group);
        group = next_group;
    }
    free(groups);
    
    User *user = users->head;
    while (user != NULL) {
        User *next_user = user->next;

        while (user->capability_list->head != NULL) {
            Capability *capability = user->capability_list->head;
            user->capability_list->head = capability->next;
            free(capability);
        }

        free(user->capability_list);
        free(user);
        user = next_user;
    }
    free(users);

    File *file = files->head;
    while (file != NULL) {
        File *next_file = file->next;
        free(file);
        file = next_file;
    }
    free(files);
}

