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
    uint32_t WrittenSize;
    uint32_t FullSize;
} FILE_META_V2;

#define FILE_META_VERSION_2 0x00000002

extern fserr_t mapErrno(int error);
extern fserr_t reportErrno(const char *message);

#endif //POSIX_FS_PRIVATE
