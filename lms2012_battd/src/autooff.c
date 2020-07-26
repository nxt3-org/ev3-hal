#include "autooff.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

static int pidof(const char *name) {
    char buffer[64];
    int pid = 0;
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("a/open");
        return -1;
    }
    // scan /proc for all processes
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR) // not a directory
            continue;
        char *end;
        int thePid = strtol(entry->d_name, &end, 10);
        if (entry->d_name == end) // not a number
            continue;
        snprintf(buffer, 64, "/proc/%s/comm", entry->d_name);
        int fd = open(buffer, O_RDONLY);
        if (fd < 0) // does not have comm
            continue;
        int bytes = read(fd, buffer, 64);
        if (bytes <= 0)
            continue;
        buffer[bytes - 1] = '\0'; // replace newline with null terminator
        close(fd);
        if (strncmp(buffer, name, 64) == 0) {
            pid = thePid;
            break;
        }
    }

    closedir(dir);
    return pid;
}

static bool doesExist(const char *commPath) {
    // low-overhead check (avoids pidof process creation, /proc traversal, etc.)
    return access(commPath, F_OK) == 0;
}

static char lms2012Comm[64];
bool autooff_open(void) {
    int pid = pidof("lms2012");
    if (pid <= 0)
        return false;
    snprintf(lms2012Comm, 64, "/proc/%d/comm", pid);
    return !autooff_should_exit();
}

void autooff_close(void) {
    return;
}

bool autooff_should_exit(void) {
    return !doesExist(lms2012Comm);
}
