#include <math.h>
#include <common/kdevices.h>
#include <io/driver/dcm.h>
#include <io/driver/if_analog.private.h>

drv_analog_t Drv_Analog;

static bool RefAdd(void) {
    if (Drv_Analog.refCount > 0) {
        Drv_Analog.refCount++;
        return true;
    }

    if (!Dcm_RefAdd())
        return false;

    if (!Kdev_RefAdd(&DeviceAnalog)) {
        Dcm_RefDel();
        return false;
    }

    for (int i = 0; i < 4; i++) {
        Drv_Analog.ports[i].analog  = NULL;
        Drv_Analog.ports[i].state   = ANALOG_OFF;
        Drv_Analog.ports[i].timer   = 0;
        Drv_Analog.ports[i].booting = false;
    }

    Drv_Analog.refCount++;
    return true;
}

static bool RefDel(void) {
    if (Drv_Analog.refCount == 0)
        return false;
    if (Drv_Analog.refCount == 1) {
        Kdev_RefDel(&DeviceAnalog);
        Dcm_RefDel();
    }
    Drv_Analog.refCount--;
    return true;
}

static bool Present(int port) {
    if (port >= 4) return false;
    return Drv_Analog.ports[port].state != ANALOG_OFF;
}

static bool Ready(int port) {
    if (port >= 4) return false;
    return Drv_Analog.ports[port].state == ANALOG_READY;
}

static bool PnpStart(int port, dcm_link_t link, dcm_type_t type) {
    if (port >= 4) return false;
    const typedb_entry_t *analog;
    if (type == DCM_DEV_UNKNOWN) {
        uint16_t mV = Dcm_MeasureAutoID(port, false);
        analog = HwDb_FindDefaultForAutoID(link, mV);
    } else {
        analog = HwDb_FindDefaultForType(link, type);
    }

    if (!analog)
        return false;

    pnp_type_t pnpDev = IdentifyDevice(analog->Main.Device);
    if (pnpDev == PNP_DEVICE_UNKNOWN || pnpDev == PNP_DEVICE_NONE)
        return false;

    Dcm_SetPins(port, analog->Main.PinSetup);
    Drv_Analog.ports[port].analog  = analog;
    Drv_Analog.ports[port].timer   = analog->Main.ModeswitchMsec;
    Drv_Analog.ports[port].state   = ANALOG_WAITING;
    Drv_Analog.ports[port].booting = true;
    return true;
}

static void PnpStop(int port) {
    if (port >= 4) return;

    Drv_Analog.ports[port].analog = NULL;
    Drv_Analog.ports[port].timer  = 0;
    Drv_Analog.ports[port].state  = ANALOG_OFF;
    Drv_Analog.ports[port].booting = true;
    Dcm_SetPins(port, 'f');
}

static bool Switch(int port, int hwMode) {
    if (!Ready(port)) return false;

    const typedb_entry_t *cur, *new;

    cur = Drv_Analog.ports[port].analog;
    if (cur->Main.Mode == hwMode)
        return true;

    new = HwDb_FindForMode(cur->Main.Link, cur->Main.Device, hwMode);
    if (!new)
        return false;

    Dcm_SetPins(port, new->Main.PinSetup);
    Drv_Analog.ports[port].analog = new;
    Drv_Analog.ports[port].timer  = new->Main.ModeswitchMsec;
    Drv_Analog.ports[port].state  = ANALOG_WAITING;
    return true;
}

static void Tick(void) {
    for (int i = 0; i < 4; i++) {
        analog_port_t *port = &Drv_Analog.ports[i];

        if (port->state != ANALOG_WAITING)
            continue;

        if (port->timer <= 0) {
            port->state = ANALOG_READY;
            DriverAnalog.Sensor.ResetDatalog(i);
            if (port->booting) {
                port->booting = false;
                Hal_Pnp_HandshakeFinished(i, false, IdentifyDevice(port->analog->Main.Device));
            }
        } else {
            port->timer--;
            port->state = ANALOG_WAITING;
        }
    }
}

static pnp_type_t GetDevice(int port) {
    if (!Present(port)) return PNP_DEVICE_NONE;

    return IdentifyDevice(Drv_Analog.ports[port].analog->Main.Device);
}

static const char *GetDeviceName(int port) {
    if (!Present(port)) return NULL;

    return Drv_Analog.ports[port].analog->Main.Name;
}

static int GetMode(int port) {
    if (!Present(port)) return -1;

    return Drv_Analog.ports[port].analog->Main.Mode;
}

static int GetModes(int port) {
    if (!Present(port)) return -1;

    return Drv_Analog.ports[port].analog->Main.GuiVisibleModes;
}

static const char *GetModeName(int port, int mode) {
    if (!Present(port)) return NULL;
    if (mode >= 8) return NULL;

    const typedata_t *info = &Drv_Analog.ports[port].analog->Main;

    if (mode == info->Mode)
        return info->Name;

    const typedb_entry_t *entry = HwDb_FindForMode(info->Link, info->Device, mode);
    if (entry == NULL)
        return NULL;
    return entry->Main.Name;
}

static const char *GetSiUnit(int port) {
    if (!Present(port)) return NULL;

    return Drv_Analog.ports[port].analog->Main.Unit;
}

bool Measure(int port, float *out, int slots, unit_type_t msr, bool clip) {
    if (!Ready(port)) return false;
    if (slots == 0) return true;

    const typedata_t *info = &Drv_Analog.ports[port].analog->Main;

    int   index = DeviceAnalog.mmap->Buffer_LastPtr[port];
    float in    = NAN;

    if (info->Link == DCM_LINK_INPUT_DUMB)
        in = DeviceAnalog.mmap->Buffer_Pin6_S[port][index];
    else
        in = DeviceAnalog.mmap->Buffer_Pin1_S[port][index];

    Transform(&in, out, 1, info, msr, clip);

    for (int i = 1; i < slots; i++) {
        out[i] = NAN;
    }
    return true;
}

static bool ResetDatalog(int port) {
    if (!Ready(port)) return false;

    DeviceAnalog.mmap->Buffer_ReadPtr[port] = DeviceAnalog.mmap->Buffer_LastPtr[port];
    return true;
}

int ReadDatalog(int port, int *pRepeat, float *si, float *pct, float *raw, int slots, bool clip) {
    if (!Ready(port)) return -1;
    const typedata_t *info = &Drv_Analog.ports[port].analog->Main;

    if (slots < 1)
        return 0;

    // end of read
    int read  = DeviceAnalog.mmap->Buffer_ReadPtr[port];
    int write = DeviceAnalog.mmap->Buffer_WritePtr[port];
    if (read == write) {
        *pRepeat = 0;
        return 0;
    } else {
        *pRepeat = 1;
    }

    // advance read pointer
    int nextRead = read + 1;
    if (nextRead >= KERNEL_DATALOG_ENTRIES)
        nextRead = 0;
    DeviceAnalog.mmap->Buffer_ReadPtr[port] = nextRead;

    float in;

    if (info->Link == DCM_LINK_INPUT_DUMB)
        in = DeviceAnalog.mmap->Buffer_Pin6_S[port][read];
    else
        in = DeviceAnalog.mmap->Buffer_Pin1_S[port][read];

    TransformBlock(&in, raw, pct, si, 1, slots, info, clip);

    return 1;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedLocalVariable"

static bool StartCommand(int port, const uint8_t *cmd, int count) {
    return false;
}

#pragma clang diagnostic pop

interface_t DriverAnalog = {
    .Init = RefAdd,
    .Exit = RefDel,
    .Tick = Tick,

    .Ready = Ready,
    .Present = Present,

    .GetDevice = GetDevice,
    .GetDeviceName = GetDeviceName,

    .Start = PnpStart,
    .Stop = PnpStop,

    .Motor = {},
    .Sensor = {
        .StartSwitch  = Switch,
        .StartCommand = StartCommand,
        .GetMode      = GetMode,
        .GetModes     = GetModes,
        .GetModeName  = GetModeName,
        .GetSiUnit    = GetSiUnit,
        .Measure      = Measure,
        .ResetDatalog = ResetDatalog,
        .ReadDatalog  = ReadDatalog
    },
};
