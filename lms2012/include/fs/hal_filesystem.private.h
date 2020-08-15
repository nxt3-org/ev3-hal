#ifndef HAL_FILESYSTEM_PRIVATE
#define HAL_FILESYSTEM_PRIVATE

#include <dirent.h>
#include "hal_filesystem.h"

typedef struct {
    char           name[FS_NAME_MAX_BYTES];
    int            linuxFd;
    uint8_t        *memCopy;
    uint32_t       fullLength;
    uint32_t       readPointer;
    uint32_t       writePointer;
    search_query_t query;
    DIR            *queryDir;
    openmode_t     mode;
    bool isReal;
} handle_data_t;

typedef struct {
    int           refCount;
    handle_data_t handles[MAX_HANDLES];
    char          *dataDir;
    char          *metaDir;
    int           dataDirFd;
    int           metaDirFd;
    int           testFd;
} mod_fs_t;

extern mod_fs_t Mod_Fs;

extern errhnd_t allocateHandle(openmode_t newMode);

extern fserr_t checkConditions_Read(handle_t hnd);
extern fserr_t checkConditions_Write(handle_t hnd);
extern fserr_t checkConditions_AnyOpen(handle_t hnd);
extern fserr_t checkConditions_OpenFn(handle_t hnd);

#endif //HAL_FILESYSTEM_PRIVATE
