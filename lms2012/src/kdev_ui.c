#include <kdev_ui.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

kdev_ui_t Kdev_UI = {
    .refCount = 0,
    .mmap = NULL,
    .fd = -1,
};

bool Kdev_UI_RefAdd(void) {
    if (Kdev_UI.refCount > 0) {
        Kdev_UI.refCount++;
        return true;
    }

    Kdev_UI.fd = open("/dev/lms_ui", O_RDWR);
    if (Kdev_UI.fd < 0) {
        perror("EV3 HAL: Cannot open LMS UI device");
        return false;
    }

    Kdev_UI.mmap = mmap(NULL, sizeof(kdev_ui_mmap_t), PROT_READ, MAP_SHARED | MAP_LOCKED, Kdev_UI.fd, 0);
    if (Kdev_UI.mmap == MAP_FAILED) {
        Kdev_UI.mmap = NULL;
        perror("EV3 HAL: cannot mmap LMS UI device");

        close(Kdev_UI.fd);
        Kdev_UI.fd = -1;
        return false;
    }

    Kdev_UI.refCount++;
    return true;
}
bool Kdev_UI_RefDel(void) {
    if (Kdev_UI.refCount == 0)
        return false;

    Kdev_UI.refCount--;
    if (Kdev_UI.refCount == 0) {
        if (Kdev_UI.mmap) {
            if (munmap((void *) Kdev_UI.mmap, sizeof(kdev_ui_mmap_t)) < 0) {
                perror("EV3 HAL: cannot munmap LMS UI device");
            }
            Kdev_UI.mmap = NULL;
        }

        if (Kdev_UI.fd >= 0) {
            if (close(Kdev_UI.fd) < 0) {
                perror("EV3 HAL: cannot close LMS UI device");
            }
            Kdev_UI.fd = -1;
        }
    }
    return true;
}
