#ifndef _PUBLIC_SOFTFUNPACK_H_
#define _PUBLIC_SOFTFUNPACK_H_

#include "stdint.h"

typedef	struct{ 
	void* func;			    //����ָ��
	const char* name;		//������(���Ҵ�)	 
}funpack_funNametab;


typedef	struct{
	uint8_t type;     //1��ͨ����  2����ṹ�� 3����ṹ�� 
	int len;
	uint32_t value;  //��������
	uint8_t *data;   //�ṹ���ָ��
}funpack_parameter;

/****************************************************
*funName : ������
*data    : ���������ݻ�����
*pt      : �����Ĳ����ṹ�����嶨�忴�ṹ��
*len     : �����Ĳ�������
****************************************************/
int P_doPack(uint8_t *funName,uint8_t *data, funpack_parameter *pt, int len);

/****************************************************
*funName : ������
*data    : ���������ݻ�����
*pt      : �����Ĳ����ṹ�����嶨�忴�ṹ��
*len     : �����Ĳ�������
*alignOff: ���ջ��������ֽڶ���ƫ�ƣ���д�ͻ��˵��ˣ�
****************************************************/
int P_doPackByalignOff(uint8_t *funName,uint8_t *data, funpack_parameter *pt, int len, int alignOff);

int P_unPack(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen);


int P_unPackASM(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen);
int P_unPackASM_fpu(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen);



/*

//������0��β
struct _m_usmart_nametab usmart_nametab[]=
{
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	
	0,0
};





*/

#endif 

