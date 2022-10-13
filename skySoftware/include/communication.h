#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "commonIOInterface.h"


#define OFFSETOF(TYPE, MEMBER) ((int)(&((TYPE *)0)->MEMBER))  //��ƫ�Ƶ�ַ
typedef int (*sky_conmmunicationSend)(uint8_t *data,int len);
typedef void (*sky_conmmunicationSendCallback)(void*);
typedef uint32_t (*sky_conmmunicationGetTime)(void);


typedef struct
{
	uint8_t startc;     //��ʼ�ַ�
	uint8_t cmd;
}DataHead;
	

typedef struct
{
	void *userData;       //bsp���Լ�ʹ�õ�ָ��(�ɸı�����), �ⲿӦ�ò㾡����Ҫʹ��           
	void *Parent;         //һ���������ṹ������� sky_comDriver,��Ӧ�ø��˱�����ֵ
	uint8_t saveFlag;	//ͨ�ű�־0 ���ڵȴ�  
						//	1 ���ڽ�������ͷ����ȡ���ݳ���  
						//	2 ����У������  
						//	3 ���յ�һ֡����������
	
	uint8_t startc;       //��ʼ�ַ�
	uint8_t endc;         //�����ַ�
	uint32_t maxlen;      //��������󳤶�
	uint32_t dataLen;     //һ֡������ ���ݶεĳ���
	uint32_t saveNum;     //����һ֡���ݹ�����  ��ǰ���ճ���
	uint8_t *data;        //��������ַ
	uint32_t oldTime;     //��һ�ν��յ����ݵ�ʱ��
	uint32_t waitTimeT;   //�ȴ�ʱ��ľ���
	
	sky_comDriver *dev;   //ͨ����������ṹ������Ĭ��ʹ�ã���Ϊ�� ��ʹ�þɵ�send��
	
	//sky_conmmunicationSend send;  //�������ݰ���ʵ���������֮�󣬲���ʹ���ڲ���������֡�ĺ���
									 //data Ҫ���͵�����
									 //len  Ҫ���͵����ݳ���
									 //����ֵ  ������0����ɹ�  ����������ʧ��
	sky_conmmunicationSendCallback completeCallback; //���յ�һ֡��������ʱ�Ļص�����
													 //�˺����ڽ��պ���sky_receivePact�б�ִ��
	
	sky_conmmunicationGetTime getTime;//��ȡ��ǰϵͳʱ�ӣ���λms
	
	
//	/*���º���ָ�����������ɹ��������� ȡ��ע��*/
//	int (*receivePact)(void *this,uint8_t c);           					//�����������  �ײ�У��
//	int (*sendPact)(void *this,uint8_t cmd,uint8_t *data,uint32_t len);  			//�������ݰ�
//	int (*runCmdPact)(void *this,uint8_t cmd,uint8_t *data,uint32_t len,uint32_t time);	//�������� �������ݷ��ڻ�������
}sky_MyByteReceiveDataTypeDef;


int sky_ConmBindDriver(sky_MyByteReceiveDataTypeDef *myRecType,sky_comDriver *driver);
void sky_ConmmunicationInit(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t *data,uint32_t len,uint8_t startC,uint8_t endC,uint32_t waitTimeT,sky_conmmunicationGetTime getTime);
	
int sky_receivePactData(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t *data, int len);
/********************************************************************************
*��������receivePact
*����  ����Э�����һ֡���ݣ���������ж��У�ÿ�δ���һ���ֽڣ���У���볬ʱ�ж�50ms
		����һ֡����û�д����꣨��־λû����0�����򲻽��������ݡ�
*����  ��1.myRecType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.newc       ��ʱ���յ���һ���ֽ�
*����  ������ͨ��״̬��־  0 ���ڵȴ�  
						1 ���ڽ�������ͷ����ȡ���ݳ���  
						2 ����У������  
						3 ���յ�һ֡����������
********************************************************************************/
int sky_receivePact(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t newc);

/********************************************************************************
*��������sendData
*����  ����Э�鷢��һ֡����,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
*����  ��1.recType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.cmd       �������ݰ�������
		 3.data     ���͵�����
		 4.len       ���͵����ݳ���
*����  ������ ���͵����ݳ���  -1���ʹ���
********************************************************************************/
int sky_sendData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data,uint32_t len);

/*******************************************************************************
 * ������ : pcui_sendData
 * ��  �� : pcui��������
 * ��  �� : *recType:ͨѶЭ��ṹ��
 * ��  �� : cmd:ָ��
 * ��  �� : **str:�ַ���ָ��
 * ��  �� : strLen:�ַ�������
 * ��  �� : >=0:���ͳɹ�
            -1:����ʧ��
*******************************************************************************/
int sky_sendStrNData(sky_MyByteReceiveDataTypeDef *recType, uint8_t cmd, char **str, int strLen);

/********************************************************************************
*��������send2Data
*����  ����Э�鷢��һ֡����,���ݶη�Ϊ����,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
*����  ��1.recType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.cmd       �������ݰ�������
		 3.data1     ���͵ĵ�һ������
		 4.len1       ���͵ĵ�һ�����ݳ���
		 5.data2     ���͵ĵڶ�������
		 6.len2       ���͵ĵڶ������ݳ���
*����  ������ ���͵����ݳ���  -1���ʹ���
********************************************************************************/
int sky_send2Data(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data1,uint32_t len1,uint8_t *data2,uint32_t len2);

/********************************************************************************
*��������runCmdPact
*����  ������һ��������ȴ�����,���������ڽṹ����,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
*ע��  ���������к������ڽṹ���У���������һֱ���գ���������0 �ṹ���еı�־
*����  ��1.recType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.cmd       �������ݰ�������
		 3.data     ���͵�����
		 4.len       ���͵����ݳ���
		 5.time     ��ʱʱ��
*����  ��1 ִ�гɹ�  -1 ��ʱ   -2 ִ�д���
********************************************************************************/
int sky_runCmdPact(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data,uint32_t len,uint32_t time);


/********************************************************************************
*��������recOffsetType
*����  ����Э�鷢��һ֡����,���ݶη����ֽ�ƫ�ƣ����ֽڳ��ȣ������θ���tpye�ж�����������
		������Ϊ �������� ���Ƿ�������,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
*ע��  ���˺���ֻ����  ���յ���������ô�����볬ʱ ���ⲿ����
*����  ��1.recType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.data      ���ݵĻ���ַ
		 4.offset    Ҫ�������ݵ�ƫ��
		 5.len       Ҫ�������ݵĳ���
		 5.cmd       �������ݰ�������
		 6.type      �Ƿ��;��������  1����  0������
*����  ������ ���͵����ݳ���  -1���ʹ���
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

