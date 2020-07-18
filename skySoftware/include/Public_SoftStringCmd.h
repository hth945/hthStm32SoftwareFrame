#ifndef __PUBLIC_SOFTSTRINGCMD_H
#define __PUBLIC_SOFTSTRINGCMD_H 

#include "stdint.h"
#include "myFIFO.h"
#include "commonIOInterface.h"

typedef struct
{
	int cmdNum;     //uartCMD当前长度
	int cmdFlag;    //uartCMD当前接收状态
	MyFIFO_t fifo;   //不再使用fifo， 直接使用通用驱动的中断
	int CMDBufLen;    //缓冲区的长度
	uint8_t *CMDBuf;       //收到命令时存放命令的缓冲区
	
	sky_comDriver *dev;
}stringCMD_t;


/*
*函数名: P_StringCmdBindFifo
*描述  : stringCMD_t控制器初始化函数
*参数  : scmd ：控制器的指针
*		 fifo : 要绑定到的fifo，外部应该已经初始化完成
*		 data : 保存接收到命令的缓冲区
*		 len :  缓冲区的长度
*返回  : =0 成功，<0 失败
*/
int P_StringCmdBindFifo(stringCMD_t *scmd,sky_comDriver* driver, uint8_t *fifoBuf, int fifolen, uint8_t *data, int len);



/*
*函数名: P_StringCmdReadCMD
*描述  : 读取一帧完整的数据，以\r\n做结束符
*参数  : userCmd ：控制器的指针
*返回  : >=0 读取到一帧数据，<0 数据不完整
*/
int P_StringCmdReadCMD(stringCMD_t *userCmd);
	
/*
*函数名: P_StringCmdScan
*描述  : 当前接收的完整的一帧数据，按指定格式解析到变量中，类似标准c的scanf
*参数  : userCmd ：控制器的指针
		 fmt     : 其他，类似scanf。
*返回  : =0 成功，<0 失败
*/
int P_StringCmdScan(stringCMD_t *userCmd,char *fmt, ...);







#endif	   


