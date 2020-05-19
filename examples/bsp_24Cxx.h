#ifndef _SD250x_H__
#define _SD250x_H__	 
#include "Public_Soft_24cxx.h" 
#include "Public_SoftI2C.h"

void BSP_AT24Cxx_Init(void);
int AT24CXX_ReadDate(uint8_t index,uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
int AT24CXX_WriteDate(uint8_t index,uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);


#endif

