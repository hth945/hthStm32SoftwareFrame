#ifndef _PUBLIC_SOFTFUNPACK_H_
#define _PUBLIC_SOFTFUNPACK_H_

#include "stdint.h"

typedef	struct{ 
	void* func;			    //函数指针
	const char* name;		//函数名(查找串)	 
}funpack_funNametab;


typedef	struct{
	uint8_t type;     //1普通数据  2输入结构体 3输出结构体 
	int len;
	uint32_t value;  //正常数据
	uint8_t *data;   //结构体或指针
}funpack_parameter;

/****************************************************
*funName : 函数名
*data    : 打包后的数据缓冲区
*pt      : 函数的参数结构，具体定义看结构体
*len     : 函数的参数个数
****************************************************/
int P_doPack(uint8_t *funName,uint8_t *data, funpack_parameter *pt, int len);

/****************************************************
*funName : 函数名
*data    : 打包后的数据缓冲区
*pt      : 函数的参数结构，具体定义看结构体
*len     : 函数的参数个数
*alignOff: 接收缓冲区的字节对齐偏移（问写客户端的人）
****************************************************/
int P_doPackByalignOff(uint8_t *funName,uint8_t *data, funpack_parameter *pt, int len, int alignOff);

int P_unPack(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen);


int P_unPackASM(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen);
int P_unPackASM_fpu(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen);



/*

//必须以0结尾
struct _m_usmart_nametab usmart_nametab[]=
{
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	
	0,0
};





*/

#endif 

