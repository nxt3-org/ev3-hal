#ifndef HAL_FILESYSTEM
#define HAL_FILESYSTEM

#include <stdbool.h>
#include <stdint.h>
#include "hal_errorcodes.h"

#define   MAX_HANDLES                   16

#define   FS_NAME_MAX_CHARS  19    // zero termination not included
#define   FS_NAME_MAX_BYTES  (FS_NAME_MAX_CHARS + 1)

// compatibility define
#define   FILENAME_SIZE                 FS_NAME_MAX_BYTES

#define   FULLNAME                      1
#define   NAME                          2
#define   EXTENTION                     3
#define   WILDCARD                      4


/* Enum related to seek operation */
typedef enum {
    SEEK_FROMSTART,
    SEEK_FROMCURRENT,
    SEEK_FROMEND
} seek_t;

typedef enum {
    OPENMODE_CLOSED,
    OPENMODE_READ,
    OPENMODE_WRITE,
    OPENMODE_APPEND,
    OPENMODE_REFERENCE
} openmode_t;

typedef struct {
    char name[FS_NAME_MAX_BYTES];
    uint32_t allocated;
    uint32_t readPointer;
    uint32_t writePointer;
    openmode_t openMode;
    bool found;
} file_meta_t;

typedef enum {
    SEARCH_EVERYTHING,
    SEARCH_FILENAME_WITHOUT_EXT,
    SEARCH_EXTENSION,
    SEARCH_FILENAME
} search_type_t;

typedef struct {
    char          content[FS_NAME_MAX_BYTES];
    search_type_t lookFor;
} search_query_t;

extern bool      Hal_Fs_RefAdd(void);
extern bool      Hal_Fs_RefDel(void);

extern errhnd_t  Hal_Fs_CreateWrite(const char *name, uint32_t length);
extern error_t   Hal_Fs_OpenRead(handle_t handle);
extern error_t   Hal_Fs_OpenAppend(handle_t handle);
extern error_t   Hal_Fs_Close(errhnd_t errHandle);

extern error_t   Hal_Fs_Read(handle_t handle, void *buffer, uint32_t *pLength);
extern error_t   Hal_Fs_Write(handle_t handle, const void *buffer, uint32_t *pLength);
extern error_t   Hal_Fs_Seek(handle_t handle, int32_t offset, seek_t mode);
extern error_t   Hal_Fs_Tell(handle_t handle, uint32_t *pFilePos);
extern error_t   Hal_Fs_Truncate(handle_t handle);
extern error_t   Hal_Fs_Resize(handle_t handle, uint32_t newSize);

extern error_t   Hal_Fs_MapFile(handle_t handle, const uint8_t **mapped, uint32_t *pLength);

extern error_t   Hal_Fs_RenameFile(handle_t handle, const char *newName);
extern error_t   Hal_Fs_DeleteFile(handle_t handle);
extern error_t   Hal_Fs_DeleteAll(void);
extern error_t   Hal_Fs_GetFreeStorage(uint32_t *pBytes);

extern error_t   Hal_Fs_CheckHandleIsExclusive(handle_t handle, bool searchToo);

extern void      Hal_Fs_ParseQuery(const char *query, search_query_t *pResult);
extern error_t   Hal_Fs_CheckQuery(const char *test, search_query_t *pQuery);

extern errhnd_t  Hal_Fs_Locate(const char *query, char *name, uint32_t *pLength);
extern errhnd_t  Hal_Fs_Locate2(search_query_t *pQuery, char *name, uint32_t *pLength);
extern errhnd_t  Hal_Fs_LocateNext(handle_t handle, char *name, uint32_t *pLength);
extern error_t   Hal_Fs_GetMeta(handle_t handle, file_meta_t *pMeta);
extern bool      Hal_Fs_CheckForbiddenFilename(const char *name);


#endif //HAL_FILESYSTEM
