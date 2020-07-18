#ifndef POSIX_FS_PRIVATE
#define POSIX_FS_PRIVATE

#ifndef O_CLOEXEC
#define O_CLOEXEC   02000000
#endif

#ifndef O_DIRECTORY
#define O_DIRECTORY  0200000
#endif

typedef struct {
    uint32_t Version;
    uint32_t DataSize;
} FILE_META;

#define FILE_META_VERSION_1 0x00000001
#define FILE_META_VERSION   FILE_META_VERSION_1

extern error_t mapErrno(int error);
extern error_t reportErrno(const char *message);

#endif //POSIX_FS_PRIVATE
