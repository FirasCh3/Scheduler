#include "dynamic_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

const char *skip_list[] = {
    "fifo.c",
    "multilevel.c",
    "preemptive-priority.c",
    "rr_utils.c",
    "rr.c",
};

int should_skip(const char *name) {
    for (int i = 0; skip_list[i] != NULL; i++) {
        if (strcmp(name, skip_list[i]) == 0)
            return 1;
    }
    return 0;
}

static char *remove_extension(const char *filename) {
    char *name = strdup(filename);
    char *dot = strrchr(name, '.');
    if (dot) *dot = '\0';
    return name;
}

int load_policies(const char *path, char ***out_policies) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        *out_policies = NULL;
        return 0;
    }

    struct dirent *entry;
    char **policies = NULL;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {

        if (entry->d_name[0] == '.') continue;

        if ((!strstr(entry->d_name, ".c") && !strstr(entry->d_name, ".txt"))
            || should_skip(entry->d_name))
            continue;

        char *name = remove_extension(entry->d_name);

        char **tmp = realloc(policies, sizeof(char*) * (count + 1));
        if (!tmp) {
            perror("realloc");
            closedir(dir);
            free(name);
            break;
        }

        policies = tmp;
        policies[count] = name;
        count++;
    }

    closedir(dir);
    *out_policies = policies;
    return count;
}
