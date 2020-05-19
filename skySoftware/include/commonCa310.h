#ifndef __COMMONCA310_H
#define	__COMMONCA310_H

#include "commonIOInterface.h"


typedef struct
{
	uint8_t PNumberFlag;        //记录上一次读取的是哪个探头
	MyFIFO_t ca310FIFO;    //通信使用的FIFO
	char caFIFOBuf[1024];  //FIFO使用到的缓冲区
	char recive[1024];   //处理字符串的临变量（有可能栈溢出）
	
	/**************使用到的硬件驱动***************/
	sky_comDriver *driver;
}Ca310Interface;


typedef struct
{
	int (*CA310_Init)(int ch,int sync);
	int (*CA310_QueryPNumber)(void);
	int (*CA310_Cal0)(void);
	int (*CA310_GetLvXY)(uint8_t PN,float* Lv,float* X,float* Y);
	int (*CA310_GetXYZ)(uint8_t PN,float* X,float* Y,float* Z);
	int (*CA310_GetFMA)(uint8_t PN,float* FMA);
}Ca310Fun;  //二次封装接口 为其他设备读取310然后传输使用

int COMCa310InterfaceInit(Ca310Interface *ca310,sky_comDriver *driver);
int COMCA310_Init(Ca310Interface *ca310, int ch,int sync);
int COMCA310_QueryPNumber(Ca310Interface *ca310);
int COMCA310_Cal0(Ca310Interface *ca310);
int COMCA310_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y);
int COMCA310_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z);
int COMCA310_GetFMA(Ca310Interface *ca310, uint8_t PN,float* FMA);


#endif

