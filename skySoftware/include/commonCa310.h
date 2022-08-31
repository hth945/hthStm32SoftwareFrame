/********************************* SKYCODE ************************************
* Copyright (c) 2017-2022, ����˼�����Լ������޹�˾
* All rights reserved.
*
* �ļ����� : commonCa310.h   
* ����ժҪ : ��ѧ̽ͷ�����ײ�����������
* ��ǰ�汾 : V1.8
* ��    �� : Optics group
* ������� : 2022-5-24
*
* ��    �� ��V1.8
* ��    �� ��Optics group
* �޸����� ��2022-5-24
* �޸����� ��a.���Ӳ�ѯCA310 TcpduvLv����
*           b.���Ӳ�ѯCA310 uvLv����
*           c.��������CA310�����ٶ�
*           d.��������CA310ͬ��Ƶ��
*           e.�Ż�ע��,ͻ���ص�    
*
* ��    �� ��V1.7
* ��    �� ��Optics group
* �޸����� ��2022-3-30
* �޸����� ��a.���Ӳ�ѯCA410 ID����
*           b.���Ӳ�ѯCA410ɫ�¹���
*           c.���Ӳ�ѯCA410 uvLv����
*           d.���Ӳ�ѯCA410 flick dB����
*
* ��    �� ��V1.6
* ��    �� ��Optics group
* �޸����� ��2022-2-19
* �޸����� ��a.����CA410�����ٶ�ģʽ����
*           b.���CA310 CA410��У׼ͨ��ʧ��
*
* ��    �� ��V1.5
* ��    �� ��Optics group
* �޸����� ��2022-1-19
* �޸����� ��a.�޸�����ע��,ɾ��CA410���ֶ������
*
* ��    �� ��V1.3
* ��    �� ��Optics group
* �޸����� ��2021-9-29
* �޸����� ��a.�޸�CA410�趨ͬ��Ƶ�ʺ�����С�����λ�����ർ�´���
*
* ��    �� ��V1.2
* ��    �� ��Optics group
* �޸����� ��2021-9-7
* �޸����� ��a.����CA410��ѯ,����ͨ���ź���
*           b.����C33��ѯINTͬ��Ƶ�ʺ���
*           c.�������к���ע��
*           d.����CA410��ѯINTͬ��Ƶ�ʺ���
*
* ��    �� ��V1.1
* ��    �� ��
* �޸����� ��2021-9-6
* �޸����� ��a.�޸�CA310,MSE��ѯͨ������
*           b.��������INTģʽͬ��Ƶ�ʺ���
*           c.���Ӳ��ֺ���ע��
*
* ��    �� ��V1.0
* ��    �� ��
* �޸����� ��2021-8-30
* �޸����� ��a.�������ò���CA310, MSE�ڴ�ͨ���ӿں���
*           b.���Ӳ�ѯ����CA310, MSE�ڴ�ͨ���ӿں���
*               
******************************************************************************/

#ifndef __COMMONCA310_H
#define	__COMMONCA310_H

#include "commonIOInterface.h"


typedef struct
{
	uint8_t PNumberFlag;        //��¼��һ�ζ�ȡ�����ĸ�̽ͷ
	MyFIFO_t ca310FIFO;    //ͨ��ʹ�õ�FIFO
	char caFIFOBuf[1024];  //FIFOʹ�õ��Ļ�����
	char recive[1024];   //�����ַ������ٱ������п���ջ�����
	
	/**************ʹ�õ���Ӳ������***************/
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
}Ca310Fun;  //���η�װ�ӿ� Ϊ�����豸��ȡ310Ȼ����ʹ��

/**
  * �������� : ��������̽ͷ��Ϣ�ṹ��
  * ������� : 1. ca310 -- CA310̽ͷ��Ϣ�ṹ
  *           2. driver -- �����ṹ��
  * �� �� ֵ : = 1 -- �ɹ�,  < 0 -- ʧ��
  */
int COMCa310InterfaceInit(Ca310Interface *ca310,sky_comDriver *driver);

/**=================================================================================
 *                      ###  CA-310ͨ�����ù��ܺ���  ###
 * =================================================================================
 * @{
 */

/**
  * �������� : ��ʼ��CA-310̽ͷ
  * ������� : 1. ca310 -- CA310̽ͷ��Ϣ�ṹ
  *           2. ch -- �ڴ�ͨ��,ֵ��0 ~ 99
  *           3. sync -- ͬ��ģʽ, 0��NTSC  1��PAL  2��EXT  3��UNIV
  * �� �� ֵ : = 1 -- ��ʼ���ɹ�,  < 0 -- ��ʼ��ʧ��
  */
int COMCA310_Init(Ca310Interface *ca310, int ch,int sync);

/**
  * �������� : ��ѯCA310̽ͷ���
  * ������� : ca310 -- CA310̽ͷ��Ϣ�ṹ
  * �� �� ֵ : ̽ͷ���
  * ע������ : ������CA-310��̽ͷ����        
  */
int COMCA310_QueryPNumber(Ca310Interface *ca310);

/**
  * �������� : У��CA-310̽ͷ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  * �� �� ֵ : = 1 -- У��ɹ�,  < 0 -- У��ʧ��
  */
int COMCA310_Cal0(Ca310Interface *ca310);

/**
  * �������� : ��ѯx, y, Lv
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. Lv -- ����ֵ
  *           2. X -- XYɫ��ͼ��X����  
  *           3. Y -- XYɫ��ͼ��Y����
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y);

/**
  * �������� : ��ѯTcp, duv, Lv
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. Tcp -- ɫ��
  *           2. duV -- ɫƫ��ֵ
  *           3. Lv -- ����ֵ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetTcpduvLv(Ca310Interface *ca310, uint8_t PN, float* Tcp, float* duv, float* Lv);

/**
  * �������� : ��ѯu, v, Lv
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. u  -- uֵ
  *           2. v  -- vֵ
  *           3. Lv -- ����ֵ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetuvLv(Ca310Interface *ca310, uint8_t PN, float* u, float* v, float* Lv);

/**
  * �������� : ��ѯX, Y, Z 
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2.PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. X -- ���̼�ֵ֮X
  *           2. Y -- ���̼�ֵ֮Y 
  *           3. Z -- ���̼�ֵ֮Z
  * �� �� ֵ := 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z);

/**
  * �������� : ��ѯ��˸ֵ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ := 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetFMA(Ca310Interface *ca310, uint8_t PN,float* FMA);

/**
  * �������� : ��ѯCA310̽ͷΨһ���к�
  * ������� : 1.ca310 -- CA310̽ͷ��Ϣ�ṹ
  *           2.PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1.id -- ̽ͷΨһID
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetSerialNO(Ca310Interface *ca310, uint8_t PN,int* id);

/**
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������
  * ������� : 1. CH -- CA310̽ͷͨ���Ż�����
  *              ��ѯͨ��ֵ��Ϊ0 ~ 99,��Ӧͨ��CH00 ~ CH99
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH);

/**
  * �������� : ���ò���ͨ����
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������
  *           3. CH -- CA310̽ͷͨ����,ֵ��Ϊ0 ~ 99,����ͨ��CH00 ~ CH99
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA310_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH);

/**
  * �������� : ���ò�������ģʽ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������
  *           3. speed -- 0:SLOW  1:FAST 2:AUTO 
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA310_SetMeasureSpeed(Ca310Interface *ca310, uint8_t PN, int speed);

/**
  * �������� : ����ͬ��Ƶ��
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN  -- ��������
  *           3. fre -- ͬ��Ƶ��,ֵ��Χ40.0 ~ 200.0,����ΪС�����һλ
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA310_SetINTFrequency(Ca310Interface *ca310, uint8_t PN, float fre);

/** @}
  * @end of CA310
  */


/**=================================================================================
 *                      ###  CA-410ͨ�����ù��ܺ���  ###
 * =================================================================================
 * @{
 */
 
/**
  * �������� : ��ʼ��CA-410̽ͷ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. ch -- �ڴ�ͨ��,ֵ��0 ~ 99
  *           3. sync -- ͬ��ģʽ, 0��NTSC  1��PAL  2��EXT  3��UNIV
  * �� �� ֵ : = 1 -- ��ʼ���ɹ�,  < 0 -- ��ʼ��ʧ��
  */
int COMCA410_Init(Ca310Interface *ca310, int ch,int sync);

/**
  * �������� : У��CA410̽ͷ
  * ������� : 1. ca410 -- CA410̽ͷ��Ϣ�ṹ
  * �� �� ֵ : = 1 -- У��ɹ�,  < 0 -- У��ʧ��
  */
int COMCA410_Cal0(Ca310Interface *ca410);

/**
  * �������� : ��ѯCA410ΨһID
  * ������� : 1. ca410 -- CA410̽ͷ��Ϣ�ṹ
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 1. id -- ����ֵ,��λcd/m^2
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMCA410_GetSerialNO(Ca310Interface *ca410, uint8_t PN, int* id);

/**
  * �������� : ��ѯx, y, Lv
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. Lv -- ����ֵ,��λcd/m^2
  *            2. X -- XYɫ��ͼ��X����  
  *            3. Y -- XYɫ��ͼ��Y����
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y);

/**
  * �������� : ��ѯTcp, duv, Lv
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. Tcp -- ɫ��
  *            2. duV -- ɫƫ��ֵ
  *            3. Lv -- ����ֵ,��λcd/m^2
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetTcpduvLv(Ca310Interface *ca410, uint8_t PN,float* Tcp,float* duv,float* Lv);

/**
  * �������� : ��ѯu, v, Lv
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 1. u -- ɫ��uֵ
  *           2. v -- ɫ��vֵ
  *           3. Lv -- ����ֵ,��λcd/m^2
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetuvLv(Ca310Interface *ca410, uint8_t PN, float* u, float* v, float* Lv);

/**
  * �������� : ��ѯX, Y, Z
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. X -- ���̼�ֵ֮X
  *            2. Y -- ���̼�ֵ֮Y 
  *            3. Z -- ���̼�ֵ֮Z
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z);

/**
  * �������� : ��ѯflick �ٷ�ֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA);

/**
  * �������� : ��ѯflick dBֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetdB(Ca310Interface *ca410, uint8_t PN, float* dB);

/**
  * �������� : ����INTģʽ�µ�ͬ��Ƶ��
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  *            3. fre -- ͬ��Ƶ��,���֧��С�������λ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_SetINTFrequency(Ca310Interface *ca410, uint8_t PN, float fre);

/**
  * �������� : ��ѯINTģʽ�µ�ͬ��Ƶ��
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. fre -- ͬ��Ƶ��
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetINTFrequency(Ca310Interface *ca410, uint8_t PN, float* fre);

/**
  * �������� : ��ѯINTģʽ�µ�ͬ��Ƶ��
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. fre -- ͬ��Ƶ��
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  * ע������ : �˺�������C33ʹ��
  */
int COMCA410_OnlyC33_GetINTFrequency(Ca310Interface *ca410, uint8_t PN, float* fre);

/**
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. CH -- CA410̽ͷͨ���Ż�����
  *               ��ѯͨ��ֵ��Ϊ0 ~ 99,��Ӧͨ��CH00 ~ CH99
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH);

/**
  * �������� : ���ò���ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  *            3. CH -- CA410̽ͷͨ����,ֵ��Ϊ0 ~ 99,����ͨ��CH00 ~ CH99
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA410_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH);
	
/**
  * �������� : ���ò����ٶ�
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  *            3. speedmode -- CA410�����ٶ�ģʽ
  *               0.SLOW 1.FAST 2.LTD.AUTO 3.AUTO
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA410_SetMeasureSpeed(Ca310Interface *ca410, uint8_t PN, int speedmode);
  
/** @}
  * @end of CA410
  */


/**=================================================================================
 *                      ###  MSEͨ�����ù��ܺ���  ###
 * =================================================================================
 * @{
 */
 
/**
  * �������� : COMMSE_Init(Ca310Interface *ca310, int ch,int sync)
  * �������� : ��ʼ������MSE̽ͷ
  * ������� : 1. ca310 -- CA410̽ͷ��Ϣ�ṹ
  *           2. ch -- δʹ��
  *           3. sync -- δʹ��
  * �� �� ֵ : ����״̬
  *           1. = 1 -- �ɹ�  2. < 0 -- ʧ��
  */
int COMMSE_Init(Ca310Interface *ca310, int ch,int sync);

/**
  * �������� : COMMSE_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y)
  * �������� : ��ѯɫ���꼰����ֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 1. Lv -- ����ֵ,��λcd/m^2
  *           2. X -- XYɫ��ͼ��X����  
  *           3. Y -- XYɫ��ͼ��Y����
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMMSE_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y);

/**
  * �������� : COMMSE_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z)
  * �������� : ��ѯ���̼�ֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 1. X -- ���̼�ֵ֮X
  *           2. Y -- ���̼�ֵ֮Y 
  *           3. Z -- ���̼�ֵ֮Z
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMMSE_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z);

/**
  * �������� : COMMSE_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA)
  * �������� : ��ѯ��˸ֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMMSE_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA);

/**
  * �������� : COMMSE_GetSN(Ca310Interface *ca410, uint8_t PN,char* SN)
  * �������� : ��ѯ���к�
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 3. SN -- MSE̽ͷΨһ���к�
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMMSE_GetSN(Ca310Interface *ca410, uint8_t PN,char* SN);//��ȡ̽ͷ���

/**
  * �������� : COMMSE_GetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 3. CH -- MSE̽ͷͨ�����ַ���
  *                     a."factory"   b."user1" ~ "user30"  c."off" 
  * �� �� ֵ : ��ѯ״̬
  *            1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  * ע������ : �˺�����COMMSE_GetMeasureCH��ͻ,����ͬʱʹ��,����ʹ��
  *            COMMSE_GetMeasureCH()
  */
int COMMSE_GetCH(Ca310Interface *ca410, uint8_t PN,char* CH);

/**
  * �������� : COMMSE_SetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
  * �������� : ���ò���ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  *           3. CH -- MSE̽ͷͨ�����ַ���
  *                    a."factory\r"   b."user1\r" ~ "user30\r"  c."off\r" 
  * �� �� ֵ : ����״̬
  *           1. = 1 -- ���óɹ�  2. < 0 -- ����ʧ��
  * ע������ : �˺�����COMMSE_SetMeasureCH��ͻ,����ͬʱʹ��,����ʹ��
  *            COMMSE_SetMeasureCH()
  */
int COMMSE_SetCH(Ca310Interface *ca410, uint8_t PN,char* CH);

/**
  * �������� : COMMSE_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH)
  * �������� : ���ò���ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  *           3. CH -- MSE̽ͷͨ����
  *                    a.0 -- factoryͨ��ģʽ    b.1 ~ 30 -- userͨ��ģʽ
  *                    c.31 -- offͨ��ģʽ
  * �� �� ֵ : ����״̬
  *            1. = 1 -- ���óɹ�  2. < 0 -- ����ʧ��
  * ע������ : �˺�����COMMSE_SetCH()��ͻ,����ͬʱʹ��
  */
int COMMSE_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH);


/**
  * �������� : COMMSE_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH)
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 1. CH -- MSE̽ͷͨ����
  *                    a.0 -- factoryͨ��ģʽ    b.1 ~ 30 -- userͨ��ģʽ
  *                    c.31 -- offͨ��ģʽ
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  * ע������ : �˺�����COMMSE_GetCH()��ͻ,����ͬʱʹ��
  */
int COMMSE_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH);

/** @}
  * @end of MSE
  * ===============================================================================
  */
  
/**=================================================================================
*                      ###  C32ͨ�����ù��ܺ���  ###
* =================================================================================
* @{
* @brief  C32�̼�����V2.2����Զ��ͨ�ź�����쳣����ʱ,�޷��Ծ���,�ᵼ��ͨ�Ź����п���C32,��
*         ��Զ��ͨ���޷�ʹ��.������BUG��ʽΪ����Զ��ͨ�ź�,���ʹ���ָ��һ�λ�����������.����
*         ��ͨ�Ź������쳣�Ͽ���������,���±���ͨ���쳣,�������ܾ��޷�����ͨ��.���ָ������ֻ
*         ������C32,���½������.
*         
*/

/**
 * �������� : COMC32_Init(Ca310Interface *ca310, int ch,int sync)
 * �������� : ��ʼ��CA-310̽ͷ
 * ������� : 1. ca310 -- CA310̽ͷ��Ϣ�ṹ
 *           2. ch -- �ڴ�ͨ��,ֵ��0 ~ 31
 *           3. sync -- ����
 * �� �� ֵ : ����״̬
 *           1. = 1 -- �ɹ�  2. = -1 -- ʧ��
 */
int COMC32_Init(Ca310Interface *ca310, int ch,int sync);


/**
  * �������� : COMC32_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y)
  * �������� : ��ѯɫ���꼰����ֵ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ����
  * ������� : 1. Lv -- ����ֵ,��λcd/m^2
  *           2. X -- XYɫ��ͼ��X����  
  *           3. Y -- XYɫ��ͼ��Y����
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. = -1 -- ��ѯʧ��
  */
int COMC32_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y);


/**
  * �������� : COMC32_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z)
  * �������� : ��ѯ���̼�ֵ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ����
  * ������� : 1. X -- ���̼�ֵ֮X
  *           2. Y -- ���̼�ֵ֮Y 
  *           3. Z -- ���̼�ֵ֮Z
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. = -1 -- ��ѯʧ��
  */
int COMC32_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z);


/**
  * �������� : COMC32_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA)
  * �������� : ��ѯ��˸ֵ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ����
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. = -1 -- ��ѯʧ��
  */
int COMC32_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA);


/**
  * �������� : COMC32_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH)
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ����
  * ������� : 1. CH -- CA310̽ͷͨ����
  *               ��ѯͨ��ֵ��Ϊ0~31
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. = -1 -- ��ѯʧ��
  */
int COMC32_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH);


/**
  * �������� : COMC32_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH)
  * �������� : ���ò���ͨ����
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ����
  *           3. CH -- CA310̽ͷͨ����,ֵ��Ϊ0~31
  * �� �� ֵ : ����״̬
  *           1. = 1 -- ���óɹ�  2. = -1 -- ����ʧ��
  */
int COMC32_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH);

/** @}
  * @end of C32
  * ===============================================================================
  */
  
#endif

