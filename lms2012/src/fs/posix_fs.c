#include <sys/stat.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <libgen.h>
#include "hal_filesystem.h"
#include "fs/hal_filesystem.private.h"
#include "fs/posix_fs.h"
#include "fs/posix_fs.private.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

bool posixFsInit(void) {
    // do not care about these errors
    if (mkdir(Mod_Fs.dataDir, 00755) < 0) {
        if (errno != EEXIST) {
            reportErrno("EV3 FS: cannot create data directory");
            return false;
        }
    } else {
        // fputs("EV3 FS: created data directory\n", stderr);
    }

    Mod_Fs.dataDirFd = open(Mod_Fs.dataDir, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (Mod_Fs.dataDirFd < 0) {
        reportErrno("EV3 FS: cannot open data directory");
        return false;
    }
    return true;
}

void posixFsExit(void) {
    if (Mod_Fs.dataDirFd >= 0) {
        if (close(Mod_Fs.dataDirFd) < 0) {
            reportErrno("EV3 FS: cannot close root data directory");
        }
        Mod_Fs.dataDirFd = -1;
    }
}

fserr_t posixFsLoadMeta(handle_data_t *pH) {
    FILE_META_V2 block  = {0, 0, 0};
    struct stat  stData = {0};
    int          fd;

    // set failure defaults
    pH->fullLength   = 0;
    pH->readPointer  = 0;
    pH->writePointer = 0;

    // check for invalid files
    if (!pH->isReal)
        return ILLEGALHANDLE;

    // get sane defaults

    if (fstatat(Mod_Fs.dataDirFd, pH->name, &stData, 0) < 0)
        return reportErrno("EV3 FS: cannot stat file to load its metadata");

    pH->fullLength   = stData.st_size;
    pH->writePointer = stData.st_size;

    // open meta block

    char *metaName = posixFsGetMetaName(pH->name);
    if (!metaName)
        return reportErrno("EV3 FS: cannot allocate memory");

    fd = openat(Mod_Fs.dataDirFd, metaName, O_RDONLY);
    free(metaName);
    metaName = NULL;
    if (fd < 0) {
        if (errno != ENOENT)
            return reportErrno("EV3 FS: cannot open meta file");
        goto skip;
    }

    // read block

    int bytes = pread(fd, &block, sizeof(block), 0);
    if (bytes < 0) {
        reportErrno("EV3 FS: cannot read meta block");
        goto skip;
    }
    if (bytes < sizeof(block))
        goto skip;
    if (block.Version != FILE_META_VERSION_2)
        goto skip;
    if (block.FullSize != pH->fullLength)
        goto skip;

    // everything went well, now we're ready
    pH->writePointer = MIN(block.WrittenSize, pH->fullLength);
skip:
    if (fd >= 0 && close(fd) < 0)
        reportErrno("EV3 FS: cannot close meta file");
    return SUCCESS;
}

fserr_t posixFsSaveMeta(handle_data_t *pH) {
    if (!pH->isReal)
        return ILLEGALFILENAME;

    FILE_META_V2 info = {
        .Version  = FILE_META_VERSION_2,
        .WrittenSize = pH->writePointer,
        .FullSize = pH->fullLength,
    };

    fserr_t result = SUCCESS;

    char *metaName = posixFsGetMetaName(pH->name);
    if (!metaName)
        return reportErrno("EV3 FS: cannot allocate memory");

    int fd = openat(Mod_Fs.dataDirFd, metaName, O_CREAT | O_TRUNC | O_WRONLY, 00644);
    free(metaName);
    metaName = NULL;
    if (fd < 0)
        return reportErrno("EV3 FS: cannot open metadata file");

    if (pwrite(fd, &info, sizeof(info), 0) < 0)
        result = reportErrno("EV3 FS: cannot write metadata block");

    if (fsync(fd) < 0)
        result = reportErrno("EV3 FS: cannot flush metadata");

    if (close(fd) < 0)
        result = reportErrno("EV3 FS: cannot close metadata file");

    return result;
}

extern fserr_t posixFsCreateWrite(handle_data_t *pH, const char *name, uint32_t fileSize) {
    strncpy(pH->name, name, FS_NAME_MAX_CHARS);
    pH->fullLength   = fileSize;
    pH->readPointer  = 0;
    pH->writePointer = 0;
    pH->isReal       = false;

    int fd = openat(Mod_Fs.dataDirFd, pH->name, O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC, 00644);
    if (fd < 0)
        return reportErrno("EV3 FS: cannot create file");

    if (ftruncate(fd, fileSize) < 0) {
        fserr_t err = reportErrno("EV3 FS: cannot set file size");
        close(fd);
        return err;
    }

    pH->linuxFd = fd;
    pH->isReal  = true;
    return SUCCESS;
}

fserr_t posixFsOpenRead(handle_data_t *pH) {
    pH->linuxFd = openat(Mod_Fs.dataDirFd, pH->name, O_RDONLY | O_CLOEXEC);
    if (pH->linuxFd < 0)
        return reportErrno("EV3 FS: cannot open file for reading");

    return SUCCESS;
}

fserr_t posixFsOpenAppend(handle_data_t *pH) {
    if (pH->writePointer >= pH->fullLength)
        return FILEISFULL;

    pH->linuxFd = openat(Mod_Fs.dataDirFd, pH->name, O_WRONLY | O_CLOEXEC);
    if (pH->linuxFd < 0)
        return reportErrno("EV3 FS: cannot open file for appending");

    return SUCCESS;
}

extern fserr_t posixFsReadAll(handle_data_t *pH) {
    if (pH->linuxFd < 0 || !pH->isReal)
        return ILLEGALHANDLE;

    if (pH->memCopy) {
        return SUCCESS;
    }

    pH->memCopy  = malloc(pH->fullLength);
    if (!pH->memCopy)
        return reportErrno("EV3 FS: cannot allocate memory for file copy");

    fserr_t err;
    size_t  read = 0;
    for (;;) {
        ssize_t now = pread(pH->linuxFd, pH->memCopy + read, pH->fullLength - read, read);

        if (now < 0 && errno == EINTR) {
            continue;
        } else if (now < 0) {
            err = reportErrno("EV3 FS: cannot read file to memory");
            goto error_cleanup;
        } else if (now == 0) {
            break;
        } else {
            read += now;
            if (read == pH->fullLength)
                break;
        }
    }

    if (read != pH->fullLength) {
        err = UNDEFINEDERROR;
        goto error_cleanup;
    }

    return SUCCESS;

error_cleanup:
    free(pH->memCopy);
    pH->memCopy = NULL;
    return err;
}

fserr_t posixFsRead(handle_data_t *pH, void *buffer, uint32_t *pLength) {
    if (pH->linuxFd < 0 || !pH->isReal)
        return ILLEGALHANDLE;

    uint32_t available = pH->writePointer - pH->readPointer;
    uint32_t request   = *pLength;
    uint32_t fulfill   = MIN(available, request);

    *pLength = 0;
    memset(buffer, 0, request);

    if (fulfill == 0) {
        return request == 0 ? SUCCESS : ENDOFFILE;
    }

    if (pH->memCopy) {
        memcpy(buffer, pH->memCopy + pH->readPointer, fulfill);

    } else {
        size_t done = 0;
        for (;;) {
            ssize_t now = pread(pH->linuxFd, buffer + done, fulfill - done, pH->readPointer + done);

            if (now < 0 && errno == EINTR) {
                continue;
            } else if (now < 0) {
                return reportErrno("EV3 FS: read failed");
            } else if (now == 0) {
                break;
            } else {
                done += now;
                if (done == fulfill)
                    break;
            }
        }

        if (done != fulfill)
            return UNDEFINEDERROR;
    }

    pH->readPointer += fulfill;
    *pLength = fulfill;

    return fulfill < request ? ENDOFFILE : SUCCESS;
}

fserr_t posixFsWrite(handle_data_t *pH, const void *buffer, uint32_t *pLength) {
    if (pH->linuxFd < 0 || !pH->isReal)
        return ILLEGALHANDLE;

    uint32_t available = pH->fullLength - pH->writePointer;
    uint32_t request   = *pLength;
    uint32_t fulfill   = MIN(available, request);

    *pLength = 0;

    if (fulfill == 0) {
        return request == 0 ? SUCCESS : EOFEXSPECTED;
    }

    size_t done = 0;
    for (;;) {
        ssize_t now = pwrite(pH->linuxFd, buffer + done, fulfill - done, pH->writePointer + done);

        if (now < 0 && errno == EINTR) {
            continue;
        } else if (now < 0) {
            return reportErrno("EV3 FS: read failed");
        } else if (now == 0) {
            break;
        } else {
            done += now;
            if (done == fulfill)
                break;
        }
    }

    if (done != fulfill)
        return UNDEFINEDERROR;

    pH->writePointer += fulfill;
    *pLength = fulfill;

    return fulfill < request ? EOFEXSPECTED : SUCCESS;
}

fserr_t posixFsSeekRead(handle_data_t *pH, int32_t offset, seek_t mode) {
    if (pH->linuxFd < 0 || !pH->isReal)
        return ILLEGALHANDLE;

    uint32_t newPointer;

    if (mode == SEEK_FROMSTART) {
        newPointer = offset;
    } else if (mode == SEEK_FROMCURRENT) {
        newPointer = pH->readPointer + offset;
    } else if (mode == SEEK_FROMEND) {
        newPointer = pH->writePointer + offset;
    } else {
        return INVALIDSEEK;
    }

    if (newPointer > pH->writePointer) {
        return INVALIDSEEK;
    }

    pH->readPointer = newPointer;
    return SUCCESS;
}

fserr_t posixFsTellRead(handle_data_t *pH, uint32_t *filePosition) {
    if (pH->linuxFd < 0 || !pH->isReal)
        return ILLEGALHANDLE;

    *filePosition = pH->readPointer;
    return SUCCESS;
}

fserr_t posixFsRemove(handle_data_t *pH) {
    if (!pH->isReal)
        return ILLEGALHANDLE;

    fserr_t err;
    if (pH->linuxFd >= 0) {
        err = posixFsClose(pH, false);
        if (FS_ISERR(err))
            return err;
    }

    pH->isReal = false;

    char *metaName = posixFsGetMetaName(pH->name);
    if (metaName) {
        int result = unlinkat(Mod_Fs.dataDirFd, metaName, 0);
        if (result < 0) {
            if (errno != ENOENT)
                reportErrno("EV3 FS: cannot delete metadata file");
        }
        free(metaName);
        metaName = NULL;
    }

    if (unlinkat(Mod_Fs.dataDirFd, pH->name, 0) < 0)
        return reportErrno("EV3 FS: cannot delete data file");

    return SUCCESS;
}

fserr_t posixFsMove(handle_data_t *pH, const char *name) {
    if (!pH->isReal)
        return ILLEGALHANDLE;

    const char *fail = NULL;

    char *oldMetaName = posixFsGetMetaName(pH->name);
    char *newMetaName = posixFsGetMetaName(name);

    if (!oldMetaName || !newMetaName) {
        fail = "EV3 FS: cannot allocate memory";
        goto end;
    }

    if (linkat(Mod_Fs.dataDirFd, pH->name, Mod_Fs.dataDirFd, name, 0) < 0) {
        fail = "EV3 FS: cannot make hardlink to data file";
        goto end;
    }

    if (linkat(Mod_Fs.dataDirFd, oldMetaName, Mod_Fs.dataDirFd, newMetaName, 0) < 0) {
        fail = "EV3 FS: cannot make hardlink to metadata file";
        goto end;
    }

    unlinkat(Mod_Fs.dataDirFd, pH->name, 0);
    unlinkat(Mod_Fs.dataDirFd, oldMetaName, 0);
    strncpy(pH->name, name, FS_NAME_MAX_CHARS);

end:
    free(oldMetaName);
    free(newMetaName);
    if (fail != NULL)
        return reportErrno(fail);
    return SUCCESS;
}

fserr_t posixFsShrink(handle_data_t *pH) {
    return posixFsResize(pH, pH->writePointer);
}

fserr_t posixFsResize(handle_data_t *pH, uint32_t newLength) {
    if (pH->linuxFd < 0 || !pH->isReal)
        return ILLEGALHANDLE;

    pH->fullLength = newLength;

    int err = ftruncate(pH->linuxFd, pH->fullLength);
    if (err < 0)
        reportErrno("EV3 FS: cannot set file size");

    return posixFsClose(pH, true);
}

fserr_t posixFsClose(handle_data_t *pH, bool saveMeta) {
    if (pH->linuxFd >= 0) {
        if (fsync(pH->linuxFd) < 0)
            reportErrno("EV3 FS: cannot flush data");
        if (close(pH->linuxFd) < 0)
            reportErrno("EV3 FS: cannot close data file descriptor");
        pH->linuxFd = -1;
    }

    if (pH->memCopy) {
        free(pH->memCopy);
        pH->memCopy = NULL;
    }

    if (pH->queryDir) {
        posixFsFinishBrowse(&pH->queryDir);
    }

    fserr_t result = saveMeta ? posixFsSaveMeta(pH) : SUCCESS;
    pH->fullLength   = 0;
    pH->writePointer = 0;
    pH->readPointer  = 0;
    return result;
}

fserr_t posixFsStartBrowse(DIR **pStream) {
    *pStream = NULL;

    DIR *stream = opendir(Mod_Fs.dataDir);
    if (stream == NULL)
        return reportErrno("EV3 FS: cannot (re)open root directory");

    *pStream = stream;
    return SUCCESS;
}

fserr_t posixFsBrowseNext(DIR *stream, struct dirent **ppDirent) {
    errno = 0;
    *ppDirent = readdir(stream);
    if (*ppDirent == NULL) {
        if (errno != 0) {
            return reportErrno("EV3 FS: cannot browse root directory");
        } else {
            return FILENOTFOUND;
        }
    }
    return SUCCESS;
}

void posixFsFinishBrowse(DIR **pStream) {
    if (*pStream) {
        if (closedir(*pStream) < 0)
            reportErrno("EV3 FS: cannot close root directory");
    }
    *pStream = NULL;
}

fserr_t posixFsGetFreeBytes(uint32_t *pAmount) {
    struct statvfs info;

    int err = fstatvfs(Mod_Fs.dataDirFd, &info);

    if (err < 0) {
        *pAmount = 0;
        return reportErrno("EV3 FS: cannot get free space");
    } else {
        *pAmount = info.f_bsize * info.f_bfree;
        return SUCCESS;
    }
}

fserr_t mapErrno(int error) {
    switch (error) {
    case 0:
        return SUCCESS;
    case ENOSPC:
        return NOSPACE;
    case ENOENT:
        return FILENOTFOUND;
    case ETXTBSY:
        return FILEISBUSY;
    case EINVAL:
        return ILLEGALFILENAME;
    case EEXIST:
        return FILEEXISTS;
    default:
        return UNDEFINEDERROR;
    }
}

fserr_t reportErrno(const char *message) {
    int error = errno;
    perror(message);
    errno     = error;
    return mapErrno(error);
}

bool posixFsGetDefaultDirs(char **pDataDir) {
    *pDataDir = NULL;

    char *selfPath = realpath("/proc/self/exe", NULL);
    if (!selfPath) {
        reportErrno("EV3 FS: cannot resolve self path");
        return false;
    }

    *pDataDir = dirname(selfPath);

    if (*pDataDir == NULL) {
        free(*pDataDir);
        *pDataDir = NULL;
        return false;
    } else {
        return true;
    }
}

char *posixFsGetMetaName(const char *dataName) {
    char *result = NULL;
    int  count   = asprintf(&result, ".%s.meta", dataName);
    return count < 0 ? NULL : result;
}
