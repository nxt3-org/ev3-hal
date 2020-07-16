#ifndef EV3_KDEV_UI
#define EV3_KDEV_UI

#include <stdbool.h>

typedef struct {
    unsigned char isPressed[6];
} kdev_ui_mmap_t;

typedef struct {
    int refCount;
    int fd;
    volatile kdev_ui_mmap_t *mmap;
} kdev_ui_t;

extern kdev_ui_t Kdev_UI;

extern bool Kdev_UI_RefAdd(void);
extern bool Kdev_UI_RefDel(void);

#endif //EV3_KDEV_UI
