#ifndef HAL_ERRORCODES
#define HAL_ERRORCODES

#include <stdint.h>
#include <stdbool.h>

/* Error codes from then Loader */
typedef enum __attribute__((packed)) {
    SUCCESS             = 0x0000,
    INPROGRESS          = 0x0001,
    REQPIN              = 0x0002,
    NOMOREHANDLES       = 0x8100,
    NOSPACE             = 0x8200,
    NOMOREFILES         = 0x8300,
    EOFEXSPECTED        = 0x8400,
    ENDOFFILE           = 0x8500,
    NOTLINEARFILE       = 0x8600,
    FILENOTFOUND        = 0x8700,
    HANDLEALREADYCLOSED = 0x8800,
    NOLINEARSPACE       = 0x8900,
    UNDEFINEDERROR      = 0x8A00,
    FILEISBUSY          = 0x8B00,
    NOWRITEBUFFERS      = 0x8C00,
    APPENDNOTPOSSIBLE   = 0x8D00,
    FILEISFULL          = 0x8E00,
    FILEEXISTS          = 0x8F00,
    MODULENOTFOUND      = 0x9000,
    OUTOFBOUNDERY       = 0x9100,
    ILLEGALFILENAME     = 0x9200,
    ILLEGALHANDLE       = 0x9300,
    BTBUSY              = 0x9400,
    BTCONNECTFAIL       = 0x9500,
    BTTIMEOUT           = 0x9600,
    FILETX_TIMEOUT      = 0x9700,
    FILETX_DSTEXISTS    = 0x9800,
    FILETX_SRCMISSING   = 0x9900,
    FILETX_STREAMERROR  = 0x9A00,
    FILETX_CLOSEERROR   = 0x9B00,
    INVALIDSEEK         = 0x9C00
} error_t;

typedef uint16_t errhnd_t;
typedef uint8_t  handle_t;

#define FS_ERR(x) ((x) & 0xFF00)
#define FS_ISERR(x) (FS_ERR(x) != 0)
#define FS_HND(x) ((x) & 0x00FF)

#endif //HAL_ERRORCODES
