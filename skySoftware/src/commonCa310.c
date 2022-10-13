/********************************* SKYCODE ************************************
* Copyright (c) 2017-2022, 深圳思凯测试技术有限公司
* All rights reserved.
*
* 文件名称 : commonCa310.c    
* 内容摘要 : 光学探头公共底层驱动函数库
* 当前版本 : V1.8
* 作    者 : Optics group
* 完成日期 : 2022-5-24
*
* 版    本 ：V1.8
* 作    者 ：Optics group
* 修改日期 ：2022-5-24
* 修改描述 ：a.增加查询CA310 TcpduvLv功能
*           b.增加查询CA310 uvLv功能
*           c.增加设置CA310测量速度
*           d.增加设置CA310同步频率
*           e.优化注释,突出重点          
*
* 版    本 ：V1.7
* 作    者 ：Optics group
* 修改日期 ：2022-3-30
* 修改描述 ：a.增加查询CA410 ID功能
*           b.增加查询CA410色温功能
*           c.增加查询CA410 uvLv功能
*           d.增加查询CA410 flick dB功能
*
* 版    本 ：V1.6
* 作    者 ：Optics group
* 修改日期 ：2022-2-19
* 修改描述 ：a.增加CA410测量速度模式函数
*           b.解决CA310 CA410零校准通信失败
*
* 版    本 ：V1.5
* 作    者 ：Optics group
* 修改日期 ：2022-1-19
* 修改描述 ：a.修改歧义注释,删除CA410部分多余代码
*           b.修改COMCA310_Init(...)通信等待时间
*
* 版    本 ：V1.4
* 作    者 ：Optics group
* 修改日期 ：2021-11-10
* 修改描述 ：a.增加华兴源创C32驱动函数
*           b.修改部分函数接收数据校验字符"OK00"为"OK"
*
* 版    本 ：V1.3
* 作    者 ：Optics group
* 修改日期 ：2021-9-29
* 修改描述 ：a.修复CA410设定同步频率函数，小数点后位数过多导致错误
*
*
* 版    本 ：V1.2
* 作    者 ：Optics group
* 修改日期 ：2021-9-7
* 修改描述 ：a.增加CA410查询,设置通道号函数
*            b.增加C33查询INT同步频率函数
*            c.补充所有函数注释
*            d.增加CA410查询INT同步频率函数
*
*
* 版    本 ：V1.1
* 作    者 ：Optics group
* 修改日期 ：2021-9-6
* 修改描述 ：a.修改CA310,MSE查询通道函数保存通道号代码,优化程序
*            b.增加设置INT模式同步频率函数
*            c.增加部分函数注释
*
*
* 版    本 ：V1.0
* 作    者 ：
* 修改日期 ：2021-8-30
* 修改描述 ：a.增加设置测量CA310, MSE内存通道接口函数
*            b.增加查询测量CA310, MSE内存通道接口函数
*               
******************************************************************************/

#include "commonCa310.h"
#include <string.h>
#include "delay.h"
#include "stdlib.h"

/**
  * 功能描述 : 绑定驱动至探头消息结构体
  * 输入参数 : 1. ca310 -- CA310探头消息结构
  *            2. driver -- 驱动结构体
  * 返 回 值 : 绑定状态
  *            1. = 1 -- 成功  2. = -1 -- 失败
  */
int COMCa310InterfaceInit(Ca310Interface *ca310, sky_comDriver *driver)
{
	memset(ca310,0,sizeof(Ca310Interface));
	myFIFOInit(&ca310->ca310FIFO,(void *)ca310->caFIFOBuf,sizeof(ca310->caFIFOBuf));
	ca310->driver = driver;
	ca310->driver->fifo = &ca310->ca310FIFO;
	return 0;
}


/**
  * 功能描述 : 执行命令发送与接收,结果保存在FIFO缓存
  * 输入参数 : 1. ca310 -- CA310探头消息结构
  *           2. com -- 字符串命令,长度至少1024
  *           3. rec -- FIFO读取缓存区
  *           4. time -- 超时等待时间,单位ms
  * 返 回 值 : 命令响应状态
  *           1. = 1 -- 响应成功  2. = -1 -- 响应失败
  */
static int COMRunCommond(Ca310Interface *ca310, char* com,char* rec,uint32_t time)
{
	char *buf=com;
	
	if (ca310->driver == 0)
		return -1;
	
	myFIFORead(&ca310->ca310FIFO, (void *)ca310->recive, sizeof(ca310->recive));
	//ca310->driver->write((uint8_t *)buf,strlen(buf));
	devWrite(ca310->driver, (uint8_t *)buf,strlen(buf));
	
	while(time--)
	{
		if(myFIFORead(&ca310->ca310FIFO, (void *)rec, 1) > 0)
		{
			*rec = (0x7f & (*rec));
			/* 通用命令结束符'\r' */
			if(*rec==0x0d)
			{
				rec++;
				*rec=0;
				return 1;
			}
			rec++;
		}
		
		Delay_ms(1); 		
	}
	
	strcpy(ca310->recive,"ER:timeOut");
	return -1;
}

/**
  * 功能描述 : 读取探头消息结构FIFO缓冲区消息
  * 输入参数 : 1. ca310 -- CA310探头消息结构
  *            2. rec -- FIFO读取缓存区
  *            3. time -- 超时等待时间,单位ms
  * 返 回 值 : 读状态
  *            1. = 1 -- 成功  2. = -1 -- 失败
  * 注意事项 : 仅在该文件下使用
  */
static int COMRunCommond_getData(Ca310Interface *ca310, char* rec,uint32_t time)
{
	if (ca310->driver == 0)
		return -1;
	while(time--)
	{
		if(myFIFORead(&ca310->ca310FIFO, (void *)rec, 1) > 0)
		{
			if(*rec==0x0d)
			{
				rec++;
				*rec=0;
				return 1;
			}
			rec++;
		}
		
		Delay_ms(1); 		
	}
	strcpy(ca310->recive,"ER:timeOut");
	return -1;
}

/**=================================================================================
 *                      ###  CA-310通用配置功能函数  ###
 * =================================================================================
 * @{
 */

/**
  * 功能描述 : 初始化CA-310探头
  * 输入参数 : 1. ca310 -- CA310探头消息结构
  *           2. ch -- 内存通道,值域0 ~ 99
  *           3. sync -- 同步模式, 0：NTSC  1：PAL  2：EXT  3：UNIV
  * 返 回 值 : = 1 -- 初始化成功,  < 0 -- 初始化失败
  */
int COMCA310_Init(Ca310Interface *ca310, int ch, int sync)
{
	char sbuf[10];
	int i = 0;
//    uint32_t id;
	if (ca310->driver == 0)
		return -1;
	
	while(1)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); 
		if(ca310->recive!=strstr(ca310->recive,"OK")) 
		{ 
			i++;
			if (i > 3)
				return -1;
		}
		else
		{
			break;
		}
	}
	
	/* 快慢模式选择 */
	COMRunCommond(ca310, "FSC,2\r",ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}//通信模式 自动
	
	/* 内存测量通道选择 */
	if (ch >= 0)
	{
		sprintf(sbuf,"MCH,%d\r",ch);
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;} 
	}
	
	if (sync >= 0)
	{
		sprintf(sbuf,"SCS,%d\r",sync);
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;} 
	}
	
	return 1;
}

/**
  * 功能描述 : 查询CA310探头编号
  * 输入参数 : ca310 -- CA310探头消息结构
  * 返 回 值 : 输出探头号
  * 注意事项 : 仅适用CA-310多探头测量        
  */
int COMCA310_QueryPNumber(Ca310Interface *ca310)
{
	uint8_t PNumber = 0;
	uint8_t i;
	
	if (ca310->driver == 0)
		return -1;
	
	COMRunCommond(ca310, "OPR,0\r",ca310->recive,1000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	COMRunCommond(ca310, "MDS,0\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	i = ca310->recive[6] - 0x31;
	if(i > 4) return -3;
	PNumber |= (1 << i);
	while(1)
	{
		COMRunCommond_getData(ca310, ca310->recive,1000);if(ca310->recive!=strstr(ca310->recive,"OK")) {break;}
		i = ca310->recive[6] - 0x31;
		if(i > 4) return -3;
		PNumber |= (1 << i);
	}
	
	return PNumber;
}

/**
  * 功能描述 : 校零CA-310探头
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  * 返 回 值 : = 1 -- 校零成功,  < 0 -- 校零失败
  */
int COMCA310_Cal0(Ca310Interface *ca310)
{
	int comm_cnt = 0;
	
	if (ca310->driver == 0)
		return -1;
	
	/* 开始远程通信 */
	while(1)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,3000); 
		if(ca310->recive!=strstr(ca310->recive,"OK")) { 
			comm_cnt++;
			if (comm_cnt > 3)
				return -1;
		}
		else {
			break;
		}
	}
	/* 校零 */
    COMRunCommond(ca310, "ZRC\r",ca310->recive,5000);  
    if(ca310->recive!=strstr(ca310->recive,"OK")) 
    { 
    	return -2;
    }
	
	return 1;
}

/**
  * 功能描述 : 查询x, y, Lv
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. Lv -- 亮度值
  *           2. X -- XY色度图中X坐标  
  *           3. Y -- XY色度图中Y坐标
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			X[len] = 0;
			Y[len] = 0;
			Lv[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,0\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf)/10000;  //X与Y放大了一万倍传递回来的
	buf=strtok(NULL,";");
	*Y=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		X++;
		Y++;
		Lv++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*X=(float)atof(buf)/10000;  //X与Y放大了一万倍传递回来的
		buf=strtok(NULL,";");
		*Y=(float)atof(buf)/10000;
		buf=strtok(NULL,";");
		*Lv=(float)atof(buf);
	}
	return 1;	
}

/**
  * 功能描述 : 查询Tcp, duv, Lv
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. Tcp -- 色温
  *           2. duV -- 色偏差值
  *           3. Lv -- 亮度值
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetTcpduvLv(Ca310Interface *ca310, uint8_t PN, float* Tcp, float* duv, float* Lv)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			Tcp[len] = 0;
			duv[len] = 0;
			Lv[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*Tcp=(float)atof(buf);  
	buf=strtok(NULL,";");
	*duv=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		Tcp++;
		duv++;
		Lv++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*Tcp=(float)atof(buf)/10000;  //X与Y放大了一万倍传递回来的
		buf=strtok(NULL,";");
		*duv=(float)atof(buf)/10000;
		buf=strtok(NULL,";");
		*Lv=(float)atof(buf);
	}
	return 1;	
}

/**
  * 功能描述 : 查询u, v, Lv
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. u  -- u值
  *           2. v  -- v值
  *           3. Lv -- 亮度值
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetuvLv(Ca310Interface *ca310, uint8_t PN, float* u, float* v, float* Lv)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			u[len] = 0;
			v[len] = 0;
			Lv[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,5\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*u=(float)atof(buf)/10000;  //X与Y放大了一万倍传递回来的
	buf=strtok(NULL,";");
	*v=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		u++;
		v++;
		Lv++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*u=(float)atof(buf)/10000;  //X与Y放大了一万倍传递回来的
		buf=strtok(NULL,";");
		*v=(float)atof(buf)/10000;
		buf=strtok(NULL,";");
		*Lv=(float)atof(buf);
	}
	return 1;	
}

/**
  * 功能描述 : 查询X, Y, Z 
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2.PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. X -- 三刺激值之X
  *           2. Y -- 三刺激值之Y 
  *           3. Z -- 三刺激值之Z
  * 返 回 值 := 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			X[len] = 0;
			Y[len] = 0;
			Z[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,7\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf);  //X与Y放大了一万倍传递回来的
	buf=strtok(NULL,";");
	*Y=(float)atof(buf);
	buf=strtok(NULL,";");
	*Z=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		X++;
		Y++;
		Z++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*X=(float)atof(buf);  //X与Y放大了一万倍传递回来的
		buf=strtok(NULL,";");
		*Y=(float)atof(buf);
		buf=strtok(NULL,";");
		*Z=(float)atof(buf);
	}
	return 1;
}

/**
  * 功能描述 : 查询闪烁值
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 := 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA)
{
	int len=0;
	char sbuf[10];
	int i;
	
	if (ca310->driver == 0)
		return -1;
	
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			FMA[len] = 0;
			len++;
		}
	}
	
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,6\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	len--;
	*FMA=(float)atof(ca310->recive+7);
	
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		FMA++;
		len--;
		*FMA=(float)atof(ca310->recive+7);
	}
	return 1;	
}

/**
  * 功能描述 : 查询CA310探头唯一序列号
  * 输入参数 : 1.ca310 -- CA310探头消息结构
  *           2.PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1.id -- 探头唯一ID
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetSerialNO(Ca310Interface *ca310, uint8_t PN,int* id)
{
	if (ca310->driver == 0)
		return -1;
	int len = 0;
	char sbuf[10];
	int i;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			id[len] = 0;
			len++;
		}
	}
		
	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	sprintf(sbuf,"RPR,");
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			sprintf(sbuf+strlen(sbuf),"%d",i+1);
		}
	}
	sprintf(sbuf+strlen(sbuf),"\r");
	COMRunCommond(ca310, sbuf,ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	
	*id = atof(ca310->recive+5);
	return 1;
}

/**
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数
  * 输出参数 : 1. CH -- CA310探头通道号缓存区
  *              查询通道值域为0 ~ 99,对应通道CH00 ~ CH99
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH)
{
	int i;
	int len = 0;
	char str_ch[10];

	memset(str_ch, 0, 10);
    if (ca310->driver == 0)
	{
        return -1;
	}
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			CH[len] = 0;
			len++;
		}
	}
	
	if (PN != ca310->PNumberFlag)
	{
	}
	
	/* 查询当前测量通道号 */
	COMRunCommond(ca310, "STR,3\r", ca310->recive, 3000);
	if (ca310->recive != strstr(ca310->recive, "OK")) 
    { 
	    return -1;
	}
	
	/* 截取回复消息中通道号 */
	*CH = atoi(ca310->recive + 5);

    return 1;
}

/**
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数
  *           3. CH -- CA310探头通道号,值域为0 ~ 99,代表通道CH00 ~ CH99
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA310_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca310->driver == 0) 
	{
		return -1;
	}
	
	/* 限制通道数 */
	if (CH >= 100 || CH < 0)
	{
		return -1;
	}
	
	/* 测量通道设置 */
	sprintf(str_buff, "MCH,%d\r", CH);
	COMRunCommond(ca310, str_buff, ca310->recive, 3000);
    if (ca310->recive != strstr(ca310->recive, "OK"))  
	{
	    return -1;
	}
	
	return 1;	
}

/**
  * 功能描述 : 设置测量快慢模式
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数
  *           3. speed -- 0:SLOW  1:FAST 2:AUTO 
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA310_SetMeasureSpeed(Ca310Interface *ca310, uint8_t PN, int speed)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca310->driver == 0) 
	{
		return -1;
	}
	
	/* 限制输入参数 */
	if (speed >= 3 || speed < 0)
	{
		return -2;
	}
	
	/* 测量快慢设置 */
	sprintf(str_buff, "FSC,%d\r", speed);
	COMRunCommond(ca310, str_buff, ca310->recive, 3000);
    if (ca310->recive != strstr(ca310->recive, "OK"))  
	{
	    return -3;
	}
	
	return 1;	
}

/**
  * 功能描述 : 设置同步频率
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN  -- 保留参数
  *           3. fre -- 同步频率,值范围40.0 ~ 200.0,精度为小数点后一位
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA310_SetINTFrequency(Ca310Interface *ca310, uint8_t PN, float fre)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca310->driver == 0) 
	{
		return -1;
	}
	
	/* 限制输入参数 */
	if (fre > 200.0 || fre < 40.0)
	{
		return -1;
	}
	
	/* 设置同步频率 */
	sprintf(str_buff, "SCS,%.1f\r", fre);
	COMRunCommond(ca310, str_buff, ca310->recive, 3000);
    if (ca310->recive != strstr(ca310->recive, "OK"))  
	{
	    return -2;
	}
	
	return 1;	
}
/** @}
  * @end of CA-310
  */

/**=================================================================================
 *                      ###  CA-410通用配置功能函数  ###
 * =================================================================================
 * @{
 */

/**
  * 功能描述 : 初始化CA-410探头
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. ch -- 内存通道,值域0 ~ 99
  *           3. sync -- 同步模式, 0：NTSC  1：PAL  2：EXT  3：UNIV
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA410_Init(Ca310Interface *ca410, int ch,int sync)
{
	char sbuf[10];
	int i = 0;
	
	if (ca410->driver == 0)
		return -1;
	
	while(1)
	{
		COMRunCommond(ca410, "COM,1\r",ca410->recive,3000); 
		if(ca410->recive!=strstr(ca410->recive,"OK")) { 
			i++;
			if (i > 3)
				return -1;
		}
		else {
			break;
		}
	}
	
	/* 快慢模式选择 */
	COMRunCommond(ca410, "FSC,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}//通信模式 自动
	
	/* 内存测量通道选择 */
	if (ch >= 0)
	{
		sprintf(sbuf,"MCH,%d\r",ch);
		COMRunCommond(ca410, sbuf,ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;} 
	}
	
	if (sync >= 0)
	{
		sprintf(sbuf,"SCS,%d\r",sync);
		COMRunCommond(ca410, sbuf,ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;} 
	}
	
	return 1;
}

/**
  * 功能描述 : 校零CA410探头
  * 输入参数 : 1. ca410 -- CA410探头消息结构
  * 返 回 值 : = 1 -- 校零成功,  < 0 -- 校零失败
  */
int COMCA410_Cal0(Ca310Interface *ca410)
{
	int comm_cnt = 0;
	
	if (ca410->driver == 0)
		return -1;
	
	/* 开始远程通信 */
	while(1)
	{
		COMRunCommond(ca410, "COM,1\r",ca410->recive,3000); 
		if(ca410->recive!=strstr(ca410->recive,"OK")) { 
			comm_cnt++;
			if (comm_cnt > 3)
				return -1;
		}
		else {
			break;
		}
	}

	/* 校零 */
    COMRunCommond(ca410, "ZRC\r",ca410->recive,20000);  
    if(ca410->recive!=strstr(ca410->recive,"OK")) 
    { 
    	return -2;
    }
	
	return 1;
}

/**
  * 功能描述 : 查询CA410唯一ID
  * 输入参数 : 1. ca410 -- CA410探头消息结构
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 1. id -- 亮度值,单位cd/m^2
  * 返 回 值 : = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMCA410_GetSerialNO(Ca310Interface *ca410, uint8_t PN, int* id)
{
	char* buf;
	uint8_t cnt = 0;
	
	if (ca410->driver == 0)
		return -1;
	
	/* 远程通信 */
	COMRunCommond(ca410, "COM,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1; }
	/* 获取探头信息 */
    COMRunCommond(ca410, "IDO,1,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2; }
	
	buf = strtok(ca410->recive, ",");
	while(cnt < 4)
	{
        cnt++;
        buf = strtok(NULL, ",");
	}
	buf = strtok(NULL, ",");
	*id = (int)atoi(buf);  
	return 1;
}

/**
  * 功能描述 : 查询x, y, Lv
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. Lv -- 亮度值,单位cd/m^2
  *            2. X -- XY色度图中X坐标  
  *            3. Y -- XY色度图中Y坐标
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MDS,0\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MES,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	
	buf=strtok(ca410->recive+9,",");
	*X=(float)atof(buf);  
	buf=strtok(NULL,",");
	*Y=(float)atof(buf);
	buf=strtok(NULL,",");
	*Lv=(float)atof(buf);
		
	return 1;	
}

/**
  * 功能描述 : 查询Tcp, duv, Lv
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. Tcp -- 色温
  *            2. duV -- 色偏差值
  *            3. Lv -- 亮度值,单位cd/m^2
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetTcpduvLv(Ca310Interface *ca410, uint8_t PN,float* Tcp,float* duv,float* Lv)
{
	char* buf;
	uint8_t cnt = 0;

	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MDS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	COMRunCommond(ca410, "MES,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -3;}
	
	buf = strtok(ca410->recive, ",");
	//再切割两次
	while(cnt < 2)
	{
        cnt++;
        buf = strtok(NULL, ",");
	}
	buf = strtok(NULL, ",");
	*Tcp = (float)atof(buf);  
	buf = strtok(NULL, ",");
	*duv = (float)atof(buf);
	buf = strtok(NULL, ",");
	*Lv = (float)atof(buf);
		
	return 1;	
}

/**
  * 功能描述 : 查询u, v, Lv
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 1. u -- 色度u值
  *           2. v -- 色度v值
  *           3. Lv -- 亮度值,单位cd/m^2
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetuvLv(Ca310Interface *ca410, uint8_t PN, float* u, float* v, float* Lv)
{
	char* buf;
	uint8_t cnt = 0;

	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MDS,5\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	COMRunCommond(ca410, "MES,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -3;}
	
	buf = strtok(ca410->recive, ",");
	//再切割两次
	while(cnt < 2)
	{
        cnt++;
        buf = strtok(NULL, ",");
	}
	buf = strtok(NULL, ",");
	*u = (float)atof(buf);  
	buf = strtok(NULL, ",");
	*v = (float)atof(buf);
	buf = strtok(NULL, ",");
	*Lv = (float)atof(buf);
		
	return 1;	
}

/**
  * 功能描述 : 查询X, Y, Z
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. X -- 三刺激值之X
  *            2. Y -- 三刺激值之Y 
  *            3. Z -- 三刺激值之Z
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MDS,7\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MES,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	
	buf=strtok(ca410->recive+9,",");
	*X=(float)atof(buf);  
	buf=strtok(NULL,",");
	*Y=(float)atof(buf);
	buf=strtok(NULL,",");
	*Z=(float)atof(buf);
		
	return 1;	
}

/**
  * 功能描述 : 查询flick 百分值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,2\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "FCS,0,0\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	COMRunCommond(ca410, "MES,1\r",ca410->recive,3000);  if(ca410->recive!=strstr(ca410->recive,"OK")) { return -3;}
	
	
	buf=strtok(ca410->recive,",");
	for (uint8_t i = 0; i< 7; i++) {
		
		buf=strtok(NULL,",");
	}	
	*FMA=(float)atof(buf);

	return 1;	
}

/**
  * 功能描述 : 查询flick dB值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetdB(Ca310Interface *ca410, uint8_t PN, float* dB)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;
	COMRunCommond(ca410, "MMS,2\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "FMS,1\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	COMRunCommond(ca410, "MDS,6\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -3;}
	COMRunCommond(ca410, "FCS,1,1\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -4;}
	COMRunCommond(ca410, "JCS,1,10\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -5;}
	COMRunCommond(ca410, "MES,1\r", ca410->recive, 5000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -6;}
	COMRunCommond(ca410, "JDR,1,0\r", ca410->recive, 10000);  	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -7;}
	
	
	buf = strtok(ca410->recive,",");
	/* 截取第三个参数 */ 
	for (uint8_t i = 0; i< 3; i++) {
		buf=strtok(NULL,",");
	}	
	*dB = (float)atof(buf);

	return 1;	
}

/**
  * 功能描述 : 设置INT模式下的同步频率
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  *            3. fre -- 同步频率,最多支持小数点后两位
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA410_SetINTFrequency(Ca310Interface *ca410, uint8_t PN, float fre)
{
	char str_buff[20];

	memset(str_buff, 0, 10);
	if (ca410->driver == 0) 
	{
		return -1;
	}
	/* 开始远程通信 */
	COMRunCommond(ca410, "COM,1\r", ca410->recive, 3000);
	if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
    /* INT模式设置同步频率 */
	sprintf(str_buff, "SCS,4,%.2f\r", fre);
	COMRunCommond(ca410, str_buff, ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -2;
	}
	
	return 1;	
}

/**
  * 功能描述 : 查询INT模式下的同步频率
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. fre -- 同步频率
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetINTFrequency(Ca310Interface *ca410, uint8_t PN, float* fre)
{

	if (ca410->driver == 0) 
	{
		return -1;
	}
	/* 开始远程通信 */
	COMRunCommond(ca410, "COM,1\r", ca410->recive, 3000);
	if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
    /* 查询同步频率 */
	COMRunCommond(ca410, "STR,28\r", ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK、"))  
	{
	    return -2;
	}
	
	/* 截取回复消息中频率 */
	*fre = atof(ca410->recive + 5);

	return 1;	
}

/**
  * 功能描述 : 查询INT模式下的同步频率
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. fre -- 同步频率
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  * 注意事项 : 此函数仅能C33使用
  */
int COMCA410_OnlyC33_GetINTFrequency(Ca310Interface *ca410, uint8_t PN, float* fre)
{
	char* str_buff;

	if (ca410->driver == 0) 
	{
		return -1;
	}
	/* 开始远程通信 */
	COMRunCommond(ca410, "COM,1\r", ca410->recive, 3000);
	if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
    /* 查询同步频率 */
	COMRunCommond(ca410, "STR,1\r", ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -2;
	}
	
	/* 截取回复消息中频率 */
	str_buff = strtok(ca410->recive + 5, ",");
	str_buff = strtok(NULL, ",");
	str_buff = strtok(NULL, ",");
	*fre = (float)atof(str_buff);

	return 1;	
}

/**
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. CH -- CA410探头通道号缓存区
  *               查询通道值域为0 ~ 99,对应通道CH00 ~ CH99
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH)
{
	char str_ch[10];

	memset(str_ch, 0, 10);
    if (ca410->driver == 0)
	{
        return -1;
	}
	
	if (PN != ca410->PNumberFlag)
	{
	}
	
	/* 查询当前测量通道号 */
	COMRunCommond(ca410, "STR,3\r", ca410->recive, 3000);
	if (ca410->recive != strstr(ca410->recive, "OK")) 
    { 
	    return -1;
	}
	
	/* 截取回复消息中通道号 */
	*CH = atoi(ca410->recive + 5);

    return 1;
}

/**
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  *            3. CH -- CA410探头通道号,值域为0 ~ 99,代表通道CH00 ~ CH99
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA410_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca410->driver == 0) 
	{
		return -1;
	}
	
	/* 限制通道数 */
	if (CH >= 100 || CH < 0)
	{
		return -1;
	}
	
	/* 测量通道设置 */
	sprintf(str_buff, "MCH,%d\r", CH);
	COMRunCommond(ca410, str_buff, ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
	
	return 1;	
}

/**
  * 功能描述 : 设置测量速度
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  *            3. speedmode -- CA410测量速度模式
  *               0.SLOW 1.FAST 2.LTD.AUTO 3.AUTO
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA410_SetMeasureSpeed(Ca310Interface *ca410, uint8_t PN, int speedmode)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca410->driver == 0) 
	{
		return -1;
	}
	
	/* 限制速度模式参数 */
	if (speedmode > 3 || speedmode < 0)
	{
		return -1;
	}
	
	/* 测量速度设置 */
	sprintf(str_buff, "FSC,%d\r", speedmode);
	COMRunCommond(ca410, str_buff, ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
	
	return 1;	
}

/** @}
  * @end of CA-410
  */


/**=================================================================================
 *                      ###  MSE通用配置功能函数  ###
 * =================================================================================
 * @{
 */

/**
  * 函数定义 : COMMSE_Init(Ca310Interface *ca310, int ch,int sync)
  * 功能描述 : 初始化荷兰MSE探头
  * 输入参数 : 1. ca310 -- CA410探头消息结构
  *            2. ch -- 未使用
  *            3. sync -- 未使用
  * 返 回 值 : 设置状态
  *            1. = 1 -- 成功  2. < 0 -- 失败
  */
int COMMSE_Init(Ca310Interface *ca310, int ch,int sync)
{
	int i = 0;
//    uint32_t id;
	
	if (ca310->driver == 0)
		return -1;
	while(1)
	{
		COMRunCommond(ca310, "COM\r",ca310->recive,300); //发送任意数据，有反馈说明线路通
		if(ca310->recive!=strstr(ca310->recive,">>")) 
		{ 
			i++;
			if (i > 3)
				return -1;
		}else
		{
			break;
		}
	}
	return 1;
}

/**
  * 函数定义 : COMMSE_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y)
  * 功能描述 : 查询色坐标及亮度值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. Lv -- 亮度值,单位cd/m^2
  *            2. X -- XY色度图中X坐标  
  *            3. Y -- XY色度图中Y坐标
  * 返 回 值 : 查询状态
  *            1. = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMMSE_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;

	
	if (ca410->driver == 0)
		return -1;
	
	if(COMRunCommond(ca410, ":MEASure:Yxy\r",ca410->recive,3000)<=0){ return -1;}
	
	buf=strtok(ca410->recive,",");
	*Lv=(float)atof(buf);  
	buf=strtok(NULL,",");
	*X=(float)atof(buf);
	buf=strtok(NULL,",");
	*Y=(float)atof(buf);
	
	return 1;	
}

/**
  * 函数定义 : COMMSE_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z)
  * 功能描述 : 查询三刺激值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. X -- 三刺激值之X
  *            2. Y -- 三刺激值之Y 
  *            3. Z -- 三刺激值之Z
  * 返 回 值 : 查询状态
  *            1. = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMMSE_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;

	if(COMRunCommond(ca410, ":MEASure:XYZ\r",ca410->recive,3000)<=0){ return -1;}
	
	buf=strtok(ca410->recive,",");
	*X=(float)atof(buf);  
	buf=strtok(NULL,",");
	*Y=(float)atof(buf);
	buf=strtok(NULL,",");
	*Z=(float)atof(buf);
	return 1;	
}

/**
  * 函数定义 : COMMSE_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA)
  * 功能描述 : 查询闪烁值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 : 查询状态
  *            1. = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMMSE_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA)
{
	if (ca410->driver == 0)
		return -1;
	
	
	if(COMRunCommond(ca410, ":MEASure:FLICker 512 \r",ca410->recive,3000)>0)
	
//	buf=strtok(ca410->recive,",");
//	for (uint8_t i = 0; i< 7; i++) {
//		
//		buf=strtok(NULL,",");
//	}	
	*FMA=(float)atof(ca410->recive);
	

	return 1;	
}

/**
  * 函数定义 : COMMSE_GetSN(Ca310Interface *ca410, uint8_t PN,char* SN)
  * 功能描述 : 查询序列号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 3. SN -- MSE探头唯一序列号
  * 返 回 值 : 查询状态
  *            1. = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMMSE_GetSN(Ca310Interface *ca410, uint8_t PN,char* SN)
{
	
	memset(SN, 0, 7);
	
	if (ca410->driver == 0)
		return -1;
	
	if(COMRunCommond(ca410, ":EEPROM:READ:SN \r",ca410->recive,3000)>0)	
	memcpy(SN, ca410->recive, 7);

	return 1;	
}

/**
  * 函数定义 : COMMSE_GetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 3. CH -- MSE探头通道号字符串
  *                     a."factory"   b."user1" ~ "user30"  c."off" 
  * 返 回 值 : 查询状态
  *             1. = 1 -- 查询成功  2. < 0 -- 查询失败
  * 注意事项 : 此函数与COMMSE_GetMeasureCH冲突,不能同时使用,建议使用
  *            COMMSE_GetMeasureCH()
  */
int COMMSE_GetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
{
	memset(CH, 0, 8);
	
	if (ca410->driver == 0)
		return -1;
	
	if(COMRunCommond(ca410, ":SENSe:SBW? \r",ca410->recive,3000)>0)	
	memcpy(CH, ca410->recive, strlen(ca410->recive)+1);
	return 1;	
}

/**
  * 函数定义 : COMMSE_SetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  *            3. CH -- MSE探头通道号字符串
  *                     a."factory\r"   b."user1\r" ~ "user30\r"  c."off\r" 
  * 返 回 值 : 设置状态
  *            1. = 1 -- 设置成功  2. < 0 -- 设置失败
  * 注意事项 : 此函数与COMMSE_SetMeasureCH冲突,不能同时使用,建议使用
  *            COMMSE_SetMeasureCH()
  */
int COMMSE_SetCH(Ca310Interface *ca410, uint8_t PN, char* CH)
{
	char str1[50]=":SENSe:SBW ";
	
	memset(CH, 0, 8);
	
	if (ca410->driver == 0)
		return -1;
	
	
	strcat(str1,CH);
	strcat(str1,"\r");
	
	if(COMRunCommond(ca410, str1,ca410->recive,3000)>0)	
	memcpy(CH, ca410->recive, strlen(ca410->recive)+1);
	return 1;	
}

/**
  * 函数定义 : COMMSE_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH)
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  *            3. CH -- MSE探头通道号
  *                     a.0 -- factory通道模式    b.1 ~ 30 -- user通道模式
  *                     c.31 -- off通道模式
  * 返 回 值 : 设置状态
  *            1. = 1 -- 设置成功  2. < 0 -- 设置失败
  * 注意事项 : 此函数与COMMSE_SetCH()冲突,不能同时使用
  */
int COMMSE_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH)
{
	char buff[10];
	char fixed_str[20] = ":SENSe:SBW ";
	char *str_match[10] = {"factory\r", "off\r", "user"};
	char match_number = 0;    /* 默认为"factory"模式*/

	/* 驱动绑定状态 */
	if (ca410->driver == 0) 
	{
		return -1;
	}
	
	/* 限制通道数 */
	if (CH > 31)
	{
		return -1;
	}
	
	/* 匹配通道,设置命令 */
	if (CH == 0)
	{
		match_number = 0;
        strcat(fixed_str, str_match[match_number]);
	}	
    else if ((CH > 0) && (CH < 31))
	{
		match_number = 2;
        strcat(fixed_str, str_match[match_number]);
		sprintf(buff, "%d\r", CH);
        strcat(fixed_str, buff);
	}
	else if (CH == 31)
	{
		match_number = 1;
        strcat(fixed_str, str_match[match_number]);
	}
	
	/* 通道设置 */
    if (COMRunCommond(ca410, fixed_str, ca410->recive, 3000) < 0)  
	{
	    return -1;
	}
	
	return 1;	
}

/**
  * 函数定义 : COMMSE_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH)
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. CH -- MSE探头通道号
  *                    a.0 -- factory通道模式    b.1 ~ 30 -- user通道模式
  *                    c.31 -- off通道模式
  * 返 回 值 : 查询状态
  *            1. = 1 -- 查询成功  2. < 0 -- 查询失败
  * 注意事项 : 此函数与COMMSE_GetCH()冲突,不能同时使用
  */
int COMMSE_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH)
{
	char str_ch[10];
	char *str_match[10] = { "factory", "off", "user"};
	int ch_num = 0;    	/* 通道号0: factory */

	/* 清空缓冲区  */
	memset(str_ch, 0, 10);
	if (ca410->driver == 0)
	{
		return -1;
	}
	
	if (PN != ca410->PNumberFlag)
	{
	}
	
	/* 查询当前通道 */
	if (COMRunCommond(ca410, ":SENSe:SBW? \r",ca410->recive,3000) > 0)	
	{
	    memcpy(str_ch, ca410->recive, strlen(ca410->recive) + 1);
	}
    /* 匹配通道 */
	if (str_ch == strstr(str_ch, str_match[0]))
	{
		/* factory */
	    *CH = 0; 
	}
	else if (str_ch == strstr(str_ch, str_match[1]))
	{
		/* off */
	    *CH = 31; 
	}
	else if (str_ch == strstr(str_ch, str_match[2]))
	{
		/* 截取通道号 user1 ~ user30 */
		memset(str_ch, '0', 4);
		ch_num = atoi(str_ch);
		*CH = ch_num;
	}
	else 
	{
		*CH = -1;
		return -1;
	}
	 
	return 1;	
}


/** @}
  * @}end of MSE
  */


/**=================================================================================
 *                      ###  C32通用配置功能函数  ###
 * =================================================================================
 * @{
 */

/**
  * 函数定义 : COMC32_Init(Ca310Interface *ca310, int ch,int sync)
  * 功能描述 : 初始化CA-310探头
  * 输入参数 : 1. ca310 -- CA310探头消息结构
  *            2. ch -- 内存通道,值域0 ~ 31
  *            3. sync -- 保留
  * 返 回 值 : 设置状态
  *            1. = 1 -- 成功  2. = -1 -- 失败
  */
int COMC32_Init(Ca310Interface *ca310, int ch,int sync)
{
	char sbuf[10];
	int i = 0;

	if (ca310->driver == 0)
		return -1;
	while(1)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,3000); 
		if(ca310->recive!=strstr(ca310->recive,"OK")) 
		{ 
			i++;
			if (i > 3)
				return -1;
		}
		else
		{
			break;
		}
	}
	
	/* 内存测量通道选择 */
	if (ch >= 0 && ch <= 31)
	{
		/* C32规定,通道号必须输入两位字符 */
		if(ch < 10)
			sprintf(sbuf,"MCH,0%d\r",ch);
		else
			sprintf(sbuf,"MCH,%d\r",ch);
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1; } 
	}
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}

	return 1;
}

/**
  * 函数定义 : COMC32_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y)
  * 功能描述 : 查询色坐标及亮度值
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留
  * 输出参数 : 1. Lv -- 亮度值,单位cd/m^2
  *           2. X -- XY色度图中X坐标  
  *           3. Y -- XY色度图中Y坐标
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. = -1 -- 查询失败
  */
int COMC32_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;
	
	if (ca310->driver == 0)
		return -1;

	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MDS,0\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}

	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf)/10000;  //X与Y放大了一万倍传递回来的
	buf=strtok(NULL,";");
	*Y=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	return 1;	
}

/**
  * 函数定义 : COMC32_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z)
  * 功能描述 : 查询三刺激值
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留
  * 输出参数 : 1. X -- 三刺激值之X
  *           2. Y -- 三刺激值之Y 
  *           3. Z -- 三刺激值之Z
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. = -1 -- 查询失败
  */
int COMC32_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;

	if (ca310->driver == 0)
		return -1;

	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MDS,7\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf);  //X与Y放大了一万倍传递回来的
	buf=strtok(NULL,";");
	*Y=(float)atof(buf);
	buf=strtok(NULL,";");
	*Z=(float)atof(buf);
		
	return 1;
}

/**
  * 函数定义 : COMC32_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA)
  * 功能描述 : 查询闪烁值
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. = -1 -- 查询失败
  */
int COMC32_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA)
{
	
	if (ca310->driver == 0)
		return -1;

	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1; }
	COMRunCommond(ca310, "MDS,6\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1; }
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);   if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2; }
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	*FMA=(float)atof(ca310->recive+7);

	return 1;	
}


/**
  * 函数定义 : COMC32_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH)
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留
  * 输出参数 : 1. CH -- CA310探头通道号
  *               查询通道值域为0~31
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. = -1 -- 查询失败
  */
int COMC32_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH)
{

    if (ca310->driver == 0)
	{
        return -1;
	}
	
	
	/* 查询当前测量通道号 */
	COMRunCommond(ca310, "STR,3\r", ca310->recive, 3000);
	if (ca310->recive != strstr(ca310->recive, "OK")) 
    { 
	    return -1;
	}
	
	/* 截取回复消息中通道号 */
	*CH = atoi(ca310->recive + 5);
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}


    return 1;
}

/**
  * 函数定义 : COMC32_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH)
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留
  *           3. CH -- CA310探头通道号,值域为0~31
  * 返 回 值 : 设置状态
  *           1. = 1 -- 设置成功  2. = -1 -- 设置失败
  */
int COMC32_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca310->driver == 0) 
	{
		return -1;
	}
	
	/* 限制通道数 */
	if (CH > 31 || CH < 0)
	{
		return -1;
	}

	/* C32规定,通道号必须输入两位字符 */
	if(CH < 10)
		sprintf(str_buff, "MCH,0%d\r", CH);
	else
		sprintf(str_buff, "MCH,%d\r", CH);
	COMRunCommond(ca310, str_buff, ca310->recive, 3000); if (ca310->recive != strstr(ca310->recive, "OK"))  { return -1; }
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if (ca310->recive!=strstr(ca310->recive,"OK"))     { return -2;}

	
	return 1;	
}

/** @}
  * @}end of C32
  */
