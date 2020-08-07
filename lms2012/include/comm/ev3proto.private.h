#ifndef EV3PROTO_PRIVATE
#define EV3PROTO_PRIVATE

typedef enum {
    SYSCMD_BEGIN_RX     = 0x92,
    SYSCMD_CONTINUE_RX  = 0x93,
    SYSCMD_BEGIN_TX     = 0x94,
    SYSCMD_CONTINUE_TX  = 0x95,
    SYSCMD_BEGIN_TXI    = 0x96,
    SYSCMD_CONTINUE_TXI = 0x97,
    SYSCMD_CLOSE        = 0x98,
    SYSCMD_BEGIN_LS     = 0x99,
    SYSCMD_CONTINUE_LS  = 0x9A,
    SYSCMD_MKDIR        = 0x9B,
    SYSCMD_REMOVE       = 0x9C,
} system_command_t;

typedef enum {
    SYSSTATE_SUCCESS            = 0x00,
    SYSSTATE_UNKNOWN_HANDLE     = 0x01,
    SYSSTATE_HANDLE_BUSY        = 0x02,
    SYSSTATE_CORRUPT_FILE       = 0x03,
    SYSSTATE_OUT_OF_HANDLES     = 0x04,
    SYSSTATE_PERM_DENIED        = 0x05,
    SYSSTATE_ILLEGAL_PATH       = 0x06,
    SYSSTATE_FILE_EXISTS        = 0x07,
    SYSSTATE_EOF                = 0x08,
    SYSSTATE_BAD_SIZE           = 0x09,
    SYSSTATE_UNKNOWN_ERROR      = 0x0A,
    SYSSTATE_ILLEGAL_FILENAME   = 0x0B,
    SYSSTATE_ILLEGAL_CONNECTION = 0x0C,
} system_status_t;

typedef uint8_t file_handle_t;

extern system_status_t Ev3Proto_BeginRx(channel_t *chan, uint32_t length, char *name, file_handle_t *pHnd);
extern system_status_t Ev3Proto_ContinueRx(channel_t *chan, uint8_t *data, uint32_t len, file_handle_t hnd);

extern system_status_t Ev3Proto_BeginTx(channel_t *chan, const char *name, uint16_t thisRead, uint32_t *pLength, file_handle_t *pHnd, uint8_t *outBuffer, int outMaxLen, int *realOutLen);
extern system_status_t Ev3Proto_ContinueTx(channel_t *chan, file_handle_t hnd, uint16_t thisRead, uint32_t *pLength, uint8_t *outBuffer, int outMaxLen, int *realOutLen);

extern system_status_t Ev3Proto_Close(channel_t *chan, file_handle_t hnd);
extern system_status_t Ev3Proto_Mkdir(char *name);
extern system_status_t Ev3Proto_Unlink(char *name);

extern file_handle_t Ev3Proto_FindFreeHandle(channel_t *chan);
extern ev3_handle_t *Ev3Proto_GetHandle(channel_t *chan, file_handle_t hnd, handle_mode_t required);
extern bool Ev3Proto_RecursiveMkdir(char *path, char *start);
extern system_status_t Ev3Proto_Errno(void);


#endif //EV3PROTO_PRIVATE
