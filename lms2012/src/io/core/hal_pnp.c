#include <stddef.h>
#include <hal_general.h>
#include <io/core/hal_pnp.private.h>
#include <io/driver/dcm.h>
#include <hal_pnp.h>

mod_pnp_t Mod_Pnp;

static interface_t *Interfaces[PNP_LINK_COUNT] = {
    [PNP_LINK_MOTOR]    = &DriverMotor,
    [PNP_LINK_UART]     = &DriverUart,
    [PNP_LINK_IIC]      = NULL,
    [PNP_LINK_ANALOG]   = &DriverAnalog,
    [PNP_LINK_NXTCOLOR] = NULL,
};

bool Hal_Pnp_RefAdd(void) {
    if (Mod_Pnp.refCount > 0) {
        Mod_Pnp.refCount++;
        return true;
    }
    if (!HwDb_RefAdd())
        return false;
    if (!Dcm_RefAdd())
        return false;

    int drv = 0;
    for (; drv < PNP_LINK_COUNT; drv++) {
        if (Interfaces[drv] && !Interfaces[drv]->Init())
            goto cleanup;
    }

    for (int i = 0; i < 8; i++) {
        Mod_Pnp.Ports[i].Interface          = NULL;
        Mod_Pnp.Ports[i].DetectedLink       = PNP_LINK_NONE;
        Mod_Pnp.Ports[i].DetectedType       = PNP_DEVICE_NONE;
        Mod_Pnp.Ports[i].LinkFromDcm        = DCM_LINK_NONE;
        Mod_Pnp.Ports[i].TypeFromDcm        = DCM_DEV_NONE;
        Mod_Pnp.Ports[i].Adapter            = NULL;
        Mod_Pnp.Ports[i].EmulatedPins       = (struct hal_pins) {
            .pwr_mode = POWER_AUX_OFF,
            .d0_dir = DIR_IN, .d1_dir = DIR_IN,
            .d0_in = PIN_LOW, .d1_in = PIN_LOW,
            .d0_out = PIN_LOW, .d1_out = PIN_LOW,
        };
        Mod_Pnp.Ports[i].EmulationTarget    = NO_SENSOR;
        Mod_Pnp.Ports[i].LastAdapterFactory = NULL;
    }

    Mod_Pnp.refCount++;
    return true;

cleanup:
    drv--;
    for (; drv >= 0; drv--) {
        if (Interfaces[drv])
            Interfaces[drv]->Exit();
    }
    Dcm_RefDel();
    return false;
}

bool Hal_Pnp_RefDel(void) {
    if (Mod_Pnp.refCount == 0)
        return false;
    if (Mod_Pnp.refCount == 1) {
        for (int drv = 0; drv < PNP_LINK_COUNT; drv++) {
            if (Interfaces[drv] && !Interfaces[drv]->Exit())
                Hal_General_AbnormalExit("ERROR: cannot deinitialize one of sensor links");
        }

        if (!Dcm_RefDel())
            Hal_General_AbnormalExit("ERROR: cannot deinitialize DCM");
        if (!HwDb_RefDel())
            Hal_General_AbnormalExit("ERROR: cannot deinitialize hardware database");
    }
    Mod_Pnp.refCount--;
    return true;
}

void Hal_Pnp_Tick(void) {
    Dcm_Tick();

    for (int port = 0; port < 8; port++) {
        Adapter_Tick(Mod_Pnp.Ports[port].Adapter);
    }
    for (int link = 0; link < PNP_LINK_COUNT; link++) {
        if (Interfaces[link] && Interfaces[link]->Tick)
            Interfaces[link]->Tick();
    }
}

void Hal_Pnp_LinkFound(int port, bool output, dcm_link_t link, dcm_type_t dev) {
    int index = port + (output ? 4 : 0);

    Mod_Pnp.Ports[index].LinkFromDcm = link;
    Mod_Pnp.Ports[index].TypeFromDcm = dev;

    pnp_link_t pnpLink = IdentifyLink(link);
    if (pnpLink == PNP_LINK_NONE || pnpLink >= PNP_LINK_COUNT)
        return;

    interface_t *interface = Interfaces[IdentifyLink(link)];
    if (!interface)
        return;

    Mod_Pnp.Ports[index].DetectedLink = pnpLink;
    Mod_Pnp.Ports[index].DetectedType = PNP_DEVICE_UNKNOWN;
    Mod_Pnp.Ports[index].Interface    = interface;
    if (!interface->Start(port, link, dev)) {
        Hal_Pnp_HandshakeFailed(port, output);
    }
}

void Hal_Pnp_LinkLost(int port, bool output) {
    int index = port + (output ? 4 : 0);

    if (Mod_Pnp.Ports[index].Adapter) {
        Adapter_Detach(Mod_Pnp.Ports[index].Adapter);
        Adapter_Destroy(Mod_Pnp.Ports[index].Adapter);
    }
    if (Mod_Pnp.Ports[index].Interface)
        Mod_Pnp.Ports[index].Interface->Stop(port);

    Mod_Pnp.Ports[index].Adapter            = NULL;
    Mod_Pnp.Ports[index].LastAdapterFactory = NULL;
    Mod_Pnp.Ports[index].DetectedType       = PNP_DEVICE_NONE;
    Mod_Pnp.Ports[index].DetectedLink       = PNP_LINK_NONE;
    Mod_Pnp.Ports[index].LinkFromDcm        = DCM_LINK_NONE;
    Mod_Pnp.Ports[index].TypeFromDcm        = DCM_DEV_NONE;
    Mod_Pnp.Ports[index].Interface          = NULL;
}

void Hal_Pnp_HandshakeFinished(int portNo, bool output, pnp_type_t type) {
    int        index = portNo + (output ? 4 : 0);
    pnp_port_t *port = &Mod_Pnp.Ports[index];

    port->DetectedType = type;
    Hal_Pnp_SetType(portNo, output, port->EmulationTarget);
}

void Hal_Pnp_HandshakeFailed(int port, bool output) {
    Hal_Pnp_LinkLost(port, output);
}

pnp_link_t Hal_Pnp_GetLink(int port, bool output) {
    if (port < 0 || port >= 4) return PNP_LINK_NONE;
    int index = port + (output ? 4 : 0);

    return Mod_Pnp.Ports[index].DetectedLink;
}

pnp_type_t Hal_Pnp_GetDevice(int port, bool output) {
    if (port < 0 || port >= 4) return PNP_DEVICE_NONE;
    int index = port + (output ? 4 : 0);

    return Mod_Pnp.Ports[index].DetectedType;
}

bool Hal_Pnp_Restart(int inputPort) {
    if (inputPort < 0 || inputPort >= 4) return false;
    int index = inputPort + 0;

    if (Mod_Pnp.Ports[index].Interface) {
        dcm_link_t prevLink = Mod_Pnp.Ports[index].LinkFromDcm;
        dcm_type_t prevType = Mod_Pnp.Ports[index].TypeFromDcm;

        Hal_Pnp_LinkLost(inputPort, false);
        Hal_Pnp_LinkFound(inputPort, false, prevLink, prevType);
        return true;
    }
    return false;
}

extern bool Hal_Pnp_IsReady(int inputPort) {
    if (inputPort < 0 || inputPort >= 4) return false;

    switch (Adapter_IsReady(Mod_Pnp.Ports[inputPort + 0].Adapter)) {
    case READY_DEVICE_NOT_PRESENT:
        return true; // do not tie "ready" down to prevent deadlock
    case READY_NOT_SIGNALLED:
        return true; // ready by default
    case READY_SIGNALLED_YES:
        return true; // explicit approval
    case READY_SIGNALLED_NO:
        return false; // explicit disapproval
    }

    return false;
}

extern void Hal_Pnp_SetType(int port, bool output, hal_nxt_type_t mode) {
    if (port < 0 || port >= 4) return;
    int index = port + (output ? 4 : 0);

    Mod_Pnp.Ports[index].EmulationTarget = mode;

    const pnp_type_t type = Mod_Pnp.Ports[index].DetectedType;

    const adapter_factory_t newFactory = HwDb_FindAdapter(type, mode);

    if (newFactory == Mod_Pnp.Ports[index].LastAdapterFactory)
        return; // skip if noop

    if (newFactory) {
        adapter_t *oldFront = Mod_Pnp.Ports[index].Adapter;
        adapter_t *newFront = newFactory(port, Mod_Pnp.Ports[index].Interface);
        if (!newFront) return;

        Adapter_Detach(oldFront);
        if (Adapter_Attach(newFront)) {
            Adapter_SetPins(newFront, Mod_Pnp.Ports[index].EmulatedPins);
            Mod_Pnp.Ports[index].LastAdapterFactory = newFactory;
            Mod_Pnp.Ports[index].Adapter            = newFront;
            Adapter_Destroy(oldFront);
        } else {
            Adapter_Attach(oldFront);
            Adapter_Destroy(newFront);
        }
    } else {
        Adapter_Detach(Mod_Pnp.Ports[index].Adapter);
        Adapter_Destroy(Mod_Pnp.Ports[index].Adapter);
        Mod_Pnp.Ports[index].Adapter            = NULL;
        Mod_Pnp.Ports[index].LastAdapterFactory = NULL;
    }
}

extern bool Hal_Pnp_GetPins(int inputPort, struct hal_pins *pins) {
    if (inputPort < 0 || inputPort >= 4) return false;

    Adapter_GetPins(Mod_Pnp.Ports[inputPort].Adapter, &Mod_Pnp.Ports[inputPort + 0].EmulatedPins);
    *pins = Mod_Pnp.Ports[inputPort + 0].EmulatedPins;
    return true;
}

extern bool Hal_Pnp_SetPins(int inputPort, struct hal_pins pins) {
    if (inputPort < 0 || inputPort >= 4) return false;

    Mod_Pnp.Ports[inputPort + 0].EmulatedPins = pins;
    Adapter_SetPins(Mod_Pnp.Ports[inputPort].Adapter, Mod_Pnp.Ports[inputPort + 0].EmulatedPins);
    return true;
}
