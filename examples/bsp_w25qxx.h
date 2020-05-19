#ifndef _W25QXX_H__
#define _W25QXX_H__	 

#include "stdint.h"

void W25QXX_Init(void);

uint16_t W25QXX_ReadID(void);
void W25QXX_Read(uint32_t addr,uint16_t len,uint8_t* buf);
void W25QXX_Write_Page(uint32_t addr,uint16_t len,uint8_t* buf);
void W25QXX_Write_NoCheck(uint32_t addr,uint16_t len,uint8_t* buf);
void W25QXX_Write(uint32_t addr,uint16_t len,uint8_t* buf);
void W25QXX_Erase_Sector(uint32_t addr);
void W25QXX_Erase_Chip(void);
    
#endif



