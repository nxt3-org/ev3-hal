#include "io/core/device_db.h"

devdb_entry_t DeviceDB[MAX_TYPEDATA_ENTRIES] = {
    { // EV3 Console Cable
        .Base = {
            .Name            = "TERMINAL",
            .Link            = DCM_LINK_INPUT_UART,
            .Device          = DCM_DEV_TTY,
            .Mode            =    0,
            .Values          =    0,
            .InterpretAs     = FORMAT_S8,
            .Digits          =    0,
            .DecimalPlaces   =    0,
            .GuiVisibleModes =    0,
            .RawMin          =    0.0f,
            .RawMax          =  255.0f,
            .PercentMin      =    0.0f,
            .PercentMax      =  100.0f,
            .SiMin           =    0.0f,
            .SiMax           =    1.0f,
            .ModeswitchMsec  =    0,
            .AdcAutoId       =    0,
            .PinSetupString  = '-',
            .Unit            = "",
        },
        .Iic = {},
        .Nxt3 = {
        },
        .Present = true
    }
};