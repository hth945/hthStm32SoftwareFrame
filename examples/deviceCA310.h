#ifndef __DEVICECA310_H
#define	__DEVICECA310_H

#include "commonIOInterface.h"
#include <stdio.h>
#include "commonCa310.h"

int CA310BindDriver(sky_comDriver* driver);
int CA310BindFun(Ca310Fun  *ca310Fun);

/********************************************************************************
*函得：CA310_Init
*描述  ：初始化 CA310,
*参数  ：ch   选择哪个内存通道 -1默认 0~99
*		 sync  同步模式  -1：默认  0：NTSC 1：PAL mode 2：EXT mode 3：UNIV mode
*返回  ：正确返回1  负数为错误码 
********************************************************************************/
extern int CA310_Init(int ch,int sync);

/********************************************************************************
*函得：QueryPNumber
*描述  ：CA310查询通道
*参数  ：无
*返回  ：正数表示有那几个通道  负数为错误码  
********************************************************************************/
extern int CA310_QueryPNumber(void);

/********************************************************************************
*函得：CA310_Cal0
*描述  ：CA310调零  调用此函数时  探头必须无光 否则会出错
*参数  ：无
*返回  ：正确返回1  负数为错误码  
********************************************************************************/
extern int CA310_Cal0(void);

/********************************************************************************
*函得：CA310_GetLvXY
*描述  ：从CA310获取数据  
*参数  ：PN  :读取哪些通道  数据bit0~bit4  表示1~5通道
*		 Lv， X， Y的地址  返回到数据会保存在其中
*返回  ：正确返回1  负数为错误码  
*使用模板 if((mlen=CA310_GetLvXY(1,&Lv,&X,&Y))>0)
********************************************************************************/
extern int CA310_GetLvXY(u8 PN,float* Lv,float* X,float* Y);

/********************************************************************************
*函得：CA310_GetXYZ
*描述  ：从CA310获取数据  
*参数  ：PN  :读取哪些通道  数据bit0~bit4  表示1~5通道
*        X， Y,Z的地址  返回到数据会保存在其中
*返回  ：正确返回1  负数为错误码  
*使用模板 if((mlen=CA310_GetXYZ(1,&X,&Y,&Z))>0)
********************************************************************************/
extern int CA310_GetXYZ(u8 PN,float* X,float* Y,float* Z);

/********************************************************************************
*函得：CA310_GetLvXY
*描述  ：从CA310获取数据  
*参数  ：PN  :读取哪些通道  数据bit0~bit4  表示1~5通道
*		FMA的地址  返回到数据会保存在其中
*返回  ：正确返回1  负数为错误码  
*使用模板 if((mlen=CA310_GetFMA(1,&FMA))>0)
********************************************************************************/
extern int CA310_GetFMA(u8 PN,float* FMA);

#endif 

