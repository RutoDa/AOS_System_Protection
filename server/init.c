#include <stdio.h>
#include <stdlib.h>
#include "capability.h"


/**
 * Initialize the system with default groups and users
 * @param groups The list of groups
 * @param users The list of users
 * @param files The list of files
 */
void init_system(Groups *groups, Users *users, Files *files) {
    // Delete Files in the files folder
    system("rm -rf files/*");

    // Create default groups
    Group *group_AOS_students = create_group(groups, "AOS_students");
    Group *group_AOS_staff = create_group(groups, "CSE_students");
    
    // Create AOS students
    User *AOS_user1 = create_user(users, groups, "AOS_user1", "AOS_students");
    User *AOS_user2 = create_user(users, groups, "AOS_user2", "AOS_students");
    User *AOS_user3 = create_user(users, groups, "AOS_user3", "AOS_students");
    User *Ken = create_user(users, groups, "Ken", "AOS_students");
    User *Ken2 = create_user(users, groups, "Ken2", "AOS_students");
    User *Ken3 = create_user(users, groups, "Ken3", "CSE_students");

    // Create CSE students
    User *CSE_user1 = create_user(users, groups, "CSE_user1", "CSE_students");
    User *CSE_user2 = create_user(users, groups, "CSE_user2", "CSE_students");
    User *CSE_user3 = create_user(users, groups, "CSE_user3", "CSE_students");

}