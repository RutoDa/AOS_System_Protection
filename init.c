#include <stdio.h>
#include <stdlib.h>
#include "capability.h"


void init_system(Groups *groups, Users *users) {

    // Create default groups
    Group *group_AOS_students = create_group(groups, "AOS_students");
    Group *group_AOS_staff = create_group(groups, "CSE_students");
    
    // Create AOS students
    User *AOS_user1 = create_user(users, groups, "AOS_user1", "AOS_students");
    User *AOS_user2 = create_user(users, groups, "AOS_user2", "AOS_students");
    User *AOS_user3 = create_user(users, groups, "AOS_user3", "AOS_students");
    User *Ken = create_user(users, groups, "Ken", "AOS_students");

    // Create CSE students
    User *CSE_user1 = create_user(users, groups, "CSE_user1", "CSE_students");
    User *CSE_user2 = create_user(users, groups, "CSE_user2", "CSE_students");
    User *CSE_user3 = create_user(users, groups, "CSE_user3", "CSE_students");
}