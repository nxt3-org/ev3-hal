#ifndef HWDB_PRIVATE
#define HWDB_PRIVATE

#include "hwdb.h"

#define TYPEDATA_DIR "/home/root/lms2012/sys/settings"
#define TYPEDATA_PREFIX "typedata"
#define TYPEDATA_PREFIX_CHARS 8
#define TYPEDATA_DEFAULT_COUNT 64

typedef struct {
    int            refCount;
    typedb_entry_t *list;
    uint32_t       capacity;
    uint32_t       count;
    adapter_info_t *adapters;
    uint32_t       adapterCount;
} hwdb_t;

extern typedb_entry_t *HwDb_TakeSlot(void);
extern void HwDb_ReclaimSlot(void);

extern void HwDb_Load(void);
extern void HwDb_LoadFile(const char *path);
extern void HwDb_Parse(const char *line);

extern hwdb_t HwDb;

#endif //HWDB_PRIVATE
