#ifndef _SKYSOFT_W25QXX_H__
#define _SKYSOFT_W25QXX_H__

#include "stdint.h"
#include "Public_SoftSPI.h"


typedef struct{
    spi_adapter adap;
    uint16_t id;
    uint8_t addr_over3byte;
    void (* delay)(void);//—” ±
}w25qxx_drv;


void skySoft_W25QXX_DeInit(w25qxx_drv * drv);
void skySoft_W25QXX_Init(w25qxx_drv * drv);
uint16_t skySoft_W25QXX_ReadID(w25qxx_drv * drv);

void skySoft_W25QXX_Read(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf);
void skySoft_W25QXX_Write_Page(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf);
void skySoft_W25QXX_Write_NoCheck(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf);
void skySoft_W25QXX_Write(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf);

void skySoft_W25QXX_Erase_Sector(w25qxx_drv * drv,uint32_t addr);
void skySoft_W25QXX_Erase_Chip(w25qxx_drv * drv);





#endif
