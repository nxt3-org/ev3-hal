#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "kdev_core.h"

typedef KDEV_TYPE(void) kdev_real_t;

bool Kdev_RefAdd(void *pDeviceG) {
    kdev_real_t *pDevice = pDeviceG;

    if (pDevice->refCount > 0) {
        pDevice->refCount++;
        return true;
    }

    pDevice->fd   = -1;
    pDevice->mmap = NULL;

    int fd = open(pDevice->path, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        Kdev_LogError("open", pDevice->path);
        return false;
    }

    if (pDevice->mmap_length > 0) {
        void *memory = mmap(NULL, pDevice->mmap_length,
                            PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED,
                            fd, 0);
        if (memory == MAP_FAILED) {
            Kdev_LogError("mmap", pDevice->path);
            close(fd);
            return false;
        }
        pDevice->mmap = memory;
    }

    pDevice->fd = fd;
    pDevice->refCount++;
    return true;
}

bool Kdev_RefDel(void *pDeviceG) {
    kdev_real_t *pDevice = pDeviceG;

    if (pDevice->refCount == 0)
        return false;
    if (pDevice->refCount == 1) {
        if (pDevice->fd >= 0) {
            if (close(pDevice->fd) < 0)
                Kdev_LogError("close", pDevice->path);
            pDevice->fd = -1;
        }
        if (pDevice->mmap) {
            if (munmap(pDevice->mmap, pDevice->mmap_length) < 0)
                Kdev_LogError("munmap", pDevice->path);
            pDevice->mmap = NULL;
        }
    }
    pDevice->refCount--;
    return true;
}

ssize_t Kdev_Write(const void *pDeviceG, const void *buffer, size_t length, size_t offset) {
    const kdev_real_t *pDevice = pDeviceG;

    if (pDevice->refCount <= 0)
        return false;

    int written = pwrite(pDevice->fd, buffer, length, offset);
    if (written < 0) {
        Kdev_LogError("write", pDevice->path);
    }
    return written;
}

ssize_t Kdev_Read(const void *pDeviceG, void *buffer, size_t length, size_t offset) {
    const kdev_real_t *pDevice = pDeviceG;

    if (pDevice->refCount <= 0)
        return false;

    int read = pread(pDevice->fd, buffer, length, offset);
    if (read < 0) {
        Kdev_LogError("read", pDevice->path);
    }
    return read;
}

int Kdev_Ioctl(const void *pDeviceG, unsigned long request, void *data) {
    const kdev_real_t *pDevice = pDeviceG;

    if (pDevice->refCount <= 0)
        return false;

    int retval = ioctl(pDevice->fd, request, data);
    if (retval < 0) {
        Kdev_LogError("ioctl", pDevice->path);
    }
    return retval;
}

void Kdev_LogError(const char *operation, const char *path) {
    int  _errno = errno;
    char *msg   = NULL;
    if (asprintf(&msg, "EV3 HAL: cannot %s device %s", operation, path) > 0) {
        errno = _errno;
        perror(msg);
        free(msg);
    } else {
        errno = _errno;
        perror("EV3 HAL: cannot operate on device <log error: cannot allocate memory>");
    }
    errno       = _errno;
}
