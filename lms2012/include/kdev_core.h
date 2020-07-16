#ifndef EV3_KDEV
#define EV3_KDEV

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define KDEV_TYPE(pointed) \
    struct { \
        int         refCount; \
        int         fd; \
        const char *path; \
        size_t      mmap_length; \
        pointed     *mmap; \
    }

#define KDEV_INIT(path_, parent_type) { \
    .refCount    = 0, \
    .fd          = -1, \
    .path        = (path_), \
    .mmap_length = sizeof(*((parent_type*)NULL)->mmap), \
    .mmap        = NULL, \
}

#define KDEV_INIT_NOMM(path_) { \
    .refCount    = 0, \
    .fd          = -1, \
    .path        = (path_), \
    .mmap_length = 0, \
    .mmap        = NULL, \
}

#define KDEV_INIT_RAWMM(path_, length_) { \
    .refCount    = 0, \
    .fd          = -1, \
    .path        = (path_), \
    .mmap_length = (length_), \
    .mmap        = NULL, \
}

extern bool Kdev_RefAdd(void *pDevice);
extern bool Kdev_RefDel(void *pDevice);

extern ssize_t Kdev_Write(const void *pDevice, const void *buffer, size_t length, size_t offset);
extern ssize_t Kdev_Read(const void *pDevice, void *buffer, size_t length, size_t offset);
extern int Kdev_Ioctl(const void *pDevice, unsigned long request, void *data);

extern void Kdev_LogError(const char *operation, const char *path);

#endif //EV3_KDEV
