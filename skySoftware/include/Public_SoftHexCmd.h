#ifndef __PUBLIC_SOFTHEXCMD_H__
#define __PUBLIC_SOFTHEXCMD_H__ 

#include "stdint.h"
#include "myFIFO.h"
#include "delay.h"
#include "commonIOInterface.h"

typedef struct
{
	void *userData;       
	void *Parent;         
	sky_comDriver *dev;	
	int CMDListLen;  
	uint8_t **CMDList;		
	int cmdNum;        
	MyFIFO_t fifo;  
	int CMDBufLen;  
	uint8_t *CMDBuf;       
}hexCMD_t;

/**************************************************************
*函数名: P_HexCmdBindFifo
*描述  : stringCMD_t控制器初始化函数
*参数  : scmd ：控制器的指针
*		 fifo : 要绑定到的fifo，外部应该已经初始化完成
*		 data : 保存接收到命令的缓冲区
*		 len :  缓冲区的长度
*		 cmd_list : 外部定义二维数组指令
*		 cmd_list_len :  指令个数
*返回  : =0 成功，<0 失败
**************************************************************/
int P_HexCmdBindFifo(hexCMD_t *userCmd,sky_comDriver* driver,uint8_t *fifoBuf,int fifolen,uint8_t *data,int len,uint8_t **cmd_list,int cmd_list_len);


/**************************************************************
*函数名: P_HexCmdReadCMD
*描述  : 读取一帧完整的数据
*参数  : userCmd ：控制器的指针
*返回  : >=0 代表接收是第几行的指令 <0 数据不完整
**************************************************************/
int P_HexCmdReadCMD(hexCMD_t *userCmd);


/**************************************************************
*函数名: P_HexCmd_Buf_Clear
*描述  : 清理接收数据
*参数  : userCmd ：控制器的指针
*返回  : 无
**************************************************************/
void P_HexCmd_Buf_Clear(hexCMD_t *userCmd);
	

/*************************************************************
 * 函数名 : P_HexCmd_Write
 * 描  述 : 十六进制指令写
 * 参  数 : *userCmd:指令句柄
 * 参  数 : *data:要写的数据(一维数组)
 * 参  数 : data_len:要写的数据长度
 * 返  回 : 无
*************************************************************/
void P_HexCmd_Write(hexCMD_t *userCmd,uint8_t *data,int data_len);



#endif	   

