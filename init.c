#include <stdio.h>
#include <stdlib.h>
#include "capability.h"


void init_system(Groups *groups, Users *users) {
    Group *group_AOS_students = create_group(groups, "AOS_students");
    printf("Group name: %s\n", group_AOS_students->group_name);
    printf("Group count: %d\n", groups->count);
}