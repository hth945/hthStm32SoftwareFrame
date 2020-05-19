#ifndef _ADS1259_BSP_H__
#define	_ADS1259_BSP_H__

#include "stdint.h"

void ADS12xx_Init(void);
void ADS1259_RdADC(uint8_t index,uint32_t *ADC_dat);


#endif
