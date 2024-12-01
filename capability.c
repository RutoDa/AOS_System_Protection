#include <stdio.h>
#include <stdlib.h>
#include "capability.h"


Group* create_group(Groups *groups, const char *group_name) {
    Group *group = malloc(sizeof(Group));
    if (group == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for group\n");
        exit(1);
    }
    strncpy(group->group_name, group_name, strlen(group_name)+1);
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
