#ifndef DRV_DCM
#define DRV_DCM

#include <stdbool.h>
#include <stdint.h>
#include "hal_pnp_defs.h"
#include "common/kdevices.h"

#define DCM_PORT_MASK 0x03
#define DCM_TYPE_MASK 0x04
#define DCM_PORT_1 0x00
#define DCM_PORT_2 0x01
#define DCM_PORT_3 0x02
#define DCM_PORT_4 0x03
#define DCM_TYPE_INPUT  0x00
#define DCM_TYPE_OUTPUT 0x04

typedef uint8_t dcm_port_id_t;

#define   ADC_MAX_VOLTS   5000
#define   ADC_MAX_POINTS  4095
#define   ADC_VOLTS(Points) ((uint16_t)((Points * ADC_MAX_VOLTS)/(ADC_MAX_POINTS)))
#define   ADC_POINTS(Volts) ((uint16_t)((Volts * ADC_MAX_POINTS)/(ADC_MAX_VOLTS)))

typedef struct dcm_callback {
    void (*linkFound)(uint8_t port, pnp_link_t link, pnp_device_t dev);
    void (*linkLost)(uint8_t port);
} dcm_callback_t;

extern bool Drv_Dcm_RefAdd(void);
extern bool Drv_Dcm_RefDel(void);
extern void Drv_Dcm_SetCallback(dcm_callback_t *mgr);
extern void Drv_Dcm_Tick(void);
extern bool Drv_Dcm_SetupPins(dcm_port_id_t port, char code);
extern uint16_t Drv_Dcm_GetIdPinVoltage(dcm_port_id_t port);

#endif //DRV_DCM
