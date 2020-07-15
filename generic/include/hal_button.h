#ifndef   HAL_BUTTON
#define   HAL_BUTTON

void  Hal_Button_Init(void *pHeaders, UBYTE Prescaler);
void  Hal_Button_Exit(void);
void  Hal_Button_Read(UBYTE *pOutMask);

#endif
