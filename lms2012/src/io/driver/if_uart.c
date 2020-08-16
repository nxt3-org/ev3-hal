#include <stdio.h>
#include <memory.h>
#include <math.h>
#include "io/core/hal_pnp.local.h"
#include "io/driver/if_uart.private.h"

drv_uart_t Drv_Uart;

static const typedata_t *GetCurrentTypedata(int port);
static const typedata_t *GetTypedata(int port, int mode);

static void ProbeAndGo(int port);
static void GoReboot(int port);
static void GoSwitch(int port);
static void GoWait(int port);
static void GoOff(int port);

static bool RefAdd(void) {
    if (Drv_Uart.refCount > 0) {
        Drv_Uart.refCount++;
        return true;
    }

    if (!Kdev_RefAdd(&DeviceAnalog))
        return false;

    memset(Drv_Uart.types, 0, sizeof(Drv_Uart.types));
    for (int no = 0; no < 4; no++) {
        Drv_Uart.state[no]       = UART_OFF;
        Drv_Uart.timer[no]       = 0;
        Drv_Uart.booting[no]     = false;
        Drv_Uart.devmap.link[no] = DeviceAnalog.mmap->Dcm_InLink[no];
        Drv_Uart.devmap.type[no] = DCM_DEV_UNKNOWN;
        Drv_Uart.devmap.mode[no] = 0;
    }
    if (!Kdev_RefDel(&DeviceAnalog))
        return false;

    if (!Kdev_RefAdd(&DeviceUart))
        return false;
    if (!Drv_Uart_KernelUpload(0x0F))
        return false;

    Drv_Uart.refCount++;
    return true;
}

static bool RefDel(void) {
    if (Drv_Uart.refCount == 0)
        return false;
    if (Drv_Uart.refCount == 1) {
        Kdev_RefDel(&DeviceUart);
    }
    Drv_Uart.refCount--;
    return true;
}

static bool PnpStart(int port, dcm_link_t link, dcm_type_t type) {
    if (port >= 4 || link != DCM_LINK_INPUT_UART) return false;
    (void) type; // always re-probe

    Drv_Uart.devmap.link[port] = DCM_LINK_INPUT_UART;
    Drv_Uart.devmap.type[port] = DCM_DEV_UNKNOWN;
    Drv_Uart.devmap.mode[port] = 0;
    Drv_Uart.booting[port]     = true;
    Drv_Uart_KernelUpload(1 << port);
    GoReboot(port);
    return true;
}

static void PnpStop(int port) {
    if (port >= 4) return;

    Drv_Uart.devmap.link[port] = DCM_LINK_NONE;
    Drv_Uart.devmap.type[port] = DCM_DEV_NONE;
    Drv_Uart.devmap.mode[port] = 0;
    Drv_Uart.booting[port]     = false;
    Drv_Uart_KernelUpload(1 << port);
    Drv_Uart.state[port] = UART_OFF;
}

static bool Present(int port) {
    if (port >= 4) return false;

    return Drv_Uart.state[port] == UART_READY ||
           Drv_Uart.state[port] == UART_WAITING_FOR_SWITCH ||
           Drv_Uart.state[port] == UART_WAITING_FOR_STABILITY ||
           Drv_Uart.state[port] == UART_WAITING_FOR_WRITE;
}

static bool Ready(int port) {
    if (port >= 4) return false;

    return Drv_Uart.state[port] == UART_READY;
}

static bool Switch(int port, int hwMode) {
    if (!Present(port)) return false;
    if (Drv_Uart.devmap.mode[port] == hwMode) return true;

    Drv_Uart.devmap.mode[port] = hwMode;
    Drv_Uart_KernelUpload(1 << port);
    GoSwitch(port);
    return true;
}

static bool StartCommand(int port, const uint8_t *command, int length) {
    if (!Ready(port)) return false;

    Drv_Uart_KernelWrite(port, command, length);
    Drv_Uart.state[port] = UART_WAITING_FOR_WRITE;
    return true;
}

static void Tick(void) {
    for (int port = 0; port < 4; port++) {
        uint8_t flags = DeviceUart.mmap->Flags[port];

        switch (Drv_Uart.state[port]) {
        case UART_OFF: { // noop
            break;
        }

        case UART_WAITING_FOR_BOOT: { // waiting for port change or readiness
            if (flags & (UART_FLAG_PNP_CHANGE | UART_FLAG_DATA_READY)) {
                ProbeAndGo(port);

            } else if (--Drv_Uart.timer[port] <= 0) {
                GoOff(port);
            }
            break;
        }

        case UART_WAITING_FOR_SWITCH: {
            if (flags & UART_FLAG_PNP_CHANGE) {
                GoReboot(port);

            } else if (flags & UART_FLAG_DATA_READY) {
                GoWait(port);

            } else if (--Drv_Uart.timer[port] <= 0) {
                GoOff(port);
            }
            break;
        }

        case UART_WAITING_FOR_STABILITY: {
            if (flags & UART_FLAG_PNP_CHANGE) {
                GoReboot(port);

            } else if (--Drv_Uart.timer[port] <= 0) {
                Drv_Uart.state[port] = UART_READY;
                DriverUart.Sensor.ResetDatalog(port);
                if (Drv_Uart.booting[port]) {
                    Drv_Uart.booting[port] = false;
                    pnp_type_t type = IdentifyDevice(Drv_Uart.types[port][0].Main.Device);
                    Hal_Pnp_HandshakeFinished(port, false, type);
                }
            }
            break;
        }

        case UART_READY: {
            if (flags & UART_FLAG_PNP_CHANGE) {
                GoReboot(port);
            }
            break;
        }

        case UART_TTY: { // tty noop
            break;
        }

        case UART_WAITING_FOR_WRITE: {
            if (flags & UART_FLAG_PNP_CHANGE) {
                // something bad happened, let's go back
                GoReboot(port);

            } else if (!(flags & UART_FLAG_SENDING)) {
                // advance forward
                Drv_Uart.state[port] = UART_READY;
            }
            break;
        }
        }
    }
}

static void ProbeAndGo(int port) {
    // load all modes
    for (int mode = 0; mode < MAX_MODES; mode++) {
        Drv_Uart_KernelReadType(port, mode);
        HwDb_Bugfix(&Drv_Uart.types[port][mode]);
    }
    Drv_Uart_KernelClearChanged(port);

    // identify
    dcm_type_t dcmDev = Drv_Uart.types[port][0].Main.Device;
    pnp_type_t pnpDev = IdentifyDevice(dcmDev);

    if (pnpDev == PNP_DEVICE_NONE || pnpDev == PNP_DEVICE_UNKNOWN) {
        Drv_Uart.devmap.type[port] = DCM_DEV_NONE;
        Drv_Uart.state[port]       = UART_OFF;
        Hal_Pnp_HandshakeFailed(port, false);

    } else if (pnpDev == PNP_DEVICE_TTY) {
        Drv_Uart.devmap.type[port] = DCM_DEV_TTY;
        Drv_Uart.state[port]       = UART_TTY;
        Hal_Pnp_HandshakeFinished(port, false, pnpDev);

    } else {
        Drv_Uart.devmap.type[port] = dcmDev;
        GoSwitch(port);
    }
}

static void GoReboot(int port) {
    Drv_Uart.state[port]       = UART_WAITING_FOR_BOOT;
    Drv_Uart.timer[port]       = MAX_HANDSHAKE_TIME;
    Drv_Uart.devmap.mode[port] = 0;
}

static void GoSwitch(int port) {
    Drv_Uart.state[port] = UART_WAITING_FOR_SWITCH;
    Drv_Uart.timer[port] = MAX_SWITCH_TIME;
}

static void GoWait(int port) {
    Drv_Uart.state[port] = UART_WAITING_FOR_STABILITY;
    Drv_Uart.timer[port] = Drv_Uart.types[Drv_Uart.devmap.mode[port]]->Main.ModeswitchMsec;
    if (Drv_Uart.timer[port] == 0)
        Drv_Uart.timer[port] = 10;
}

static void GoOff(int port) {
    Drv_Uart.state[port] = UART_OFF;
    Hal_Pnp_HandshakeFailed(port, false);
}

static const typedata_t *GetTypedata(int port, int mode) {
    return &Drv_Uart.types[port][mode].Main;
}

static const typedata_t *GetCurrentTypedata(int port) {
    return GetTypedata(port, Drv_Uart.devmap.mode[port]);
}

static int ImportRaw(uint8_t *pData, float *out, int slots, const typedata_t *data) {
    int realCount = slots;
    if (realCount > data->Values)
        realCount = data->Values;

    data_format_t fmt = data->InterpretAs;
    if (fmt == FORMAT_S8) {
        for (int i = 0; i < realCount; i++)
            out[i] = (float) (((int8_t *) pData)[i]);

    } else if (fmt == FORMAT_S16) {
        for (int i = 0; i < realCount; i++)
            out[i] = (float) (((int16_t *) pData)[i]);

    } else if (fmt == FORMAT_S32) {
        for (int i = 0; i < realCount; i++)
            out[i] = (float) (((int32_t *) pData)[i]);

    } else if (fmt == FORMAT_FLT) {
        for (int i = 0; i < realCount; i++)
            out[i] = ((float *) pData)[i];
    }

    for (int i = realCount; i < slots; i++) {
        out[i] = NAN;
    }
    return realCount;
}

static int MeasureRaw(int port, float *out, int slots, const typedata_t *data) {
    uint16_t current = DeviceUart.mmap->Buffer_LastPtr[port];
    uint8_t  *pData  = DeviceUart.mmap->Buffer_Data[port][current];

    return ImportRaw(pData, out, slots, data);
}

static bool Measure(int port, float *out, int slots, unit_type_t msr, bool clip) {
    if (!Ready(port)) return false;
    if (slots == 0) return true;

    const typedata_t *data = GetCurrentTypedata(port);

    int real = MeasureRaw(port, out, slots, data);
    Transform(out, out, real, data, msr, clip);
    return true;
}

static bool ResetDatalog(int port) {
    if (!Ready(port)) return false;

    Drv_Uart.datalogIndex[port] = DeviceUart.mmap->Buffer_LastPtr[port];
    Drv_Uart.datalogAge[port]   = DeviceUart.mmap->Buffer_Age[port][Drv_Uart.datalogIndex[port]];
    if (Drv_Uart.datalogAge[port] > 0)
        Drv_Uart.datalogAge[port]--; // let one value in
    return true;
}

static int ReadDatalog(int port, int *pRepeat, float *si, float *pct, float *raw, int slots, bool clip) {
    if (!Ready(port)) return false;

    typedata_t *info = &Drv_Uart.types[port][Drv_Uart.devmap.mode[port]].Main;

    int readFrom = -1;
    while (Drv_Uart.datalogIndex[port] != DeviceUart.mmap->Buffer_WritePtr[port]) {
        int newAge = DeviceUart.mmap->Buffer_Age[port][Drv_Uart.datalogIndex[port]];
        if (Drv_Uart.datalogAge[port] < newAge) {
            readFrom = Drv_Uart.datalogIndex[port];
            *pRepeat = newAge - Drv_Uart.datalogAge[port];
            Drv_Uart.datalogAge[port] = newAge;
            break;

        } else {
            Drv_Uart.datalogAge[port] = 0;
            Drv_Uart.datalogIndex[port]++;
            if (Drv_Uart.datalogIndex[port] >= KERNEL_DATALOG_ENTRIES)
                Drv_Uart.datalogIndex[port] = 0;
            // next iteration
        }
    }
    if (readFrom == -1) {
        *pRepeat = 0;
        return 0;
    }

    float rawStore[8];

    uint8_t *bucket = DeviceUart.mmap->Buffer_Data[port][readFrom];
    int     real    = ImportRaw(bucket, rawStore, slots, info);
    TransformBlock(rawStore, raw, pct, si, real, slots, info, clip);
    return real;
}

static pnp_type_t GetDevice(int port) {
    if (!Present(port)) return PNP_DEVICE_NONE;

    return IdentifyDevice(Drv_Uart.devmap.type[port]);
}

static const char *GetDeviceName(int port) {
    if (!Present(port)) return NULL;

    return GetCurrentTypedata(port)->Name;
}

static int GetMode(int port) {
    if (!Present(port)) return -1;

    return GetCurrentTypedata(port)->Mode;
}

static int GetModes(int port) {
    if (!Present(port)) return -1;

    return GetCurrentTypedata(port)->GuiVisibleModes;
}

static const char *GetModeName(int port, int mode) {
    if (!Present(port)) return NULL;
    if (mode >= 8) return NULL;

    const char *name = Drv_Uart.types[port][mode].Main.Name;
    if (name[0] == '\0')
        return NULL;
    return name;
}

static const char *GetSiUnit(int port) {
    if (!Present(port)) return NULL;

    return GetCurrentTypedata(port)->Unit;
}

interface_t DriverUart = {
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
        .ReadDatalog  = ReadDatalog,
    },
};
