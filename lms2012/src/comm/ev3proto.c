#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <errno.h>
#include <bits/errno.h>
#include <unistd.h>
#include <memory.h>
#include <stdio.h>
#include "comm/ev3proto.h"
#include "comm/ev3proto.private.h"

bool Ev3Proto_Init(channel_t *chan, remotebuf_t rx, remotebuf_t tx) {
    chan->refCount = 1;
    chan->rx       = rx;
    chan->tx       = tx;

    for (int i = 0; i < MAX_PROTO_HANDLES; i++) {
        chan->handles[i].mode = HANDLE_CLOSED;
        chan->handles[i].fd   = -1;
        memset(&chan->handles[i].state, 0, sizeof(chan->handles[i].state));
    }
    return true;
}

bool Ev3Proto_RefDel(channel_t *chan) {
    if (chan->refCount == 0)
        return false;
    if (chan->refCount == 1) {
        // free resources
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
    if (inLen > 1024) return false;

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
        if (inLen < 11) return false;

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
        if (inLen < 7) return false;
        file_handle_t hnd     = inBuf[6];
        uint8_t       *data   = &inBuf[7];
        uint32_t      dataLen = inLen - 7;

        status  = Ev3Proto_ContinueRx(chan, data, dataLen, hnd);
        success = status == SYSSTATE_SUCCESS || status == SYSSTATE_EOF;
        resultBuf[0] = hnd;
        resultLen = 1;
        break;
    }
    case SYSCMD_BEGIN_TX: {
        break;
    }
    case SYSCMD_CONTINUE_TX: {
        break;
    }
    case SYSCMD_BEGIN_TXI: {
        break;
    }
    case SYSCMD_CONTINUE_TXI: {
        break;
    }
    case SYSCMD_CLOSE: {
        if (inLen < 7) return false;
        file_handle_t hnd = inBuf[6];
        status    = Ev3Proto_Close(chan, hnd);
        success   = status == SYSSTATE_SUCCESS;
        resultLen = 0;
        break;
    }
    case SYSCMD_BEGIN_LS: {
        break;
    }
    case SYSCMD_CONTINUE_LS: {
        break;
    }
    case SYSCMD_MKDIR: {
        if (inLen < 7) return false;
        inBuf[inLen - 1] = '\0';
        char *name = (char *) &inBuf[6];

        status    = Ev3Proto_Mkdir(name);
        success   = status == SYSSTATE_SUCCESS;
        resultLen = 0;
        break;
    }
    case SYSCMD_REMOVE: {
        if (inLen < 7) return false;
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

system_status_t Ev3Proto_BeginRx(channel_t *chan, uint32_t length, char *name, file_handle_t *hnd) {
    if (!Ev3Proto_RecursiveMkdir(name, NULL))
        goto fail;

    *hnd = Ev3Proto_FindFreeHandle(chan);
    if (*hnd == NO_PROTO_HANDLES) return SYSSTATE_OUT_OF_HANDLES;

    // unlink first
    // this should allow updating NXT3 from within NXT3
    if (unlink(name) < 0 && errno != ENOENT) goto fail;

    int fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 00777);
    if (fd < 0) goto fail;
    if (fchmod(fd, 00777) < 0) goto failCloseFd;
    if (ftruncate(fd, length) < 0) goto failCloseFd;

    ev3_handle_t *pH = &chan->handles[*hnd];
    pH->mode                = HANDLE_RX;
    pH->fd                  = fd;
    pH->state.rx.fileLength = length;
    pH->state.rx.sentLength = 0;
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

    uint32_t oldPos = pH->state.rx.sentLength;
    uint32_t newPos = oldPos + len;
    if (newPos < oldPos) newPos = 0xFFFFFFFF; // overflow
    uint32_t inputDelta  = newPos - oldPos;
    uint32_t outputDelta = pH->state.rx.fileLength - oldPos;

    uint32_t delta = inputDelta < outputDelta ? inputDelta : outputDelta;

    while (delta != 0) {
        int real = pwrite(pH->fd, data, delta, pH->state.rx.sentLength);
        if (real < 0) {
            if (errno == EINTR)
                continue;
            return Ev3Proto_Errno();
        }
        if (real > delta) real = delta;
        pH->state.rx.sentLength += real;
        delta -= real;
    }

    if (pH->state.rx.sentLength == pH->state.rx.fileLength) {
        fsync(pH->fd);
        close(pH->fd);
        pH->fd   = -1;
        pH->mode = HANDLE_CLOSED;
        return SYSSTATE_EOF;
    } else {
        return SYSSTATE_SUCCESS;
    }
}

system_status_t Ev3Proto_Close(channel_t *chan, file_handle_t hnd) {
    if (hnd >= MAX_PROTO_HANDLES) return SYSSTATE_UNKNOWN_HANDLE;
    ev3_handle_t *pH = &chan->handles[hnd];

    if (pH->mode == HANDLE_CLOSED) return SYSSTATE_UNKNOWN_HANDLE;

    if (pH->fd >= 0) {
        close(pH->fd);
        pH->fd = -1;
    }
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
