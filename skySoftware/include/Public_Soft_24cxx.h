#ifndef __24CXX_H
#define __24CXX_H
#include "Public_SoftI2C.h"

typedef enum {
		 AT24C01	=	127
		,AT24C02	=	255
		,AT24C04	=	511
		,AT24C08	=	1023
		,AT24C16	=	2047
		,AT24C32	=	4095
		,AT24C64	=    8191
		,AT24C128	=	16383
		,AT24C256	=	32767  	
} AT24Cxx_TYPE ;

typedef enum {
		 ADDR000	=	0X50+0
		,ADDR001	=	0X50+1
		,ADDR010	=	0X50+2
		,ADDR011	=	0X50+3
		,ADDR100	=	0X50+4
		,ADDR101	=	0X50+5
		,ADDR110	=   0X50+6
		,ADDR111	=	0X50+7  	
} hard_A0_A1_A2 ;

typedef struct{
	hard_A0_A1_A2 AT24CXX_Address ;
	AT24Cxx_TYPE AT24C08_TYP;

}AT24Cxx_drv;



int AT24CXX_Write(i2c_adapter *adap,AT24Cxx_drv *at24cxx,uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);
int AT24CXX_Read(i2c_adapter *adap,AT24Cxx_drv *at24cxx,uint16_t Addr,uint8_t *pBuffer,uint16_t NumToRead);
#endif
















