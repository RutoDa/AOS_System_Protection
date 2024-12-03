#include <stdio.h>
#include <stdlib.h>
#include "capability.h"


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
    if (groups->head == NULL) {
        groups->head = group;
        groups->tail = group;
    } else {
        groups->tail->next = group;
        groups->tail = group;
    }
    groups->count++;
    return group;
}

Group* find_group_by_name(Groups *groups, const char *group_name) {
    for (Group* group = groups->head; group != NULL; group = group->next) {
        if (!strcmp(group->name, group_name))
            return group;
    }
    return NULL;
}

User* find_user_by_name(Users *users, const char *user_name) {
    for (User* user = users->head; user != NULL; user = user->next) {
        if (!strcmp(user->name, user_name)) 
            return user;
    }
    return NULL;
}

User* create_user(Users *users, Group *groups, char *username, char *group_name) {
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
    if (users->head == NULL) {
        users->head = user;
        users->tail = user;
    } else {
        users->tail->next = user;
        users->tail = user;
    }
    users->count++;
    return user;
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


void free_system(Groups *groups, Users *users) {
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
}