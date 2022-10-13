/********************************* SKYCODE ************************************
* Copyright (c) 2017-2022, 深圳思凯测试技术有限公司
* All rights reserved.
*
* 文件名称 : commonCa310.h   
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
*
* 版    本 ：V1.3
* 作    者 ：Optics group
* 修改日期 ：2021-9-29
* 修改描述 ：a.修复CA410设定同步频率函数，小数点后位数过多导致错误
*
* 版    本 ：V1.2
* 作    者 ：Optics group
* 修改日期 ：2021-9-7
* 修改描述 ：a.增加CA410查询,设置通道号函数
*           b.增加C33查询INT同步频率函数
*           c.补充所有函数注释
*           d.增加CA410查询INT同步频率函数
*
* 版    本 ：V1.1
* 作    者 ：
* 修改日期 ：2021-9-6
* 修改描述 ：a.修改CA310,MSE查询通道函数
*           b.增加设置INT模式同步频率函数
*           c.增加部分函数注释
*
* 版    本 ：V1.0
* 作    者 ：
* 修改日期 ：2021-8-30
* 修改描述 ：a.增加设置测量CA310, MSE内存通道接口函数
*           b.增加查询测量CA310, MSE内存通道接口函数
*               
******************************************************************************/

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
	int (*CA310_GetSerialNO)(uint8_t PN,int* id);
	int (*CA310_GetSN)(uint8_t PN,char* SN);
	int (*CA310_GetCH)(uint8_t PN,char* SN);
	int (*CA310_SetCH)(uint8_t PN,char* SN);
}Ca310Fun;  //二次封装接口 为其他设备读取310然后传输使用

/**
  * 功能描述 : 绑定驱动至探头消息结构体
  * 输入参数 : 1. ca310 -- CA310探头消息结构
  *           2. driver -- 驱动结构体
  * 返 回 值 : = 1 -- 成功,  < 0 -- 失败
  */
int COMCa310InterfaceInit(Ca310Interface *ca310,sky_comDriver *driver);

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
int COMCA310_Init(Ca310Interface *ca310, int ch,int sync);

/**
  * 功能描述 : 查询CA310探头编号
  * 输入参数 : ca310 -- CA310探头消息结构
  * 返 回 值 : 探头编号
  * 注意事项 : 仅适用CA-310多探头测量        
  */
int COMCA310_QueryPNumber(Ca310Interface *ca310);

/**
  * 功能描述 : 校零CA-310探头
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  * 返 回 值 : = 1 -- 校零成功,  < 0 -- 校零失败
  */
int COMCA310_Cal0(Ca310Interface *ca310);

/**
  * 功能描述 : 查询x, y, Lv
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. Lv -- 亮度值
  *           2. X -- XY色度图中X坐标  
  *           3. Y -- XY色度图中Y坐标
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y);

/**
  * 功能描述 : 查询Tcp, duv, Lv
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. Tcp -- 色温
  *           2. duV -- 色偏差值
  *           3. Lv -- 亮度值
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetTcpduvLv(Ca310Interface *ca310, uint8_t PN, float* Tcp, float* duv, float* Lv);

/**
  * 功能描述 : 查询u, v, Lv
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. u  -- u值
  *           2. v  -- v值
  *           3. Lv -- 亮度值
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetuvLv(Ca310Interface *ca310, uint8_t PN, float* u, float* v, float* Lv);

/**
  * 功能描述 : 查询X, Y, Z 
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2.PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. X -- 三刺激值之X
  *           2. Y -- 三刺激值之Y 
  *           3. Z -- 三刺激值之Z
  * 返 回 值 := 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z);

/**
  * 功能描述 : 查询闪烁值
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 := 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetFMA(Ca310Interface *ca310, uint8_t PN,float* FMA);

/**
  * 功能描述 : 查询CA310探头唯一序列号
  * 输入参数 : 1.ca310 -- CA310探头消息结构
  *           2.PN -- 默认为1,CA310多探头输出时,选择bit[x](x = 0..5) = 1指定探头输出
  * 输出参数 : 1.id -- 探头唯一ID
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetSerialNO(Ca310Interface *ca310, uint8_t PN,int* id);

/**
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数
  * 输出参数 : 1. CH -- CA310探头通道号缓存区
  *              查询通道值域为0 ~ 99,对应通道CH00 ~ CH99
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA310_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH);

/**
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数
  *           3. CH -- CA310探头通道号,值域为0 ~ 99,代表通道CH00 ~ CH99
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA310_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH);

/**
  * 功能描述 : 设置测量快慢模式
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数
  *           3. speed -- 0:SLOW  1:FAST 2:AUTO 
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA310_SetMeasureSpeed(Ca310Interface *ca310, uint8_t PN, int speed);

/**
  * 功能描述 : 设置同步频率
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN  -- 保留参数
  *           3. fre -- 同步频率,值范围40.0 ~ 200.0,精度为小数点后一位
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA310_SetINTFrequency(Ca310Interface *ca310, uint8_t PN, float fre);

/** @}
  * @end of CA310
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
  * 返 回 值 : = 1 -- 初始化成功,  < 0 -- 初始化失败
  */
int COMCA410_Init(Ca310Interface *ca310, int ch,int sync);

/**
  * 功能描述 : 校零CA410探头
  * 输入参数 : 1. ca410 -- CA410探头消息结构
  * 返 回 值 : = 1 -- 校零成功,  < 0 -- 校零失败
  */
int COMCA410_Cal0(Ca310Interface *ca410);

/**
  * 功能描述 : 查询CA410唯一ID
  * 输入参数 : 1. ca410 -- CA410探头消息结构
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 1. id -- 亮度值,单位cd/m^2
  * 返 回 值 : = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMCA410_GetSerialNO(Ca310Interface *ca410, uint8_t PN, int* id);

/**
  * 功能描述 : 查询x, y, Lv
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. Lv -- 亮度值,单位cd/m^2
  *            2. X -- XY色度图中X坐标  
  *            3. Y -- XY色度图中Y坐标
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y);

/**
  * 功能描述 : 查询Tcp, duv, Lv
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. Tcp -- 色温
  *            2. duV -- 色偏差值
  *            3. Lv -- 亮度值,单位cd/m^2
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetTcpduvLv(Ca310Interface *ca410, uint8_t PN,float* Tcp,float* duv,float* Lv);

/**
  * 功能描述 : 查询u, v, Lv
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 1. u -- 色度u值
  *           2. v -- 色度v值
  *           3. Lv -- 亮度值,单位cd/m^2
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetuvLv(Ca310Interface *ca410, uint8_t PN, float* u, float* v, float* Lv);

/**
  * 功能描述 : 查询X, Y, Z
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. X -- 三刺激值之X
  *            2. Y -- 三刺激值之Y 
  *            3. Z -- 三刺激值之Z
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z);

/**
  * 功能描述 : 查询flick 百分值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA);

/**
  * 功能描述 : 查询flick dB值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetdB(Ca310Interface *ca410, uint8_t PN, float* dB);

/**
  * 功能描述 : 设置INT模式下的同步频率
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  *            3. fre -- 同步频率,最多支持小数点后两位
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_SetINTFrequency(Ca310Interface *ca410, uint8_t PN, float fre);

/**
  * 功能描述 : 查询INT模式下的同步频率
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. fre -- 同步频率
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetINTFrequency(Ca310Interface *ca410, uint8_t PN, float* fre);

/**
  * 功能描述 : 查询INT模式下的同步频率
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. fre -- 同步频率
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  * 注意事项 : 此函数仅能C33使用
  */
int COMCA410_OnlyC33_GetINTFrequency(Ca310Interface *ca410, uint8_t PN, float* fre);

/**
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  * 输出参数 : 1. CH -- CA410探头通道号缓存区
  *               查询通道值域为0 ~ 99,对应通道CH00 ~ CH99
  * 返 回 值 : = 1 -- 查询成功,  < 0 -- 查询失败
  */
int COMCA410_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH);

/**
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  *            3. CH -- CA410探头通道号,值域为0 ~ 99,代表通道CH00 ~ CH99
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA410_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH);
	
/**
  * 功能描述 : 设置测量速度
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *            2. PN -- 保留参数,值无效
  *            3. speedmode -- CA410测量速度模式
  *               0.SLOW 1.FAST 2.LTD.AUTO 3.AUTO
  * 返 回 值 : = 1 -- 设置成功,  < 0 -- 设置失败
  */
int COMCA410_SetMeasureSpeed(Ca310Interface *ca410, uint8_t PN, int speedmode);
  
/** @}
  * @end of CA410
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
  *           2. ch -- 未使用
  *           3. sync -- 未使用
  * 返 回 值 : 设置状态
  *           1. = 1 -- 成功  2. < 0 -- 失败
  */
int COMMSE_Init(Ca310Interface *ca310, int ch,int sync);

/**
  * 函数定义 : COMMSE_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y)
  * 功能描述 : 查询色坐标及亮度值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 1. Lv -- 亮度值,单位cd/m^2
  *           2. X -- XY色度图中X坐标  
  *           3. Y -- XY色度图中Y坐标
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMMSE_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y);

/**
  * 函数定义 : COMMSE_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z)
  * 功能描述 : 查询三刺激值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 1. X -- 三刺激值之X
  *           2. Y -- 三刺激值之Y 
  *           3. Z -- 三刺激值之Z
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMMSE_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z);

/**
  * 函数定义 : COMMSE_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA)
  * 功能描述 : 查询闪烁值
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMMSE_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA);

/**
  * 函数定义 : COMMSE_GetSN(Ca310Interface *ca410, uint8_t PN,char* SN)
  * 功能描述 : 查询序列号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 3. SN -- MSE探头唯一序列号
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. < 0 -- 查询失败
  */
int COMMSE_GetSN(Ca310Interface *ca410, uint8_t PN,char* SN);//获取探头编号

/**
  * 函数定义 : COMMSE_GetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 3. CH -- MSE探头通道号字符串
  *                     a."factory"   b."user1" ~ "user30"  c."off" 
  * 返 回 值 : 查询状态
  *            1. = 1 -- 查询成功  2. < 0 -- 查询失败
  * 注意事项 : 此函数与COMMSE_GetMeasureCH冲突,不能同时使用,建议使用
  *            COMMSE_GetMeasureCH()
  */
int COMMSE_GetCH(Ca310Interface *ca410, uint8_t PN,char* CH);

/**
  * 函数定义 : COMMSE_SetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  *           3. CH -- MSE探头通道号字符串
  *                    a."factory\r"   b."user1\r" ~ "user30\r"  c."off\r" 
  * 返 回 值 : 设置状态
  *           1. = 1 -- 设置成功  2. < 0 -- 设置失败
  * 注意事项 : 此函数与COMMSE_SetMeasureCH冲突,不能同时使用,建议使用
  *            COMMSE_SetMeasureCH()
  */
int COMMSE_SetCH(Ca310Interface *ca410, uint8_t PN,char* CH);

/**
  * 函数定义 : COMMSE_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH)
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  *           3. CH -- MSE探头通道号
  *                    a.0 -- factory通道模式    b.1 ~ 30 -- user通道模式
  *                    c.31 -- off通道模式
  * 返 回 值 : 设置状态
  *            1. = 1 -- 设置成功  2. < 0 -- 设置失败
  * 注意事项 : 此函数与COMMSE_SetCH()冲突,不能同时使用
  */
int COMMSE_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH);


/**
  * 函数定义 : COMMSE_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH)
  * 功能描述 : 查询测量通道号
  * 输入参数 : 1. ca410 -- Ca310Interface类型结构体
  *           2. PN -- 保留参数,值无效
  * 输出参数 : 1. CH -- MSE探头通道号
  *                    a.0 -- factory通道模式    b.1 ~ 30 -- user通道模式
  *                    c.31 -- off通道模式
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. < 0 -- 查询失败
  * 注意事项 : 此函数与COMMSE_GetCH()冲突,不能同时使用
  */
int COMMSE_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH);

/** @}
  * @end of MSE
  * ===============================================================================
  */
  
/**=================================================================================
*                      ###  C32通用配置功能函数  ###
* =================================================================================
* @{
* @brief  C32固件程序V2.2进入远程通信后接收异常数据时,无法自纠错,会导致通信过程中卡死C32,或
*         者远程通信无法使用.触发此BUG方式为进入远程通信后,发送错误指令一次或者连续几次.或者
*         在通信过程中异常断开物理连接,导致本次通信异常,后续可能就无法正常通信.出现该情况后只
*         能重启C32,重新进入测量.
*         
*/

/**
 * 函数定义 : COMC32_Init(Ca310Interface *ca310, int ch,int sync)
 * 功能描述 : 初始化CA-310探头
 * 输入参数 : 1. ca310 -- CA310探头消息结构
 *           2. ch -- 内存通道,值域0 ~ 31
 *           3. sync -- 保留
 * 返 回 值 : 设置状态
 *           1. = 1 -- 成功  2. = -1 -- 失败
 */
int COMC32_Init(Ca310Interface *ca310, int ch,int sync);


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
int COMC32_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y);


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
int COMC32_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z);


/**
  * 函数定义 : COMC32_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA)
  * 功能描述 : 查询闪烁值
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留
  * 输出参数 : 1. FMA -- flick值
  * 返 回 值 : 查询状态
  *           1. = 1 -- 查询成功  2. = -1 -- 查询失败
  */
int COMC32_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA);


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
int COMC32_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH);


/**
  * 函数定义 : COMC32_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH)
  * 功能描述 : 设置测量通道号
  * 输入参数 : 1. ca310 -- Ca310Interface类型结构体
  *           2. PN -- 保留
  *           3. CH -- CA310探头通道号,值域为0~31
  * 返 回 值 : 设置状态
  *           1. = 1 -- 设置成功  2. = -1 -- 设置失败
  */
int COMC32_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH);

/** @}
  * @end of C32
  * ===============================================================================
  */
  
#endif

