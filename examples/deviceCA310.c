#include "deviceCA310.h"
#include "commonCa310.h"


static Ca310Interface ca310device1;  //自己定义了设备
static Ca310Fun  *ca310deviceFun;    //调用其他设备的接口

int CA310BindDriver(sky_comDriver* driver)
{
	ca310deviceFun = 0;
	return COMCa310InterfaceInit(&ca310device1, driver);
}

int CA310BindFun(Ca310Fun  *ca310Fun)
{
	ca310deviceFun = ca310Fun;
	return 0;
}

int CA310_Init(int ch,int sync)
{
	if (ca310deviceFun == 0)
		return COMCA310_Init(&ca310device1, ch, sync);
	else if (ca310deviceFun->CA310_Init != 0)
		return ca310deviceFun->CA310_Init(ch, sync);
	else
		return -1;
}

int CA310_QueryPNumber(void)
{
	if (ca310deviceFun == 0)
		return COMCA310_QueryPNumber(&ca310device1);
	else if (ca310deviceFun->CA310_QueryPNumber != 0)
		return ca310deviceFun->CA310_QueryPNumber();
	else
		return -1; 
}

int CA310_Cal0(void)
{
	if (ca310deviceFun == 0)
		return COMCA310_Cal0(&ca310device1);
	else if (ca310deviceFun->CA310_Cal0 != 0)
		return ca310deviceFun->CA310_Cal0();
	else
		return -1; 
}

int CA310_GetLvXY(u8 PN,float* Lv,float* X,float* Y)
{
	if (ca310deviceFun == 0)
		return COMCA310_GetLvXY(&ca310device1, PN, Lv, X, Y);
	else if (ca310deviceFun->CA310_GetLvXY != 0)
		return ca310deviceFun->CA310_GetLvXY(PN, Lv, X, Y);
	else
		return -1; 
}

int CA310_GetXYZ(u8 PN,float* X,float* Y,float* Z)
{
	if (ca310deviceFun == 0)
		return COMCA310_GetXYZ(&ca310device1, PN, X, Y, Z);
	else if (ca310deviceFun->CA310_GetXYZ != 0)
		return ca310deviceFun->CA310_GetXYZ(PN, X, Y, Z);
	else
		return -1; 
}

int CA310_GetFMA(u8 PN,float* FMA)
{
	if (ca310deviceFun == 0)
		return COMCA310_GetFMA(&ca310device1, PN, FMA);
	else if (ca310deviceFun->CA310_GetFMA != 0)
		return ca310deviceFun->CA310_GetFMA(PN, FMA);
	else
		return -1; 
}




