#include "hal_filesystem.h"
#include <memory.h>

void Hal_Fs_ParseQuery(const char *query, search_query_t *pResult) {
    memset(pResult->content, '\0', FS_NAME_MAX_BYTES);

    if (!Hal_Fs_CheckForbiddenFilename(query)) {
        // problem: too long query/uninitialized buffer
        pResult->lookFor = SEARCH_FILENAME;
        strncpy(pResult->content, "__uninitialized.txt", FS_NAME_MAX_CHARS);
        return;
    }

    /* match-all wildcard */
    if (0 != strstr(query, "*.*")) {
        pResult->lookFor = SEARCH_EVERYTHING;
        return;
    }

    /* extension wildcard */
    const char *dotExt = strstr(query, ".*");
    if (dotExt != NULL) {
        pResult->lookFor = SEARCH_FILENAME_WITHOUT_EXT;
        strncpy(pResult->content, query, dotExt - query + 1);
        return;
    }

    /* filename wildcard */
    const char *fileDot = strstr(query, "*.");
    if (fileDot != NULL) {
        pResult->lookFor = SEARCH_EXTENSION;
        strncpy(pResult->content, fileDot + 1, FS_NAME_MAX_CHARS);
        return;
    }

    /* no wildcards used */
    pResult->lookFor = SEARCH_FILENAME;
    strncpy(pResult->content, query, FS_NAME_MAX_CHARS);
}

error_t Hal_Fs_CheckQuery(const char *test, search_query_t *pQuery) {
    bool found;

    if (!Hal_Fs_CheckForbiddenFilename(test)) {
        // filename too long / uninitialized buffer
        return ILLEGALFILENAME;
    }

    switch (pQuery->lookFor) {
    case SEARCH_FILENAME:
        found = (0 == strcasecmp(test, pQuery->content));
        break;
    case SEARCH_FILENAME_WITHOUT_EXT:
        found = (0 == strncasecmp(test, pQuery->content, strlen(pQuery->content)));
        break;
    case SEARCH_EXTENSION:
        found = (NULL != strstr(test, pQuery->content));
        break;
    case SEARCH_EVERYTHING:
        found = true;
        break;
    default:
        return ILLEGALFILENAME;
    }

    if (found)
        return SUCCESS;
    else
        return UNDEFINEDERROR;
}

bool   Hal_Fs_CheckForbiddenFilename(const char *name) {
    if (name == NULL)
        return false;
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
