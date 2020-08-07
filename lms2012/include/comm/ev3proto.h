#ifndef EV3PROTO
#define EV3PROTO

#include <stdbool.h>
#include <stdint.h>

#define COMMAND_NXT3_HOST_TO_DEV      0x90
#define COMMAND_NXT3_DEV_TO_HOST      0x91
#define COMMAND_EV3_SYS_REQUEST       0x01
#define COMMAND_EV3_SYS_REQUEST_QUIET 0x81
#define COMMAND_EV3_SYS_REPLY_OK      0x03
#define COMMAND_EV3_SYS_REPLY_ERROR   0x05
#define COMMAND_EV3_VM_REQUEST        0x00
#define COMMAND_EV3_VM_REQUEST_QUIET  0x80
#define COMMAND_EV3_VM_REPLY_OK       0x02
#define COMMAND_EV3_VM_REPLY_ERROR    0x04

#define MAX_PROTO_HANDLES 8
#define NO_PROTO_HANDLES  0xFF

typedef struct {
    uint8_t *buffer;
    int     *pLength;
} remotebuf_t;

typedef enum {
    HANDLE_CLOSED,
    HANDLE_RX,
} handle_mode_t;

typedef struct {
    uint32_t fileLength;
    uint32_t sentLength;
} download_state_t;

typedef union {
    download_state_t rx;
} handle_cb_t;

typedef struct {
    handle_mode_t mode;
    int           fd;
    handle_cb_t   state;
} ev3_handle_t;

typedef struct {
    int refCount;

    remotebuf_t rx;
    remotebuf_t tx;

    ev3_handle_t handles[MAX_PROTO_HANDLES];
} channel_t;

extern bool Ev3Proto_Init(channel_t *chan, remotebuf_t rx, remotebuf_t tx);
extern bool Ev3Proto_RefDel(channel_t *chan);
extern bool Ev3Proto_ConnEstablished(channel_t *chan);
extern bool Ev3Proto_ConnLost(channel_t *chan);
extern bool Ev3Proto_SystemCommand(channel_t *chan);

#endif //EV3PROTO
