#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "capability.h"
#include "init.h"

Groups *groups;
Users *users;

int main(void) {
    Groups *groups = init_groups();
    Users *users = init_users();
    init_system(groups, users);

    //printf("Group count: %d\n", groups->count);





    free(groups);
    free(users);
    return 0;
}