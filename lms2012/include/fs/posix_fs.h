#ifndef POSIX_FS
#define POSIX_FS

#include <stdbool.h>
#include <stdint.h>

extern bool posixFsInit(void);
extern void posixFsExit(void);

extern error_t posixFsLoadMeta(handle_data_t *pH);
extern error_t posixFsSaveMeta(handle_data_t *pH);

extern error_t posixFsCreateWrite(handle_data_t *pH, const char *name, uint32_t fileSize);
extern error_t posixFsOpenRead(handle_data_t *pH);
extern error_t posixFsOpenAppend(handle_data_t *pH);

extern error_t posixFsReadAll(handle_data_t *pH);
extern error_t posixFsRead(handle_data_t *pH, void *buffer, uint32_t *pLength);
extern error_t posixFsWrite(handle_data_t *pH, const void *buffer, uint32_t *pLength);
extern error_t posixFsSeekRead(handle_data_t *pH, int32_t offset, seek_t mode);
extern error_t posixFsTellRead(handle_data_t *pH, uint32_t *filePosition);

extern error_t posixFsRemove(handle_data_t *pH);
extern error_t posixFsMove(handle_data_t *pH, const char *name);
extern error_t posixFsShrink(handle_data_t *pH);
extern error_t posixFsResize(handle_data_t *pH, uint32_t newLength);

extern error_t posixFsClose(handle_data_t *pH, bool saveMeta);

extern error_t posixFsStartBrowse(DIR **pStream);
extern error_t posixFsBrowseNext(DIR *stream, struct dirent **ppDirent);
extern void    posixFsFinishBrowse(DIR **pStream);

extern error_t posixFsGetFreeBytes(uint32_t *pBytes);

extern bool posixFsGetDefaultDirs(char **pDataDir, char **pMetaDir);
#endif //POSIX_FS
