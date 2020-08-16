#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include "hal_filesystem.h"
#include "fs/hal_filesystem.private.h"
#include "fs/posix_fs.h"

mod_fs_t Mod_Fs;

bool Hal_Fs_RefAdd(void) {
    if (Mod_Fs.refCount > 0) {
        Mod_Fs.refCount++;
        return true;
    }

    for (handle_t hnd = 0; hnd < MAX_HANDLES; hnd++) {
        Mod_Fs.handles[hnd].name[0] = '\0';
        Mod_Fs.handles[hnd].linuxFd      = -1;
        Mod_Fs.handles[hnd].memCopy      = NULL;
        Mod_Fs.handles[hnd].fullLength   = 0;
        Mod_Fs.handles[hnd].readPointer  = 0;
        Mod_Fs.handles[hnd].writePointer = 0;
        Mod_Fs.handles[hnd].query.content[0] = '\0';
        Mod_Fs.handles[hnd].query.lookFor = SEARCH_FILENAME;
        Mod_Fs.handles[hnd].queryDir      = NULL;
        Mod_Fs.handles[hnd].mode          = OPENMODE_CLOSED;
        Mod_Fs.handles[hnd].isReal        = false;
    }

    if (!posixFsGetDefaultDirs(&Mod_Fs.dataDir, &Mod_Fs.metaDir)) {
        return false;
    }

    bool initOK = posixFsInit();
    if (!initOK) {
        free((void *) Mod_Fs.dataDir);
        free((void *) Mod_Fs.metaDir);
        Mod_Fs.dataDir = NULL;
        Mod_Fs.metaDir = NULL;
        return false;
    }

    Mod_Fs.refCount++;
    return true;
}

bool Hal_Fs_RefDel(void) {
    if (Mod_Fs.refCount == 0)
        return false;
    if (Mod_Fs.refCount == 1) {
        for (handle_t hnd = 0; hnd < MAX_HANDLES; hnd++) {
            Hal_Fs_Close(hnd);
        }
        posixFsExit();
        free((void *) Mod_Fs.dataDir);
        free((void *) Mod_Fs.metaDir);
        Mod_Fs.dataDir = NULL;
        Mod_Fs.metaDir = NULL;
    }
    Mod_Fs.refCount--;
    return true;
}

errhnd_t allocateHandle(openmode_t newMode) {
    for (handle_t hnd = 0; hnd < MAX_HANDLES; hnd++) {
        if (Mod_Fs.handles[hnd].mode == OPENMODE_CLOSED) {
            Mod_Fs.handles[hnd].mode = newMode;
            return hnd;
        }
    }
    return NOMOREHANDLES;
}

errhnd_t Hal_Fs_CreateWrite(const char *name, uint32_t length) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;
    if (!Hal_Fs_CheckForbiddenFilename(name))
        return ILLEGALFILENAME;

    errhnd_t existing = Hal_Fs_Locate(name, NULL, NULL);
    if (FS_ERR(existing) == NOMOREHANDLES)
        return NOMOREHANDLES;
    if (!FS_ISERR(existing))
        return FS_HND(existing) | FILEEXISTS;
    if (FS_ERR(existing) != FILENOTFOUND)
        return FS_HND(existing) | UNDEFINEDERROR;

    handle_t handle = FS_HND(existing);
    fserr_t  err    = posixFsCreateWrite(&Mod_Fs.handles[handle], name, length);
    if (err == SUCCESS) {
        Mod_Fs.handles[handle].mode = OPENMODE_WRITE;
        return handle;
    } else {
        return err | handle;
    }
}

fserr_t Hal_Fs_OpenRead(handle_t handle) {
    fserr_t err = checkConditions_OpenFn(handle);
    if (FS_ISERR(err))
        return err;

    handle_data_t *pH = &Mod_Fs.handles[handle];

    err = posixFsOpenRead(pH);
    if (err == SUCCESS) {
        posixFsFinishBrowse(&pH->queryDir);
        pH->mode = OPENMODE_READ;
        return handle;
    } else {
        return err;
    }
}

fserr_t Hal_Fs_OpenAppend(handle_t handle) {
    fserr_t err = checkConditions_OpenFn(handle);
    if (FS_ISERR(err))
        return err;

    handle_data_t *pH = &Mod_Fs.handles[handle];

    err = posixFsOpenAppend(pH);
    if (err == SUCCESS) {
        posixFsFinishBrowse(&pH->queryDir);
        pH->mode = OPENMODE_APPEND;
        return handle;
    } else {
        return err;
    }
}

fserr_t Hal_Fs_Close(errhnd_t errHandle) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;

    if (FS_ERR(errHandle) == NOMOREHANDLES)
        return NOMOREHANDLES;

    handle_t hnd = FS_HND(errHandle);
    if (hnd >= MAX_HANDLES)
        return ILLEGALHANDLE;

    handle_data_t *pH = &Mod_Fs.handles[hnd];
    if (pH->mode == OPENMODE_CLOSED) {
        return HANDLEALREADYCLOSED;
    } else {
        bool saveMeta = pH->mode == OPENMODE_WRITE || pH->mode == OPENMODE_APPEND;

        fserr_t result = posixFsClose(pH, saveMeta);
        pH->mode = OPENMODE_CLOSED;
        return result;
    }
}


fserr_t Hal_Fs_Read(handle_t handle, void *buffer, uint32_t *pLength) {
    fserr_t err = checkConditions_Read(handle);
    if (FS_ISERR(err))
        return err;

    return posixFsRead(&Mod_Fs.handles[handle], buffer, pLength);
}

fserr_t Hal_Fs_Write(handle_t handle, const void *buffer, uint32_t *pLength) {
    fserr_t err = checkConditions_Write(handle);
    if (FS_ISERR(err))
        return err;

    return posixFsWrite(&Mod_Fs.handles[handle], buffer, pLength);
}

fserr_t Hal_Fs_Seek(handle_t handle, int32_t offset, seek_t mode) {
    fserr_t err = checkConditions_Read(handle);
    if (FS_ISERR(err))
        return err;

    return posixFsSeekRead(&Mod_Fs.handles[handle], offset, mode);
}

fserr_t Hal_Fs_Tell(handle_t handle, uint32_t *pFilePos) {
    fserr_t err = checkConditions_Read(handle);
    if (FS_ISERR(err))
        return err;

    return posixFsTellRead(&Mod_Fs.handles[handle], pFilePos);
}

fserr_t Hal_Fs_Truncate(handle_t handle) {
    fserr_t err = checkConditions_Write(handle);
    if (FS_ISERR(err))
        return err;

    handle_data_t *pH = &Mod_Fs.handles[handle];

    fserr_t code = posixFsShrink(pH);
    pH->mode = OPENMODE_CLOSED;
    return code;
}

fserr_t Hal_Fs_Resize(handle_t handle, uint32_t newSize) {
    fserr_t err = checkConditions_Write(handle);
    if (FS_ISERR(err))
        return err;

    handle_data_t *pH = &Mod_Fs.handles[handle];

    fserr_t code = posixFsResize(pH, newSize);
    pH->mode = OPENMODE_CLOSED;
    return code;
}


fserr_t Hal_Fs_MapFile(handle_t handle, const uint8_t **mapped, uint32_t *pLength) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;
    if (handle >= MAX_HANDLES)
        return ILLEGALHANDLE;

    handle_data_t *pH = &Mod_Fs.handles[handle];
    fserr_t       err;

    if (pH->mode != OPENMODE_READ) {
        err = Hal_Fs_OpenRead(handle);
        if (FS_ISERR(err))
            return err;
    }

    err = posixFsReadAll(pH);
    if (FS_ISERR(err))
        return err;

    if (mapped)
        *mapped  = pH->memCopy;
    if (pLength)
        *pLength = pH->fullLength;

    return SUCCESS;
}


fserr_t Hal_Fs_RenameFile(handle_t handle, const char *newName) {
    fserr_t err = checkConditions_AnyOpen(handle);
    if (FS_ISERR(err))
        return err;

    if (!Hal_Fs_CheckForbiddenFilename(newName))
        return ILLEGALFILENAME;

    return posixFsMove(&Mod_Fs.handles[handle], newName);
}

fserr_t Hal_Fs_DeleteFile(handle_t handle) {
    fserr_t err = checkConditions_AnyOpen(handle);
    if (FS_ISERR(err))
        return err;

    handle_data_t *pH = &Mod_Fs.handles[handle];

    fserr_t result = posixFsRemove(pH);
    pH->mode = OPENMODE_CLOSED;
    return result;
}

fserr_t Hal_Fs_DeleteAll(void) {
    for (handle_t hnd = 0; hnd < MAX_HANDLES; hnd++) {
        Hal_Fs_Close(hnd);
    }

    errhnd_t hnd = Hal_Fs_Locate("*.*", NULL, NULL);
    while (!FS_ISERR(hnd)) {
        Hal_Fs_DeleteFile(hnd);
        hnd = Hal_Fs_Locate("*.*", NULL, NULL);
    }
    Hal_Fs_Close(hnd);
    return SUCCESS;
}

fserr_t Hal_Fs_GetFreeStorage(uint32_t *pBytes) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;

    return posixFsGetFreeBytes(pBytes);
}


fserr_t Hal_Fs_CheckHandleIsExclusive(handle_t handle, bool searchToo) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;
    if (handle >= MAX_HANDLES)
        return ILLEGALHANDLE;

    for (handle_t hnd = 0; hnd < MAX_HANDLES; hnd++) {
        if (hnd == handle)
            continue;
        if (Mod_Fs.handles[hnd].mode == OPENMODE_CLOSED)
            continue;
        if (!searchToo && Mod_Fs.handles[hnd].mode == OPENMODE_REFERENCE)
            continue;
        if (0 == strcasecmp(Mod_Fs.handles[hnd].name, Mod_Fs.handles[handle].name))
            return FILEISBUSY;
        // this won't detect symlinks/hardlinks, but those cannot be created easily anyways
    }
    return SUCCESS;
}


errhnd_t Hal_Fs_Locate(const char *query, char *name, uint32_t *pLength) {
    search_query_t parsed;
    Hal_Fs_ParseQuery(query, &parsed);
    return Hal_Fs_Locate2(&parsed, name, pLength);
}

errhnd_t Hal_Fs_Locate2(search_query_t *pQuery, char *name, uint32_t *pLength) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;

    errhnd_t handle = allocateHandle(OPENMODE_REFERENCE);
    if (FS_ISERR(handle))
        return handle;

    Mod_Fs.handles[handle].queryDir = NULL;
    Mod_Fs.handles[handle].query    = *pQuery;

    return Hal_Fs_LocateNext(handle, name, pLength);
}

errhnd_t Hal_Fs_LocateNext(handle_t handle, char *name, uint32_t *pLength) {
    if (name)
        name[0] = '\0';
    if (pLength)
        *pLength = 0;

    if (Mod_Fs.refCount <= 0)
        return handle | UNDEFINEDERROR;
    if (handle >= MAX_HANDLES)
        return handle | ILLEGALHANDLE;

    handle_data_t *pH = &Mod_Fs.handles[handle];
    fserr_t       err;

    if (pH->mode != OPENMODE_REFERENCE)
        return handle | ILLEGALHANDLE;

    if (pH->queryDir == NULL) {
        err = posixFsStartBrowse(&pH->queryDir);
        if (FS_ISERR(err))
            return handle | err;
    }

    struct dirent *pEntry;
    while ((err = posixFsBrowseNext(pH->queryDir, &pEntry)) == SUCCESS) {
        if (pEntry->d_type != DT_REG)
            continue;
        if (SUCCESS == Hal_Fs_CheckQuery(pEntry->d_name, &pH->query)) {
            break;
        }
    }
    if (err != SUCCESS)
        return handle | err;

    strncpy(pH->name, pEntry->d_name, FS_NAME_MAX_CHARS);
    pH->isReal = true;

    errhnd_t result = handle | posixFsLoadMeta(pH);
    if (name)
        strncpy(name, pH->name, FS_NAME_MAX_CHARS);
    if (pLength)
        *pLength = pH->fullLength;
    return result;
}

fserr_t Hal_Fs_GetMeta(handle_t handle, file_meta_t *pMeta) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;
    if (handle >= MAX_HANDLES)
        return ILLEGALHANDLE;

    handle_data_t *pH = &Mod_Fs.handles[handle];

    pMeta->openMode = pH->mode;
    memset(pMeta->name, '\0', FS_NAME_MAX_BYTES);

    switch (pH->mode) {
    case OPENMODE_READ:
    case OPENMODE_WRITE:
    case OPENMODE_APPEND:
        pMeta->readPointer  = pH->readPointer;
        pMeta->writePointer = pH->writePointer;
        pMeta->allocated    = pH->fullLength;
        pMeta->found        = true;
        strncpy(pMeta->name, pH->name, FS_NAME_MAX_CHARS);
        return SUCCESS;
    case OPENMODE_REFERENCE:
        if (pH->isReal) {
            pMeta->readPointer  = pH->readPointer;
            pMeta->writePointer = pH->writePointer;
            pMeta->allocated    = pH->fullLength;
            pMeta->found        = true;
            strncpy(pMeta->name, pH->name, FS_NAME_MAX_CHARS);
        } else {
            pMeta->readPointer  = 0;
            pMeta->writePointer = 0;
            pMeta->allocated    = 0;
            pMeta->found        = false;
            strcpy(pMeta->name, "");
        }
        return SUCCESS;
    default:
        return ILLEGALHANDLE;
    }
}

fserr_t checkConditions_Read(handle_t hnd) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;
    if (hnd >= MAX_HANDLES)
        return ILLEGALHANDLE;
    if (Mod_Fs.handles[hnd].mode != OPENMODE_READ)
        return ILLEGALHANDLE;
    return SUCCESS;
}

fserr_t checkConditions_Write(handle_t hnd) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;
    if (hnd >= MAX_HANDLES)
        return ILLEGALHANDLE;
    if (Mod_Fs.handles[hnd].mode != OPENMODE_WRITE && Mod_Fs.handles[hnd].mode != OPENMODE_APPEND)
        return ILLEGALHANDLE;
    return SUCCESS;
}

fserr_t checkConditions_AnyOpen(handle_t hnd) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;
    if (hnd >= MAX_HANDLES)
        return ILLEGALHANDLE;
    if (Mod_Fs.handles[hnd].mode == OPENMODE_CLOSED)
        return ILLEGALHANDLE;
    if (!Mod_Fs.handles[hnd].isReal)
        return FILENOTFOUND;
    return SUCCESS;
}

fserr_t checkConditions_OpenFn(handle_t hnd) {
    if (Mod_Fs.refCount <= 0)
        return UNDEFINEDERROR;
    if (hnd >= MAX_HANDLES)
        return ILLEGALHANDLE;
    if (Mod_Fs.handles[hnd].mode == OPENMODE_CLOSED)
        return ILLEGALHANDLE;
    if (Mod_Fs.handles[hnd].mode != OPENMODE_REFERENCE)
        return FILEISBUSY;
    if (!Mod_Fs.handles[hnd].isReal)
        return FILENOTFOUND;
    fserr_t err = Hal_Fs_CheckHandleIsExclusive(hnd, false);
    if (err != SUCCESS)
        return err;
    return SUCCESS;
}

bool   Hal_Fs_CheckForbiddenFilename(const char *name) {
    // prevent
    // - filenames too long to otherwise represent
    // - crashes due to uninitialized buffers
    if (strnlen(name, FS_NAME_MAX_BYTES) > FS_NAME_MAX_CHARS)
        return false;

    // prevent simple directory escapes (via parent, root, ...)
    if (strstr(name, "/") != NULL)
        return false;

    // be nice to Win32
    if (strstr(name, "\\") != NULL)
        return false;

    // note: These are not proper defenses against malicious software.
    //       Never run any programs that you don't know are safe!
    return true;
}
