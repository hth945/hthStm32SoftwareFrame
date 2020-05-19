/********************************* SKYCODE *************************************
* Copyright (c) 2015-2016, 深圳思凯测试技术有限公司（广州天码电子技术有限公司）
* All rights reserved.
*
* 文件名   ：Bus.h    
* 描述     ：Bus 通讯总线驱动程序
*
* 版    本 ：V0.4
* 作    者 ：SKYCODE2013
* 完成日期 ：2016-12-16
* 修改描述 ：1. 结合Bus1.0 和 mbus
*            2. Bus 1.0 波特率是500k
*
* 版    本 ：V0.3
* 作    者 ：SKYCODE2013
* 完成日期 ：2016-11-16
* 修改描述 ：1. 修改添加接收滤波器的漏洞
*            2. 更名BUS
*            3. 删除了64B PC13脚的修正
*
* 版    本 ：V0.2
* 作    者 ：SKYCODE2013
* 完成日期 ：2016-05-28
* 修改描述 xxxx中断向量包含在此
*
* 版    本 ：V0.1
* 作    者 ：SKYCODE2013
* 完成日期 ：2016-04-01
* 修改描述 ：
*					                          	
*******************************************************************************/

#ifndef _BUS_H_
#define _BUS_H_

#include "stm32f10x.h"

#define BUS_MAX_FILTER 28 //过滤器数量


/*
Busl送消息结构体,有用的7个元素
1. BusTxMsgStruct.dst_type - 目标模块类型 取值0x00~0x3F (0~63)
2. BusTxMsgStruct.dst_id   - 目标模块ID，取值 0x0~0x7 (0~7)
3. BusTxMsgStruct.src_type - 源模块类型，取值0x00~0x3F (0~63)
4. BusTxMsgStruct.src_id   - 源模块ID     0x0~0x7 (0~7)
5. BusTxMsgStruct.index    - 功能索引 0x00~0xFF (0~255)
6. BusTxMsgStruct.dlc      - l送的数据长度 0~8
7. BusTxMsgStruct.data[8]  - l送的数据
*/
typedef struct
{
	u32 resev1:32;
	
	u32 index:8;  //8bit 功能索引
	u32 src_id:3;   //3bit 源模块ID
	u32 src_type:6; //6bit 源模块类型
	u32 dst_id:3;   //3bit 目标模块ID
	u32 dst_type:6; //6bit 目标模块类型
	u32 resev2:3;   //无视
	u32 resev3:3;   //无视

	u8 resev4;      //无视
	u8 resev5;      //无视
	
	u8 dlc; 	 //8bit 0~8 数据长度
	u8 data[8];   //8Byte
}
BusTxMsgStruct;

/*
Bus接收消息结构体,有用的8个元素
1. BusTxMsgStruct.dst_type - 目标模块类型 取值0x00~0x3F (0~63)
2. BusTxMsgStruct.dst_id   - 目标模块ID，取值 0x0~0x7 (0~7)
3. BusTxMsgStruct.src_type - 源模块类型，取值0x00~0x3F (0~63)
4. BusTxMsgStruct.src_id   - 源模块ID     0x0~0x7 (0~7)
5. BusTxMsgStruct.index    - 功能索引 0x00~0xFF (0~255)
6. BusTxMsgStruct.dlc      - l送的数据长度 0~8
7. BusTxMsgStruct.data[8]  - l送的数据
8. BusTxMsgStruct.fmi      - 过滤器号
*/
typedef struct
{
	u32 resev1:32;
	
	u32 index:8;  //8bit 功能索引
	u32 src_id:3;   //3bit 源模块ID
	u32 src_type:6; //6bit 源模块类型
	u32 dst_id:3;   //3bit 目标模块ID
	u32 dst_type:6; //6bit 目标模块类型
	u32 resev2:3;   //无视
	u32 resev3:3;   //无视

	u8 resev4;      //无视
	u8 resev5;      //无视
	
	u8 dlc; 	 //8bit 1~8 数据长度
	u8 data[8];   //8Byte
	
	u8 fmi;       //过滤器号
}
BusRxMsgStruct;

//接收过滤器设置的结构体
//注: 目标是指 BUS信息 传送的目标
//      源 是指 BUS信息的发送源
typedef struct
{
	u8 dst_type; 	//目标模块（即接收端）类型
	u8 dst_id;   	//目标模块（即接收端）ID
	u8 filt_dst;    //使能过滤 接收端的类型和ID。=0 不过滤dst_type、dst_id，=1过滤dst_type、dst_id，
	u8 src_type; 	//源模块（即l送端）类型
	u8 src_id;   	//源模块（即l送端）ID
	u8 filt_src;    //使能过滤 l送端的类型和ID
	//void (*rx_irq_handler)(void* rx_msg); //接收中断函数
	
	
	//昊  增加其他形式的接收中断函数
	void *userData;
	int (*rx_irq_handler)(void* rx_msg,void *userData); 
}
BusFilterStruct;


/*Bus总线中各种模块类型定义*/
#define BUS_MAIN        1 //测试机主机
#define BUS_POWER       2 //电源板
#define BUS_KEYBOARD    3 //按键盒
#define BUS_MONITOR     4 //按键盒上的显示器
#define BUS_CCD         5 //镜^
#define BUS_RS232       6 //Bus转串口(RS232)
#define BUS_MOTOR       7 //电机驱动
#define BUS_WAVE        8
#define BUS_LED         9
#define BUS_VOL         10
#define BUS_S10POWER    11 //电源板
#define BUS_CFL    		12 //新镜头
#define BUS_SCAN    	13 //扫描枪
#define BUS_KEY    	    14 //一拖五按键盒子
#define BUS_FORWARD	    15 //一拖十五抓板
#define BUS_CE48CORE    16 //ce48核心板
#define BUS_CE48KEY	    17 //ce48按键板
#define BUS_CE48C8T6    18 //ce48FPAGc8t6
#define BUS_DHCP        19 //自动分配ID总通信板
#define BUS_SD250       20 //自动分配ID总通信板



/*mBUSl送邮箱状态*/
#define BUS_NO_MB 0x04 //Bus没有可用的邮箱来l送信息
#define BUS_TXFAILED 0X05 //l送失败

/*添加接收状态定义*/
#define BUS_ADDRXMLD_ERROR   0XFD
#define BUS_ADDRXMLD_REPEAT  0XFE //重复添加接收
#define BUS_ADDRXMLD_NOFILT  0xFF //接收过滤器用完

/*
*函得：Bus_Init
*描述  ：Bus 总线初始化。此函数只能调用一次，它会清除所有Bus模块的添加记录
*参数  ：无
*返回  ：无      
*/
void Bus_Init(void);

/*
*函得：Bus_AddRxMdl
*描述  ：增加接收模块
*参数  ：BusFilterStruct 结构体指针
*
*返回  ：接收过滤器编号 0~(BUS_MAX_FILTER-1)
*       =BUS_ADDRXMLD_ERROR  错误 没有要过滤的
*       =BUS_ADDRXMLD_REPEAT 重复添加    
*		=BUS_ADDRXMLD_NOFILT 滤波器用完，增加失败  
*/
u8 Bus_AddRxMdl(BusFilterStruct* filt_struct);

/*
*函得：Bus_SendMsg
*描述  ：Bus l送消息
*参数  ：bus_tx_msg - 消息结构体 请看BusTxMsgStruct 结构体的声明
*返回  ：l送消息用到的邮箱号， 0~2
*        =BUS_NO_MB 说明没有空邮箱，l送失败
*        =BUS_TXFAILED 
*/
u8 Bus_SendMsg(BusTxMsgStruct* bus_tx_msg);

/*
*函得：Bus_DisableRxIT
*描述  ：禁止Bus 接收中断
*参数  ：无
*返回  ：无     
*/
void Bus_DisableRxIT(void);

/*
*函得：Bus_EnableRxIT
*描述  ：使能Bus 接收中断
*参数  ：无
*返回  ：无     
*/
void Bus_EnableRxIT(void);


////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////mBus/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
mBusl送消息结构体,有用的7个元素
1. mBusTxMsgStruct.dst_type - 目标模块类型 取值0x0~0xF (0~15)
2. mBusTxMsgStruct.dst_id   - 目标模块ID，取值 0x0~0xF (0~15)
3. mBusTxMsgStruct.src_type - 源模块类型，取值0x0~0xF (0~15)
4. mBusTxMsgStruct.src_id   - 源模块ID     0x0~0xF (0~15)
5. mBusTxMsgStruct.index    - 功能索引 0x000~0x3FF (0~1023)
6. mBusTxMsgStruct.dlc      - l送的数据长度 1~8
7. mBusTxMsgStruct.data[8]  - l送的数据
*/
typedef struct
{
	u32 resev1:32;
	
	u32 index:10;  //10bit 功能索引
	u32 src_id:4;   //4bit 源模块ID
	u32 src_type:4; //4bit 源模块类型
	u32 dst_id:4;   //4bit 目标模块ID
	u32 dst_type:4; //4bit 目标模块类型
	u32 resev2:3;   //无视
	u32 resev3:3;   //无视

	u8 resev4;      //无视
	u8 resev5;      //无视
	
	u8 dlc; 	 //8bit 1~8 数据长度
	u8 data[8];   //8Byte
}
mBusTxMsgStruct;

/*
mBus接收消息结构体,有用的8个元素
1. mBusTxMsgStruct.dst_type - 目标模块类型 取值0x0~0xF (0~15)
2. mBusTxMsgStruct.dst_id   - 目标模块ID，取值 0x0~0xF (0~15)
3. mBusTxMsgStruct.src_type - 源模块类型，取值0x0~0xF (0~15)
4. mBusTxMsgStruct.src_id   - 源模块ID     0x0~0xF (0~15)
5. mBusTxMsgStruct.index    - 功能索引 0x000~0x3FF (0~1023)
6. mBusTxMsgStruct.dlc      - l送的数据长度 1~8
7. mBusTxMsgStruct.data[8]  - l送的数据
8. mBusTxMsgStruct.fmi      - 过滤器号
*/
typedef struct
{
	u32 resev1:32;
	
	u32 index:10;  //10bit 功能索引
	u32 src_id:4;   //4bit 源模块ID
	u32 src_type:4; //4bit 源模块类型
	u32 dst_id:4;   //4bit 目标模块ID
	u32 dst_type:4; //4bit 目标模块类型
	u32 resev2:3;   //无视
	u32 resev3:3;   //无视

	u8 resev4;      //无视
	u8 resev5;      //无视
	
	u8 dlc; 	 //8bit 1~8 数据长度
	u8 data[8];   //8Byte
	
	u8 fmi;       //过滤器号
}
mBusRxMsgStruct;

//接收过滤器设置的结构体

#define mBusFilterStruct BusFilterStruct


/*mBus总线中各种模块类型定义*/
#define MBUS_MAIN        1 //测试机主机
#define MBUS_POWER       2 //电源板
#define MBUS_KEYBOARD    3 //按键盒
#define MBUS_DISPLAY     4 //按键盒上的显示器
#define MBUS_CCD         5 //镜^
#define MBUS_RS232       6 //mBus转串口(RS232)
#define MBUS_MOTOR       7 //电机驱动

/*mBUSl送邮箱状态*/
#define MBUS_NO_MB 0x04 //mBus没有可用的邮箱来l送信息
#define MBUS_TXFAILED 0X05 //l送失败

/*添加接收状态定义*/
#define MBUS_ADDRXMLD_ERROR   0XFD
#define MBUS_ADDRXMLD_REPEAT  0XFE //重复添加接收
#define MBUS_ADDRXMLD_NOFILT  0xFF //接收过滤器用完

/*
*函得：mBus_Init
*描述  ：mBus 总线初始化。此函数只能调用一次，它会清除所有mBus模块的添加记录
*参数  ：无
*返回  ：无      
*/
void mBus_Init(void);

/*
*函得：mBus_AddRxMdl
*描述  ：增加接收模块
*参数  ：mBusFilterStruct 结构体指针
*
*返回  ：接收过滤器编号 0~13
*       =MBUS_ADDRXMLD_ERROR  错误 没有要过滤的
*       =MBUS_ADDRXMLD_REPEAT 重复添加    
*		=MBUS_ADDRXMLD_NOFILT 滤波器用完，增加失败  
*/
u8 mBus_AddRxMdl(mBusFilterStruct* filt_struct);

/*
*函得：mBus_SendMsg
*描述  ：mBus l送消息
*参数  ：mbus_tx_msg - 消息结构体 请看mBusTxMsgStruct 结构体的声明
*返回  ：l送消息用到的邮箱号， 0~2
*        =MBUS_NO_MB 说明没有空邮箱，l送失败
*        =MBUS_TXFAILED 
*/
u8 mBus_SendMsg(mBusTxMsgStruct* mbus_tx_msg);

/*
*函得：mBus_DisableRxIT
*描述  ：禁止mBus 接收中断
*参数  ：无
*返回  ：无     
*/
#define mBus_DisableRxIT() Bus_DisableRxIT()

/*
*函得：mBus_EnableRxIT
*描述  ：使能mBus 接收中断
*参数  ：无
*返回  ：无     
*/
#define mBus_EnableRxIT() Bus_EnableRxIT()

///////////////////////////////////mBus END/////////////////////////////////////


#endif

/********************************* SKYCODE ************************************/
