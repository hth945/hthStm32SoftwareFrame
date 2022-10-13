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
*������: P_HexCmdBindFifo
*����  : stringCMD_t��������ʼ������
*����  : scmd ����������ָ��
*		 fifo : Ҫ�󶨵���fifo���ⲿӦ���Ѿ���ʼ�����
*		 data : ������յ�����Ļ�����
*		 len :  �������ĳ���
*		 cmd_list : �ⲿ�����ά����ָ��
*		 cmd_list_len :  ָ�����
*����  : =0 �ɹ���<0 ʧ��
**************************************************************/
int P_HexCmdBindFifo(hexCMD_t *userCmd,sky_comDriver* driver,uint8_t *fifoBuf,int fifolen,uint8_t *data,int len,uint8_t **cmd_list,int cmd_list_len);


/**************************************************************
*������: P_HexCmdReadCMD
*����  : ��ȡһ֡����������
*����  : userCmd ����������ָ��
*����  : >=0 ��������ǵڼ��е�ָ�� <0 ���ݲ�����
**************************************************************/
int P_HexCmdReadCMD(hexCMD_t *userCmd);


/**************************************************************
*������: P_HexCmd_Buf_Clear
*����  : �����������
*����  : userCmd ����������ָ��
*����  : ��
**************************************************************/
void P_HexCmd_Buf_Clear(hexCMD_t *userCmd);
	

/*************************************************************
 * ������ : P_HexCmd_Write
 * ��  �� : ʮ������ָ��д
 * ��  �� : *userCmd:ָ����
 * ��  �� : *data:Ҫд������(һά����)
 * ��  �� : data_len:Ҫд�����ݳ���
 * ��  �� : ��
*************************************************************/
void P_HexCmd_Write(hexCMD_t *userCmd,uint8_t *data,int data_len);



#endif	   

