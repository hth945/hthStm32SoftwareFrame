#ifndef __PUBLIC_SOFTSTRINGCMD_H
#define __PUBLIC_SOFTSTRINGCMD_H 

#include "stdint.h"
#include "myFIFO.h"
#include "commonIOInterface.h"

typedef struct
{
	void *userData;       //bsp���Լ�ʹ�õ�ָ��(�ɸı�����), �ⲿӦ�ò㾡����Ҫʹ��           
	void *Parent;         //һ���������ṹ������� sky_comDriver,��Ӧ�ø��˱�����ֵ
	sky_comDriver *dev;
	
	int cmdNum;     //uartCMD��ǰ����
	int cmdFlag;    //uartCMD��ǰ����״̬
	MyFIFO_t fifo;   //����ʹ��fifo�� ֱ��ʹ��ͨ���������ж�
	int CMDBufLen;    //�������ĳ���
	uint8_t *CMDBuf;       //�յ�����ʱ�������Ļ�����

}stringCMD_t;


/*
*������: P_StringCmdBindFifo
*����  : stringCMD_t��������ʼ������
*����  : scmd ����������ָ��
*		 fifo : Ҫ�󶨵���fifo���ⲿӦ���Ѿ���ʼ�����
*		 data : ������յ�����Ļ�����
*		 len :  �������ĳ���
*����  : =0 �ɹ���<0 ʧ��
*/
int P_StringCmdBindFifo(stringCMD_t *scmd,sky_comDriver* driver, uint8_t *fifoBuf, int fifolen, uint8_t *data, int len);



/*
*������: P_StringCmdReadCMD
*����  : ��ȡһ֡���������ݣ���\r\n��������
*����  : userCmd ����������ָ��
*����  : >=0 ��ȡ��һ֡���ݣ�<0 ���ݲ�����
*/
int P_StringCmdReadCMD(stringCMD_t *userCmd);
	
/*
*������: P_StringCmdScan
*����  : ��ǰ���յ�������һ֡���ݣ���ָ����ʽ�����������У����Ʊ�׼c��scanf
*����  : userCmd ����������ָ��
		 fmt     : ����������scanf��
*����  : =0 �ɹ���<0 ʧ��
*/
int P_StringCmdScan(stringCMD_t *userCmd,char *fmt, ...);







#endif	   


