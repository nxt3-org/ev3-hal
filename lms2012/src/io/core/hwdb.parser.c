#include <io/core/hwdb.private.h>
#include <ctype.h>
#include <memory.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <hal_general.h>

/////////////////////////
// TYPEDATA POPULATION //
/////////////////////////

void HwDb_Load(void) {
    DIR *dir = opendir(TYPEDATA_DIR);
    if (dir == NULL) {
        perror("Cannot open typedata directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG)
            continue;
        if (0 == strncmp(entry->d_name, TYPEDATA_PREFIX, TYPEDATA_PREFIX_CHARS)) {
            char *name;
            if (asprintf(&name, "%s/%s", TYPEDATA_DIR, entry->d_name) < 0)
                continue;
            HwDb_LoadFile(name);
            free(name);
        }
    }
    closedir(dir);
}

typedb_entry_t *HwDb_TakeSlot(void) {
    HwDb.count++;
    if (HwDb.count > HwDb.capacity) {
        if (HwDb.capacity == 0)
            HwDb.capacity = 1;
        else
            HwDb.capacity *= 2;

        HwDb.list = realloc(HwDb.list, HwDb.capacity * sizeof(typedb_entry_t));
        if (!HwDb.list)
            Hal_General_AbnormalExit("Cannot allocate typedata");
    }
    return &HwDb.list[HwDb.count - 1];
}

void HwDb_ReclaimSlot(void) {
    HwDb.count--;
}

void HwDb_LoadFile(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Cannot open typedata file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        int len = strlen(line);
        if (len == sizeof(line) - 1) {
            fprintf(stderr, "typedata line too long: %d chars", len);
            return;
        }

        if (line[len - 1] == '\n')
            line[len - 1] = '\0';
        HwDb_Parse(line);
    }

    if (fclose(file) != 0) {
        perror("Cannot close typedata file");
    }
}

static void skipws(const char **pPtr) {
    const char *pos = *pPtr;
    while (pos[0] != '\0' && isspace(pos[0])) {
        pos++;
    }
    *pPtr = pos;
}

static bool readU32(const char **pPtr, uint32_t *pNumber) {
    const char    *pos  = *pPtr;
    char          *end  = NULL;
    long long int value = strtoll(pos, &end, 0);
    if (pos != end) {
        if (value < 0 || value > UINT32_MAX)
            return false;
        *pPtr    = end;
        *pNumber = value;
        return true;
    }
    return false;
}

static bool readS32(const char **pPtr, int32_t *pNumber) {
    const char *pos  = *pPtr;
    char       *end  = NULL;
    long int   value = strtol(pos, &end, 0);
    if (pos != end) {
        *pPtr    = end;
        *pNumber = value;
        return true;
    }
    return false;
}

static bool readU8(const char **pPtr, uint8_t *pNumber) {
    int32_t full;
    if (!readS32(pPtr, &full)) return false;
    if (full < 0 || full > UINT8_MAX) return false;
    *pNumber = full;
    return true;
}

static bool readU16(const char **pPtr, uint16_t *pNumber) {
    int32_t full;
    if (!readS32(pPtr, &full)) return false;
    if (full < 0 || full > UINT16_MAX) return false;
    *pNumber = full;
    return true;
}

static bool readS16(const char **pPtr, int16_t *pNumber) {
    int32_t full;
    if (!readS32(pPtr, &full)) return false;
    if (full < INT16_MIN || full > INT16_MAX) return false;
    *pNumber = full;
    return true;
}

static bool readF32(const char **pPtr, float *pNumber) {
    const char *pos  = *pPtr;
    char       *end  = NULL;
    float      value = strtof(pos, &end);
    if (pos != end) {
        *pPtr    = end;
        *pNumber = value;
        return true;
    }
    return false;

}

static bool readStr(const char **pPtr, char *output, int32_t maxLen) {
    memset(output, 0, maxLen);
    skipws(pPtr);
    const char *start = *pPtr;

    int i;
    for (i    = 0; i < maxLen - 1; i++) {
        if (start[i] == '\0') break;
        if (isspace(start[i])) break;
        output[i] = start[i];
    }
    output[i] = '\0';

    *pPtr = start + i;
    return i > 0;
}

void HwDb_Parse(const char *line) {
    const char *pos = line;

    while (true) {
        skipws(&pos);

        if (strncmp(pos, "//", 2) == 0)
            return; // only a comment
        if (pos[0] == '*')
            return; // UART device
        if (pos[0] == '#')
            continue;
        break;
    }

    typedb_entry_t *entry = HwDb_TakeSlot();

    if (!readU8(&pos, &entry->Main.Device)) goto problem;
    if (!readU8(&pos, &entry->Main.Mode)) goto problem;
    if (!readStr(&pos, entry->Main.Name, SENSOR_NAME_BYTES)) goto problem;
    if (!readU8(&pos, &entry->Main.Values)) goto problem;
    if (!readU8(&pos, &entry->Main.InterpretAs)) goto problem;
    if (!readU8(&pos, &entry->Main.Digits)) goto problem;
    if (!readU8(&pos, &entry->Main.DecimalPlaces)) goto problem;
    if (!readU8(&pos, &entry->Main.GuiVisibleModes)) goto problem;
    if (!readU8(&pos, &entry->Main.Link)) goto problem;
    if (!readU8(&pos, (uint8_t *) &entry->Main.PinSetup)) goto problem;
    if (!readF32(&pos, &entry->Main.RawMin)) goto problem;
    if (!readF32(&pos, &entry->Main.RawMax)) goto problem;
    if (!readF32(&pos, &entry->Main.PercentMin)) goto problem;
    if (!readF32(&pos, &entry->Main.PercentMax)) goto problem;
    if (!readF32(&pos, &entry->Main.SiMin)) goto problem;
    if (!readF32(&pos, &entry->Main.SiMax)) goto problem;
    if (!readU16(&pos, &entry->Main.ModeswitchMsec)) goto problem;
    if (!readS16(&pos, &entry->Main.AdcAutoId)) goto problem;
    if (!readStr(&pos, entry->Main.Unit, UNIT_BYTES)) goto problem;

    if (entry->Main.Link == DCM_LINK_NXT_IIC) {
        if (!readStr(&pos, entry->Iic.IicManufacturer, IIC_NAME_BYTES)) goto problem;
        if (!readStr(&pos, entry->Iic.IicProduct, IIC_NAME_BYTES)) goto problem;
        if (!readU8(&pos, &entry->Iic.SetupLength)) goto problem;
        if (!readU32(&pos, &entry->Iic.SetupMsg)) goto problem;
        if (!readU8(&pos, &entry->Iic.PollLength)) goto problem;
        if (!readU32(&pos, &entry->Iic.PollMsg)) goto problem;
        if (!readU8(&pos, &entry->Iic.ReadLength)) goto problem;
    } else {
        memset(&entry->Iic, 0, sizeof(entry->Iic));
    }

    HwDb_Bugfix(entry);
    return;
problem:
    HwDb_ReclaimSlot();
    return;
}

void HwDb_Bugfix(typedb_entry_t *entry) {
    if (entry->Main.Device == DCM_DEV_EV3_COLOR) {
        if (entry->Main.Mode == PNP_MODE_EV3_COLOR_REF_RAW ||
            entry->Main.Mode == PNP_MODE_EV3_COLOR_RGB_RAW) {
            entry->Main.RawMax = 1023.0f;
            entry->Main.SiMax  = 1023.0f;
        }
        if (entry->Main.Mode == PNP_MODE_EV3_COLOR_CALIBRATE) {
            entry->Main.Values = 3; // fourth is always zero
        }
    }
}
