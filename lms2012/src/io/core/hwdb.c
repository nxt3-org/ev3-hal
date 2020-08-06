#include <malloc.h>
#include <io/core/interface.h>
#include <io/core/hwdb.private.h>
#include <io/core/hwdb.h>
#include <stdio.h>

hwdb_t HwDb;

static adapter_info_t adapters[] = {
    {PNP_DEVICE_TTY,              LOWSPEED,       Create_TTY},
    {PNP_DEVICE_TTY,              LOWSPEED_9V,    Create_TTY},
    {PNP_DEVICE_SENSOR_EV3_SONIC, EV3SENSOR,      Create_EV3Native},
    {PNP_DEVICE_SENSOR_EV3_SONIC, LOWSPEED_9V,    Create_EV3Sonic},
    {PNP_DEVICE_SENSOR_EV3_TOUCH, EV3SENSOR,      Create_EV3Native},
    {PNP_DEVICE_SENSOR_EV3_TOUCH, SWITCH,         Create_Touch},
    {PNP_DEVICE_SENSOR_EV3_COLOR, EV3SENSOR,      Create_EV3Native},
    {PNP_DEVICE_SENSOR_EV3_COLOR, LIGHT_ACTIVE,   Create_EV3Color},
    {PNP_DEVICE_SENSOR_EV3_COLOR, LIGHT_INACTIVE, Create_EV3Color},
    {PNP_DEVICE_SENSOR_EV3_GYRO,  LIGHT_INACTIVE, Create_EV3Gyro}, // this is what NBC uses
    {PNP_DEVICE_SENSOR_NXT_LIGHT, LIGHT_ACTIVE,   Create_NxtLight},
    {PNP_DEVICE_SENSOR_NXT_LIGHT, LIGHT_INACTIVE, Create_NxtLight},
};


bool HwDb_RefAdd(void) {
    if (HwDb.refCount > 0) {
        HwDb.refCount++;
        return true;
    }
    HwDb.adapters     = adapters;
    HwDb.adapterCount = sizeof(adapters) / sizeof(adapters[0]);
    HwDb.list         = malloc(TYPEDATA_DEFAULT_COUNT * sizeof(typedb_entry_t));
    if (!HwDb.list)
        return false;
    HwDb.count    = 0;
    HwDb.capacity = TYPEDATA_DEFAULT_COUNT;
    HwDb_Load();
    HwDb.refCount++;
    return true;
}

bool HwDb_RefDel(void) {
    if (HwDb.refCount == 0)
        return false;
    if (HwDb.refCount == 1) {
        if (HwDb.list) {
            free(HwDb.list);
            HwDb.list = NULL;
        }
        HwDb.count    = 0;
        HwDb.capacity = 0;
    }
    HwDb.refCount--;
    return true;
}

///////////////////
// DEVICE SEARCH //
///////////////////

#define mV
#define AUTOID_TOLERANCE 50 mV
#define MODE_DEFAULT 0

const typedb_entry_t *HwDb_FindDefaultForAutoID(dcm_link_t link, int16_t idVoltage) {
    int16_t lower = idVoltage - AUTOID_TOLERANCE;
    int16_t upper = idVoltage + AUTOID_TOLERANCE;

    for (int i = 0; i < HwDb.count; i++) {
        typedb_entry_t *e = &HwDb.list[i];

        if (e->Main.Link != link)
            continue;

        int16_t this = e->Main.AdcAutoId;

        if (!(lower <= this && this <= upper))
            continue;

        if (e->Main.Mode != MODE_DEFAULT)
            continue;

        return e;
    }
    return NULL;
}

const typedb_entry_t *HwDb_FindDefaultForType(dcm_link_t link, dcm_type_t type) {
    return HwDb_FindForMode(link, type, MODE_DEFAULT);
}

const typedb_entry_t *HwDb_FindForMode(dcm_link_t link, dcm_type_t type, uint8_t hwMode) {
    for (int i = 0; i < HwDb.count; i++) {
        typedb_entry_t *e = &HwDb.list[i];

        if (e->Main.Link == link &&
            e->Main.Device == type &&
            e->Main.Mode == hwMode) {
            return e;
        }
    }
    return NULL;
}

adapter_factory_t HwDb_FindAdapter(pnp_type_t type, hal_nxt_type_t emulation) {
    if (type == PNP_DEVICE_MOTOR_MEDIUM || type == PNP_DEVICE_MOTOR_LARGE)
        return Create_TachoMotor;

    for (int i = 0; i < HwDb.adapterCount; i++) {
        adapter_info_t *e = &HwDb.adapters[i];

        if (e->Type == type && e->Mode == emulation) {
            return e->Factory;
        }
    }
    return NULL;
}

//////////////////////////
// DCM->PNP TRANSLATION //
//////////////////////////

pnp_link_t IdentifyLink(dcm_link_t dcm) {
    switch (dcm) {

    case DCM_LINK_NXT_COLOR:
        return PNP_LINK_NXTCOLOR;

    case DCM_LINK_NXT_DUMB:
    case DCM_LINK_INPUT_DUMB:
        return PNP_LINK_ANALOG;

    case DCM_LINK_NXT_IIC:
        return PNP_LINK_IIC;

    case DCM_LINK_INPUT_UART:
        return PNP_LINK_UART;

    case DCM_LINK_OUTPUT_TACHO:
        return PNP_LINK_MOTOR;

    default:
    case DCM_LINK_UNKNOWN:
    case DCM_LINK_NONE:
    case DCM_LINK_ERROR:
    case DCM_LINK_OUTPUT_DUMB:
    case DCM_LINK_OUTPUT_SMART:
        return PNP_LINK_NONE;
    }
}

pnp_type_t IdentifyDevice(dcm_type_t dcm) {
    switch (dcm) {
    case DCM_DEV_NXT_TOUCH:
        return PNP_DEVICE_SENSOR_NXT_TOUCH;
    case DCM_DEV_NXT_LIGHT:
        return PNP_DEVICE_SENSOR_NXT_LIGHT;
    case DCM_DEV_NXT_SOUND:
        return PNP_DEVICE_SENSOR_NXT_SOUND;
    case DCM_DEV_NXT_COLOR:
        return PNP_DEVICE_SENSOR_NXT_COLOR;
    case DCM_DEV_NXT_SONIC:
        return PNP_DEVICE_SENSOR_NXT_SONIC;
    case DCM_DEV_NXT_TEMP:
        return PNP_DEVICE_SENSOR_NXT_TEMP;
    case DCM_DEV_LARGE_MOTOR:
    case DCM_DEV_LARGE_MOTOR_2:
        return PNP_DEVICE_MOTOR_LARGE;
    case DCM_DEV_MEDIUM_MOTOR:
        return PNP_DEVICE_MOTOR_MEDIUM;
    case DCM_DEV_EV3_TOUCH:
        return PNP_DEVICE_SENSOR_EV3_TOUCH;
    case DCM_DEV_EV3_COLOR:
        return PNP_DEVICE_SENSOR_EV3_COLOR;
    case DCM_DEV_EV3_SONIC:
        return PNP_DEVICE_SENSOR_EV3_SONIC;
    case DCM_DEV_EV3_GYRO:
        return PNP_DEVICE_SENSOR_EV3_GYRO;
    case DCM_DEV_EV3_IR:
        return PNP_DEVICE_SENSOR_EV3_IR;
    case DCM_DEV_ENERGYMETER:
        return PNP_DEVICE_SENSOR_NXT_ENERGY;
    case DCM_DEV_GENERIC_IIC:
    case DCM_DEV_UNKNOWN:
    default:
        return PNP_DEVICE_UNKNOWN;
    case DCM_DEV_TTY:
        return PNP_DEVICE_TTY;
    case DCM_DEV_NONE:
    case DCM_DEV_ERROR:
        return PNP_DEVICE_NONE;
    }
}
