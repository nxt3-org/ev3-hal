#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <errno.h>
#include <bits/errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <comm/md5.h>
#include "comm/ev3proto.h"
#include "comm/ev3proto.private.h"

bool Ev3Proto_Init(channel_t *chan, remotebuf_t rx, remotebuf_t tx, int bufferCapacity) {
    chan->refCount    = 1;
    chan->rx          = rx;
    chan->tx          = tx;
    chan->bufCapacity = bufferCapacity;

    for (int i = 0; i < MAX_PROTO_HANDLES; i++) {
        chan->handles[i].mode       = HANDLE_CLOSED;
        chan->handles[i].fd         = -1;
        chan->handles[i].fileLength = 0;
        chan->handles[i].sentLength = 0;
    }
    return true;
}

bool Ev3Proto_RefDel(channel_t *chan) {
    if (chan->refCount == 0)
        return false;
    if (chan->refCount == 1) {
        Ev3Proto_ConnLost(chan);
    }
    chan->refCount--;
    return true;
}

bool Ev3Proto_ConnEstablished(channel_t *chan) {
    (void) chan;
    return true;
}

bool Ev3Proto_ConnLost(channel_t *chan) {
    for (int i = 0; i < MAX_PROTO_HANDLES; i++) {
        if (chan->handles[i].mode != HANDLE_CLOSED) {
            Ev3Proto_Close(chan, i);
        }
    }
    return true;
}

bool Ev3Proto_SystemCommand(channel_t *chan) {
    if (*chan->rx.pLength < 6) return false;

    uint8_t *inBuf   = chan->rx.buffer;
    uint8_t *outBuf  = chan->tx.buffer;
    int     *pOutLen = chan->tx.pLength;
    int     inLen    = 2 + (inBuf[1] << 8 | inBuf[0] << 0);
    if (inLen < 6) return false;
    if (inLen > *chan->rx.pLength) return false;

    int inCounter = inBuf[3] << 8 | inBuf[2] << 0;
    bool inQuiet;
    if (inBuf[4] == COMMAND_EV3_SYS_REQUEST)
        inQuiet = false;
    else if (inBuf[4] == COMMAND_EV3_SYS_REQUEST_QUIET)
        inQuiet = true;
    else
        return false;

    system_command_t inCmd = inBuf[5];

    uint8_t *resultBuf = &outBuf[7];
    int     resultLen  = 0;

    system_status_t status = SYSSTATE_UNKNOWN_ERROR;
    bool success = false;
    switch (inCmd) {
    default: // bye bye
        return false;

    case SYSCMD_BEGIN_RX: {
        if (inLen < 11) break;

        uint32_t fileLen = inBuf[9] << 24 | inBuf[8] << 16 | inBuf[7] << 8 | inBuf[6] << 0;
        inBuf[inLen - 1] = '\0';
        char *name = (char *) &inBuf[10];

        file_handle_t hnd;
        status  = Ev3Proto_BeginRx(chan, fileLen, name, &hnd);
        success = status == SYSSTATE_SUCCESS;
        resultBuf[0] = hnd;
        resultLen = 1;
        break;
    }
    case SYSCMD_CONTINUE_RX: {
        if (inLen < 7) break;
        file_handle_t hnd     = inBuf[6];
        uint8_t       *data   = &inBuf[7];
        uint32_t      dataLen = inLen - 7;

        status  = Ev3Proto_ContinueRx(chan, data, dataLen, hnd);
        success = status == SYSSTATE_SUCCESS || status == SYSSTATE_EOF;
        resultBuf[0] = hnd;
        resultLen = 1;
        break;
    }
    case SYSCMD_BEGIN_LS:
    case SYSCMD_BEGIN_TX:
    case SYSCMD_BEGIN_TXI: {
        if (inLen < 9) break;

        uint16_t thisRead = inBuf[7] << 8 | inBuf[6] << 0;
        inBuf[inLen - 1] = '\0';
        const char *name = (const char *) &inBuf[8];

        file_handle_t hnd     = 0;
        uint32_t      fileLen = 0;
        int           realOut = 0;
        if (inCmd == SYSCMD_BEGIN_LS)
            status = Ev3Proto_BeginLs(chan, name, thisRead, &fileLen, &hnd,
                                      &resultBuf[5], chan->bufCapacity - 12, &realOut);
        else
            status = Ev3Proto_BeginTx(chan, name, thisRead, &fileLen, &hnd,
                                      &resultBuf[5], chan->bufCapacity - 12, &realOut);
        success    = status == SYSSTATE_SUCCESS || status == SYSSTATE_EOF;
        resultBuf[0] = (fileLen >> 0) & 0xFF;
        resultBuf[1] = (fileLen >> 8) & 0xFF;
        resultBuf[2] = (fileLen >> 16) & 0xFF;
        resultBuf[3] = (fileLen >> 24) & 0xFF;
        resultBuf[4] = hnd;
        resultLen = 5 + realOut;
        break;
    }
    case SYSCMD_CONTINUE_LS:
    case SYSCMD_CONTINUE_TX: {
        if (inLen < 9) break;

        file_handle_t hnd      = inBuf[6];
        uint16_t      thisRead = inBuf[8] << 8 | inBuf[7] << 0;

        uint32_t fileLen = 0;
        int      realOut = 0;
        status  = Ev3Proto_ContinueTx(chan, hnd, thisRead, &fileLen,
                                      &resultBuf[1], chan->bufCapacity - 8, &realOut, true);
        success = status == SYSSTATE_SUCCESS || status == SYSSTATE_EOF;
        resultBuf[0] = hnd;
        resultLen = 1 + realOut;
        break;
    }
    case SYSCMD_CONTINUE_TXI: {
        if (inLen < 9) break;

        file_handle_t hnd      = inBuf[6];
        uint16_t      thisRead = inBuf[8] << 8 | inBuf[7] << 0;

        uint32_t fileLen = 0;
        int      realOut = 0;
        status  = Ev3Proto_ContinueTx(chan, hnd, thisRead, &fileLen,
                                      &resultBuf[5], chan->bufCapacity - 12, &realOut, true);
        success = status == SYSSTATE_SUCCESS || status == SYSSTATE_EOF;
        resultBuf[0] = (fileLen >> 0) & 0xFF;
        resultBuf[1] = (fileLen >> 8) & 0xFF;
        resultBuf[2] = (fileLen >> 16) & 0xFF;
        resultBuf[3] = (fileLen >> 24) & 0xFF;
        resultBuf[4] = hnd;
        resultLen = 5 + realOut;
        break;
    }
    case SYSCMD_CLOSE: {
        if (inLen < 7) break;
        file_handle_t hnd = inBuf[6];
        status            = Ev3Proto_Close(chan, hnd);
        success           = status == SYSSTATE_SUCCESS;
        resultLen         = 0;
        break;
    }
    case SYSCMD_MKDIR: {
        if (inLen < 7) break;
        inBuf[inLen - 1] = '\0';
        char *name       = (char *) &inBuf[6];

        status    = Ev3Proto_Mkdir(name);
        success   = status == SYSSTATE_SUCCESS;
        resultLen = 0;
        break;
    }
    case SYSCMD_REMOVE: {
        if (inLen < 7) break;
        inBuf[inLen - 1] = '\0';
        char *name = (char *) &inBuf[6];

        status    = Ev3Proto_Unlink(name);
        success   = status == SYSSTATE_SUCCESS;
        resultLen = 0;
        break;
    }
    }


    // forget written data
    if (inQuiet) {
        *pOutLen = 0;
    } else {
        int fullLength = resultLen + 7;
        outBuf[0] = ((fullLength - 2) >> 0) & 0xFF;
        outBuf[1] = ((fullLength - 2) >> 8) & 0xFF;
        outBuf[2] = (inCounter >> 0) & 0xFF;
        outBuf[3] = (inCounter >> 8) & 0xFF;
        outBuf[4] = success ? COMMAND_EV3_SYS_REPLY_OK : COMMAND_EV3_SYS_REPLY_ERROR;
        outBuf[5] = inCmd;
        outBuf[6] = status;
        *pOutLen = fullLength;
    }
    return true;
}

system_status_t Ev3Proto_BeginRx(channel_t *chan, uint32_t length, char *name, file_handle_t *pHnd) {
    if (!Ev3Proto_RecursiveMkdir(name, NULL))
        goto fail;

    *pHnd = Ev3Proto_FindFreeHandle(chan);
    if (*pHnd == NO_PROTO_HANDLES) return SYSSTATE_OUT_OF_HANDLES;

    // unlink first
    // this should allow updating NXT3 from within NXT3
    if (unlink(name) < 0 && errno != ENOENT) goto fail;

    int fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 00777);
    if (fd < 0) goto fail;
    if (fchmod(fd, 00777) < 0) goto failCloseFd;
    if (ftruncate(fd, length) < 0) goto failCloseFd;

    ev3_handle_t *pH = &chan->handles[*pHnd];
    pH->mode       = HANDLE_RX;
    pH->fd         = fd;
    pH->fileLength = length;
    pH->sentLength = 0;
    return SYSSTATE_SUCCESS;

failCloseFd:;
    int error = errno;
    close(fd);
    errno = error;
fail:
    return Ev3Proto_Errno();
}

system_status_t Ev3Proto_ContinueRx(channel_t *chan, uint8_t *data, uint32_t len, file_handle_t hnd) {
    ev3_handle_t *pH = Ev3Proto_GetHandle(chan, hnd, HANDLE_RX);
    if (pH == NULL) return SYSSTATE_UNKNOWN_HANDLE;

    // calculate delta
    uint32_t oldPos = pH->sentLength;
    uint32_t newPos = oldPos + len;
    if (newPos < oldPos) newPos = 0xFFFFFFFF; // overflow
    uint32_t inputDelta  = newPos - oldPos;
    uint32_t outputDelta = pH->fileLength - oldPos;
    uint32_t delta       = inputDelta < outputDelta ? inputDelta : outputDelta;

    // write loop
    while (delta != 0) {
        int real = pwrite(pH->fd, data, delta, pH->sentLength);
        if (real < 0) {
            if (errno == EINTR)
                continue;
            return Ev3Proto_Errno();
        }
        if (real > delta) real = delta;
        pH->sentLength += real;
        delta -= real;
        data += real;
    }

    // finalize
    if (pH->sentLength == pH->fileLength) {
        fsync(pH->fd);
        close(pH->fd);
        pH->fd   = -1;
        pH->mode = HANDLE_CLOSED;
        return SYSSTATE_EOF;
    } else {
        return SYSSTATE_SUCCESS;
    }
}

system_status_t Ev3Proto_BeginTx(channel_t *chan,
                                 const char *name, uint16_t thisRead,
                                 uint32_t *pLength, file_handle_t *pHnd,
                                 uint8_t *outBuffer, int outMaxLen, int *realOutLen) {
    *pHnd = Ev3Proto_FindFreeHandle(chan);
    if (*pHnd == NO_PROTO_HANDLES) return SYSSTATE_OUT_OF_HANDLES;

    int fd = open(name, O_RDONLY);
    if (fd < 0)
        return Ev3Proto_Errno();

    ev3_handle_t *pH = &chan->handles[*pHnd];
    pH->mode       = HANDLE_TX;
    pH->fd         = fd;
    pH->fileLength = 0;
    pH->sentLength = 0;

    return Ev3Proto_ContinueTx(chan, *pHnd, thisRead, pLength, outBuffer, outMaxLen, realOutLen, true);
}

system_status_t Ev3Proto_ContinueTx(channel_t *chan,
                                    file_handle_t hnd, uint16_t thisRead,
                                    uint32_t *pLength,
                                    uint8_t *outBuffer, int outMaxLen, int *realOutLen,
                                    bool closeOnEof) {
    if (thisRead > outMaxLen) thisRead = outMaxLen;

    ev3_handle_t *pH = Ev3Proto_GetHandle(chan, hnd, HANDLE_TX);
    if (pH == NULL) return SYSSTATE_UNKNOWN_HANDLE;

    // get real size
    struct stat info;
    if (fstat(pH->fd, &info) < 0)
        return Ev3Proto_Errno();
    if (!S_ISREG(info.st_mode))
        return SYSSTATE_CORRUPT_FILE; // symlinks not supported
    pH->fileLength = *pLength = info.st_size;

    // calculate delta
    uint32_t oldPos = pH->sentLength;
    uint32_t newPos = oldPos + thisRead;
    if (newPos < oldPos) newPos = 0xFFFFFFFF; // overflow
    uint32_t inputDelta  = newPos - oldPos;
    uint32_t outputDelta = pH->fileLength - oldPos;
    uint32_t delta       = inputDelta < outputDelta ? inputDelta : outputDelta;

    *realOutLen = delta;

    // read loop
    while (delta != 0) {
        int real = pread(pH->fd, outBuffer, delta, pH->sentLength);
        if (real < 0) {
            if (errno == EINTR)
                continue;
            return Ev3Proto_Errno();
        }
        if (real > delta) real = delta;
        pH->sentLength += real;
        delta -= real;
        outBuffer += real;
    }

    // finalize
    if (pH->sentLength == pH->fileLength) {
        if (closeOnEof) {
            fsync(pH->fd);
            close(pH->fd);
            pH->fd   = -1;
            pH->mode = HANDLE_CLOSED;
        }
        return SYSSTATE_EOF;
    } else {
        return SYSSTATE_SUCCESS;
    }
}

system_status_t
Ev3Proto_BeginLs(channel_t *chan, const char *name, uint16_t thisRead,
                 uint32_t *pLength, file_handle_t *pHnd,
                 uint8_t *outBuffer, int outMaxLen, int *realOutLen) {
    int  fd         = -1;
    FILE *fp        = NULL;
    char filename[] = "/tmp/nxt3-ls.XXXXXX";

    *pHnd = Ev3Proto_FindFreeHandle(chan);
    if (*pHnd == NO_PROTO_HANDLES) return SYSSTATE_OUT_OF_HANDLES;

    fd = mkstemp(filename);
    if (fd < 0) return Ev3Proto_Errno();
    if (unlink(filename) < 0) goto fail;

    fp = fdopen(dup(fd), "w+");
    if (fp == NULL) goto fail;
    if (!Ev3Proto_FillLs(name, fp)) goto fail;
    fclose(fp);
    if (lseek(fd, 0, SEEK_SET) < 0) goto fail;

    ev3_handle_t *pH = &chan->handles[*pHnd];
    pH->mode       = HANDLE_TX;
    pH->fd         = fd;
    pH->fileLength = 0;
    pH->sentLength = 0;

    return Ev3Proto_ContinueTx(chan, *pHnd, thisRead, pLength, outBuffer, outMaxLen, realOutLen, false);
fail:
    if (fp) fclose(fp);
    if (fd >= 0) close(fd);
    return Ev3Proto_Errno();
}

bool Ev3Proto_FillLs(const char *path, FILE *fp) {
    DIR *dir = opendir(path);
    if (!dir) goto fail;

    int error;
    errno = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char *fullPath;
        int  ch = asprintf(&fullPath, "%s/%s", path, entry->d_name);
        if (ch < 0) goto fail;

        // success/failure of one entry is not interesting - better list other files
        if (entry->d_type == DT_REG) {
            Ev3Proto_LsFile(entry->d_name, fullPath, fp);
        } else if (entry->d_type == DT_LNK || entry->d_type == DT_DIR) {
            Ev3Proto_LsDir(entry->d_name, fp);
        } else {
            Ev3Proto_LsOther(entry->d_name, fp);
        }
        free(fullPath);
        errno   = 0;
    }
    if (errno != 0) goto fail;
    closedir(dir);
    dir = NULL;
    return true;

fail:
    error = errno;
    if (dir) closedir(dir);
    errno    = error;
    return false;
}

bool Ev3Proto_LsFile(const char *entry, const char *fullPath, FILE *fp) {
    uint8_t md5buf[16];
    uint8_t buffer[1024];
    MD5_CTX md5ctx;
    memset(md5buf, 0, 16);

    // proc has magical regular files that block when read
    if (strncmp(fullPath, "/proc", 5) != 0) {
        MD5_Init(&md5ctx);

        FILE *inp = fopen(fullPath, "r");
        if (!inp) return false;
        while (true) {
            int count = fread(buffer, 1, 1024, inp);
            MD5_Update(&md5ctx, buffer, count);
            if (count < 1024) break;
        }
        bool fail = ferror(inp);
        fclose(inp);
        if (fail) return false;
        MD5_Final(md5buf, &md5ctx);
    }

    struct stat info;
    if (stat(fullPath, &info) < 0) return false;

    fprintf(fp, "%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX"
                "%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX"
                " %08llX %s\n",
            md5buf[0], md5buf[1], md5buf[2], md5buf[3], md5buf[4], md5buf[5], md5buf[6], md5buf[7],
            md5buf[8], md5buf[9], md5buf[10], md5buf[11], md5buf[12], md5buf[13], md5buf[14], md5buf[15],
            info.st_size, entry);
    return true;
}

bool Ev3Proto_LsDir(const char *entry, FILE *fp) {
    fprintf(fp, "%s/\n", entry);
    return true;
}

bool Ev3Proto_LsOther(const char *entry, FILE *fp) {
    fprintf(fp, "00000000000000000000000000000000 00000000 %s\n", entry);
    return true;
}

system_status_t Ev3Proto_Close(channel_t *chan, file_handle_t hnd) {
    if (hnd >= MAX_PROTO_HANDLES) return SYSSTATE_UNKNOWN_HANDLE;
    ev3_handle_t *pH = &chan->handles[hnd];

    if (pH->mode == HANDLE_CLOSED) return SYSSTATE_UNKNOWN_HANDLE;

    if (pH->fd >= 0) {
        close(pH->fd);
        pH->fd = -1;
    }

    pH->mode = HANDLE_CLOSED;
    return SYSSTATE_SUCCESS;
}

system_status_t Ev3Proto_Mkdir(char *name) {
    if (!Ev3Proto_RecursiveMkdir(name, NULL))
        return Ev3Proto_Errno();

    if (mkdir(name, 00777) < 0)
        return Ev3Proto_Errno();

    return SYSSTATE_SUCCESS;
}

system_status_t Ev3Proto_Unlink(char *name) {
    if (remove(name) < 0)
        return Ev3Proto_Errno();

    return SYSSTATE_SUCCESS;
}

file_handle_t Ev3Proto_FindFreeHandle(channel_t *chan) {
    for (int i = 0; i < MAX_PROTO_HANDLES; i++) {
        if (chan->handles[i].mode == HANDLE_CLOSED) {
            return i;
        }
    }
    return NO_PROTO_HANDLES;
}

ev3_handle_t *Ev3Proto_GetHandle(channel_t *chan, file_handle_t hnd, handle_mode_t required) {
    if (hnd >= MAX_PROTO_HANDLES) return false;
    if (chan->handles[hnd].mode == required)
        return &chan->handles[hnd];
    return NULL;
}

bool Ev3Proto_RecursiveMkdir(char *path, char *start) {
    if (start == NULL) start = path;

    char *nextSlash = start;
    while (nextSlash[0] == '/') nextSlash++;
    while (nextSlash[0] != '/' && nextSlash[0] != '\0') nextSlash++;

    // this is the final part
    if (nextSlash[0] == '\0') return true;

    nextSlash[0] = '\0';
    if (mkdir(path, 00777) < 0 && errno != EEXIST) return false;
    nextSlash[0] = '/';
    return Ev3Proto_RecursiveMkdir(path, nextSlash + 1);
}

system_status_t Ev3Proto_Errno(void) {
    switch (errno) {
    case EEXIST:
        return SYSSTATE_FILE_EXISTS;
    case ENOENT:
        return SYSSTATE_UNKNOWN_HANDLE;
    case ETXTBSY:
        return SYSSTATE_HANDLE_BUSY;
    case EPERM:
    case EACCES:
        return SYSSTATE_PERM_DENIED;
    case EISDIR:
    case ENOTDIR:
        return SYSSTATE_ILLEGAL_PATH;
    case ENAMETOOLONG:
        return SYSSTATE_ILLEGAL_FILENAME;
    case ENOSPC:
        return SYSSTATE_BAD_SIZE;
    default:
        return SYSSTATE_UNKNOWN_ERROR;
    }
}
