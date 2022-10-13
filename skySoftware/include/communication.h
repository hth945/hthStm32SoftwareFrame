#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "commonIOInterface.h"


#define OFFSETOF(TYPE, MEMBER) ((int)(&((TYPE *)0)->MEMBER))  //求偏移地址
typedef int (*sky_conmmunicationSend)(uint8_t *data,int len);
typedef void (*sky_conmmunicationSendCallback)(void*);
typedef uint32_t (*sky_conmmunicationGetTime)(void);


typedef struct
{
	uint8_t startc;     //起始字符
	uint8_t cmd;
}DataHead;
	

typedef struct
{
	void *userData;       //bsp层自己使用的指针(可改变类型), 外部应用层尽量不要使用           
	void *Parent;         //一旦有其他结构体包含了 sky_comDriver,就应该给此变量赋值
	uint8_t saveFlag;	//通信标志0 正在等待  
						//	1 正在接收数据头部提取数据长度  
						//	2 正在校验数据  
						//	3 接收到一帧完整的数据
	
	uint8_t startc;       //起始字符
	uint8_t endc;         //结束字符
	uint32_t maxlen;      //缓冲区最大长度
	uint32_t dataLen;     //一帧数据中 数据段的长度
	uint32_t saveNum;     //接收一帧数据过程中  当前接收长度
	uint8_t *data;        //缓冲区地址
	uint32_t oldTime;     //上一次接收到数据的时间
	uint32_t waitTimeT;   //等待时间的精度
	
	sky_comDriver *dev;   //通用输入输出结构体句柄（默认使用，若为空 则使用旧的send）
	
	//sky_conmmunicationSend send;  //发送数据包，实现这个函数之后，才能使用内部发送完整帧的函数
									 //data 要发送的数据
									 //len  要发送的数据长度
									 //返回值  正数或0代表成功  负数代表发送失败
	sky_conmmunicationSendCallback completeCallback; //接收到一帧完整数据时的回调函数
													 //此函数在接收函数sky_receivePact中被执行
	
	sky_conmmunicationGetTime getTime;//获取当前系统时钟，单位ms
	
	
//	/*以下函数指针已舍弃，旧工程若报错 取消注释*/
//	int (*receivePact)(void *this,uint8_t c);           					//传入接收数据  底层校验
//	int (*sendPact)(void *this,uint8_t cmd,uint8_t *data,uint32_t len);  			//发送数据包
//	int (*runCmdPact)(void *this,uint8_t cmd,uint8_t *data,uint32_t len,uint32_t time);	//运行命令 返回数据放在缓冲区中
}sky_MyByteReceiveDataTypeDef;


int sky_ConmBindDriver(sky_MyByteReceiveDataTypeDef *myRecType,sky_comDriver *driver);
void sky_ConmmunicationInit(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t *data,uint32_t len,uint8_t startC,uint8_t endC,uint32_t waitTimeT,sky_conmmunicationGetTime getTime);
	
int sky_receivePactData(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t *data, int len);
/********************************************************************************
*函數名：receivePact
*描述  ：按协议接收一帧数据，放入接收中断中，每次传入一个字节，带校验与超时判断50ms
		若上一帧数据没有处理完（标志位没有清0），则不接收新数据。
*参数  ：1.myRecType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.newc       此时接收到的一个字节
*返回  ：返回通信状态标志  0 正在等待  
						1 正在接收数据头部提取数据长度  
						2 正在校验数据  
						3 接收到一帧完整的数据
********************************************************************************/
int sky_receivePact(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t newc);

/********************************************************************************
*函數名：sendData
*描述  ：按协议发送一帧数据,结构体中send函数指针实现之后才可以使用
*参数  ：1.recType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.cmd       发送数据包的命令
		 3.data     发送的数据
		 4.len       发送的数据长度
*返回  ：正数 发送的数据长度  -1发送错误
********************************************************************************/
int sky_sendData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data,uint32_t len);

/*******************************************************************************
 * 函数名 : pcui_sendData
 * 描  述 : pcui发送数据
 * 参  数 : *recType:通讯协议结构体
 * 参  数 : cmd:指令
 * 参  数 : **str:字符串指针
 * 参  数 : strLen:字符串个数
 * 返  回 : >=0:发送成功
            -1:发送失败
*******************************************************************************/
int sky_sendStrNData(sky_MyByteReceiveDataTypeDef *recType, uint8_t cmd, char **str, int strLen);

/********************************************************************************
*函數名：send2Data
*描述  ：按协议发送一帧数据,数据段分为两段,结构体中send函数指针实现之后才可以使用
*参数  ：1.recType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.cmd       发送数据包的命令
		 3.data1     发送的第一段数据
		 4.len1       发送的第一段数据长度
		 5.data2     发送的第二段数据
		 6.len2       发送的第二段数据长度
*返回  ：正数 发送的数据长度  -1发送错误
********************************************************************************/
int sky_send2Data(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data1,uint32_t len1,uint8_t *data2,uint32_t len2);

/********************************************************************************
*函數名：runCmdPact
*描述  ：运行一条命令，并等待反馈,反馈数据在结构体中,结构体中send函数指针实现之后才可以使用
*注意  ：命令运行后数据在结构体中，若后续想一直接收，请主动清0 结构体中的标志
*参数  ：1.recType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.cmd       发送数据包的命令
		 3.data     发送的数据
		 4.len       发送的数据长度
		 5.time     超时时间
*返回  ：1 执行成功  -1 超时   -2 执行错误
********************************************************************************/
int sky_runCmdPact(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data,uint32_t len,uint32_t time);


/********************************************************************************
*函數名：recOffsetType
*描述  ：按协议发送一帧数据,数据段分两字节偏移，两字节长度，第三段根据tpye判断书否包含数据
		即功能为 请求数据 还是发送数据,结构体中send函数指针实现之后才可以使用
*注意  ：此函数只发送  接收到的数据怎么处理与超时 请外部处理
*参数  ：1.recType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.data      数据的基地址
		 4.offset    要发送数据的偏移
		 5.len       要发送数据的长度
		 5.cmd       发送数据包的命令
		 6.type      是否发送具体的数据  1发送  0不发送
*返回  ：正数 发送的数据长度  -1发送错误
********************************************************************************/
int sky_recOffsetType(sky_MyByteReceiveDataTypeDef *recType,uint8_t *data, uint16_t offset, uint16_t len,uint8_t cmd, uint8_t type);


int sky_recSetVirType(sky_MyByteReceiveDataTypeDef *recType, uint8_t cmd, int time, uint16_t offset, uint16_t olen,uint8_t *data, uint8_t *data2,int len2);
int sky_waitData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,int time);
int sky_extractData(sky_MyByteReceiveDataTypeDef *recType,uint8_t *data);
int sky_runCmdType(sky_MyByteReceiveDataTypeDef *recType,uint8_t *data, uint16_t offset, uint16_t len,uint8_t cmd, uint8_t type,int time);
int sky_runOffsetCMD(sky_MyByteReceiveDataTypeDef *recType, uint8_t PN,uint8_t cmd,uint8_t **dataAdress, uint16_t offset, uint16_t len,uint8_t type,uint16_t time);
int sky_recSetVirTypePN(uint8_t PN,sky_MyByteReceiveDataTypeDef *recType, uint8_t cmd, int time, uint16_t offset, uint16_t olen,uint8_t *data, uint8_t *data2,int len2);
int sky_runNFun(int group, sky_MyByteReceiveDataTypeDef *recType,
					uint8_t cmd,uint8_t *data,uint32_t len,uint32_t time,
					uint8_t *outData,uint32_t outOnelen);
int sky_waitNData(uint8_t PN,sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,int time);
int sky_Delay_ms(sky_MyByteReceiveDataTypeDef *myRecType, int ms);

unsigned int sky_GetMeasureTime_ms(sky_MyByteReceiveDataTypeDef *myRecType, unsigned int ts);

#endif

