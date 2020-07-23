#ifndef DRV_ANALOG
#define DRV_ANALOG

#include "drv_port.h"

extern bool Drv_Analog_RefAdd(void);
extern bool Drv_Analog_RefDel(void);

extern bool Drv_Analog_AdcValue(uint8_t sPort, uint16_t *pValue);
extern bool Drv_Analog_RawRange(uint8_t sPort, float *pValue);
extern bool Drv_Analog_PctValue(uint8_t sPort, float *pValue);
extern bool Drv_Analog_SiValue(uint8_t sPort, float *pValue);

extern port_driver_ops_t DriverAnalog;

#endif //DRV_ANALOG
