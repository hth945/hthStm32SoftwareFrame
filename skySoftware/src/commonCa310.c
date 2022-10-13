/********************************* SKYCODE ************************************
* Copyright (c) 2017-2022, ����˼�����Լ������޹�˾
* All rights reserved.
*
* �ļ����� : commonCa310.c    
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
*           b.�޸�COMCA310_Init(...)ͨ�ŵȴ�ʱ��
*
* ��    �� ��V1.4
* ��    �� ��Optics group
* �޸����� ��2021-11-10
* �޸����� ��a.���ӻ���Դ��C32��������
*           b.�޸Ĳ��ֺ�����������У���ַ�"OK00"Ϊ"OK"
*
* ��    �� ��V1.3
* ��    �� ��Optics group
* �޸����� ��2021-9-29
* �޸����� ��a.�޸�CA410�趨ͬ��Ƶ�ʺ�����С�����λ�����ർ�´���
*
*
* ��    �� ��V1.2
* ��    �� ��Optics group
* �޸����� ��2021-9-7
* �޸����� ��a.����CA410��ѯ,����ͨ���ź���
*            b.����C33��ѯINTͬ��Ƶ�ʺ���
*            c.�������к���ע��
*            d.����CA410��ѯINTͬ��Ƶ�ʺ���
*
*
* ��    �� ��V1.1
* ��    �� ��Optics group
* �޸����� ��2021-9-6
* �޸����� ��a.�޸�CA310,MSE��ѯͨ����������ͨ���Ŵ���,�Ż�����
*            b.��������INTģʽͬ��Ƶ�ʺ���
*            c.���Ӳ��ֺ���ע��
*
*
* ��    �� ��V1.0
* ��    �� ��
* �޸����� ��2021-8-30
* �޸����� ��a.�������ò���CA310, MSE�ڴ�ͨ���ӿں���
*            b.���Ӳ�ѯ����CA310, MSE�ڴ�ͨ���ӿں���
*               
******************************************************************************/

#include "commonCa310.h"
#include <string.h>
#include "delay.h"
#include "stdlib.h"

/**
  * �������� : ��������̽ͷ��Ϣ�ṹ��
  * ������� : 1. ca310 -- CA310̽ͷ��Ϣ�ṹ
  *            2. driver -- �����ṹ��
  * �� �� ֵ : ��״̬
  *            1. = 1 -- �ɹ�  2. = -1 -- ʧ��
  */
int COMCa310InterfaceInit(Ca310Interface *ca310, sky_comDriver *driver)
{
	memset(ca310,0,sizeof(Ca310Interface));
	myFIFOInit(&ca310->ca310FIFO,(void *)ca310->caFIFOBuf,sizeof(ca310->caFIFOBuf));
	ca310->driver = driver;
	ca310->driver->fifo = &ca310->ca310FIFO;
	return 0;
}


/**
  * �������� : ִ������������,���������FIFO����
  * ������� : 1. ca310 -- CA310̽ͷ��Ϣ�ṹ
  *           2. com -- �ַ�������,��������1024
  *           3. rec -- FIFO��ȡ������
  *           4. time -- ��ʱ�ȴ�ʱ��,��λms
  * �� �� ֵ : ������Ӧ״̬
  *           1. = 1 -- ��Ӧ�ɹ�  2. = -1 -- ��Ӧʧ��
  */
static int COMRunCommond(Ca310Interface *ca310, char* com,char* rec,uint32_t time)
{
	char *buf=com;
	
	if (ca310->driver == 0)
		return -1;
	
	myFIFORead(&ca310->ca310FIFO, (void *)ca310->recive, sizeof(ca310->recive));
	//ca310->driver->write((uint8_t *)buf,strlen(buf));
	devWrite(ca310->driver, (uint8_t *)buf,strlen(buf));
	
	while(time--)
	{
		if(myFIFORead(&ca310->ca310FIFO, (void *)rec, 1) > 0)
		{
			*rec = (0x7f & (*rec));
			/* ͨ�����������'\r' */
			if(*rec==0x0d)
			{
				rec++;
				*rec=0;
				return 1;
			}
			rec++;
		}
		
		Delay_ms(1); 		
	}
	
	strcpy(ca310->recive,"ER:timeOut");
	return -1;
}

/**
  * �������� : ��ȡ̽ͷ��Ϣ�ṹFIFO��������Ϣ
  * ������� : 1. ca310 -- CA310̽ͷ��Ϣ�ṹ
  *            2. rec -- FIFO��ȡ������
  *            3. time -- ��ʱ�ȴ�ʱ��,��λms
  * �� �� ֵ : ��״̬
  *            1. = 1 -- �ɹ�  2. = -1 -- ʧ��
  * ע������ : ���ڸ��ļ���ʹ��
  */
static int COMRunCommond_getData(Ca310Interface *ca310, char* rec,uint32_t time)
{
	if (ca310->driver == 0)
		return -1;
	while(time--)
	{
		if(myFIFORead(&ca310->ca310FIFO, (void *)rec, 1) > 0)
		{
			if(*rec==0x0d)
			{
				rec++;
				*rec=0;
				return 1;
			}
			rec++;
		}
		
		Delay_ms(1); 		
	}
	strcpy(ca310->recive,"ER:timeOut");
	return -1;
}

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
int COMCA310_Init(Ca310Interface *ca310, int ch, int sync)
{
	char sbuf[10];
	int i = 0;
//    uint32_t id;
	if (ca310->driver == 0)
		return -1;
	
	while(1)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); 
		if(ca310->recive!=strstr(ca310->recive,"OK")) 
		{ 
			i++;
			if (i > 3)
				return -1;
		}
		else
		{
			break;
		}
	}
	
	/* ����ģʽѡ�� */
	COMRunCommond(ca310, "FSC,2\r",ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}//ͨ��ģʽ �Զ�
	
	/* �ڴ����ͨ��ѡ�� */
	if (ch >= 0)
	{
		sprintf(sbuf,"MCH,%d\r",ch);
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;} 
	}
	
	if (sync >= 0)
	{
		sprintf(sbuf,"SCS,%d\r",sync);
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;} 
	}
	
	return 1;
}

/**
  * �������� : ��ѯCA310̽ͷ���
  * ������� : ca310 -- CA310̽ͷ��Ϣ�ṹ
  * �� �� ֵ : ���̽ͷ��
  * ע������ : ������CA-310��̽ͷ����        
  */
int COMCA310_QueryPNumber(Ca310Interface *ca310)
{
	uint8_t PNumber = 0;
	uint8_t i;
	
	if (ca310->driver == 0)
		return -1;
	
	COMRunCommond(ca310, "OPR,0\r",ca310->recive,1000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	COMRunCommond(ca310, "MDS,0\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	i = ca310->recive[6] - 0x31;
	if(i > 4) return -3;
	PNumber |= (1 << i);
	while(1)
	{
		COMRunCommond_getData(ca310, ca310->recive,1000);if(ca310->recive!=strstr(ca310->recive,"OK")) {break;}
		i = ca310->recive[6] - 0x31;
		if(i > 4) return -3;
		PNumber |= (1 << i);
	}
	
	return PNumber;
}

/**
  * �������� : У��CA-310̽ͷ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  * �� �� ֵ : = 1 -- У��ɹ�,  < 0 -- У��ʧ��
  */
int COMCA310_Cal0(Ca310Interface *ca310)
{
	int comm_cnt = 0;
	
	if (ca310->driver == 0)
		return -1;
	
	/* ��ʼԶ��ͨ�� */
	while(1)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,3000); 
		if(ca310->recive!=strstr(ca310->recive,"OK")) { 
			comm_cnt++;
			if (comm_cnt > 3)
				return -1;
		}
		else {
			break;
		}
	}
	/* У�� */
    COMRunCommond(ca310, "ZRC\r",ca310->recive,5000);  
    if(ca310->recive!=strstr(ca310->recive,"OK")) 
    { 
    	return -2;
    }
	
	return 1;
}

/**
  * �������� : ��ѯx, y, Lv
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. Lv -- ����ֵ
  *           2. X -- XYɫ��ͼ��X����  
  *           3. Y -- XYɫ��ͼ��Y����
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			X[len] = 0;
			Y[len] = 0;
			Lv[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,0\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf)/10000;  //X��Y�Ŵ���һ�򱶴��ݻ�����
	buf=strtok(NULL,";");
	*Y=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		X++;
		Y++;
		Lv++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*X=(float)atof(buf)/10000;  //X��Y�Ŵ���һ�򱶴��ݻ�����
		buf=strtok(NULL,";");
		*Y=(float)atof(buf)/10000;
		buf=strtok(NULL,";");
		*Lv=(float)atof(buf);
	}
	return 1;	
}

/**
  * �������� : ��ѯTcp, duv, Lv
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. Tcp -- ɫ��
  *           2. duV -- ɫƫ��ֵ
  *           3. Lv -- ����ֵ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetTcpduvLv(Ca310Interface *ca310, uint8_t PN, float* Tcp, float* duv, float* Lv)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			Tcp[len] = 0;
			duv[len] = 0;
			Lv[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*Tcp=(float)atof(buf);  
	buf=strtok(NULL,";");
	*duv=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		Tcp++;
		duv++;
		Lv++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*Tcp=(float)atof(buf)/10000;  //X��Y�Ŵ���һ�򱶴��ݻ�����
		buf=strtok(NULL,";");
		*duv=(float)atof(buf)/10000;
		buf=strtok(NULL,";");
		*Lv=(float)atof(buf);
	}
	return 1;	
}

/**
  * �������� : ��ѯu, v, Lv
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. u  -- uֵ
  *           2. v  -- vֵ
  *           3. Lv -- ����ֵ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetuvLv(Ca310Interface *ca310, uint8_t PN, float* u, float* v, float* Lv)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			u[len] = 0;
			v[len] = 0;
			Lv[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,5\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*u=(float)atof(buf)/10000;  //X��Y�Ŵ���һ�򱶴��ݻ�����
	buf=strtok(NULL,";");
	*v=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		u++;
		v++;
		Lv++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*u=(float)atof(buf)/10000;  //X��Y�Ŵ���һ�򱶴��ݻ�����
		buf=strtok(NULL,";");
		*v=(float)atof(buf)/10000;
		buf=strtok(NULL,";");
		*Lv=(float)atof(buf);
	}
	return 1;	
}

/**
  * �������� : ��ѯX, Y, Z 
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2.PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. X -- ���̼�ֵ֮X
  *           2. Y -- ���̼�ֵ֮Y 
  *           3. Z -- ���̼�ֵ֮Z
  * �� �� ֵ := 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			X[len] = 0;
			Y[len] = 0;
			Z[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,7\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf);  //X��Y�Ŵ���һ�򱶴��ݻ�����
	buf=strtok(NULL,";");
	*Y=(float)atof(buf);
	buf=strtok(NULL,";");
	*Z=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		X++;
		Y++;
		Z++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*X=(float)atof(buf);  //X��Y�Ŵ���һ�򱶴��ݻ�����
		buf=strtok(NULL,";");
		*Y=(float)atof(buf);
		buf=strtok(NULL,";");
		*Z=(float)atof(buf);
	}
	return 1;
}

/**
  * �������� : ��ѯ��˸ֵ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ := 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA)
{
	int len=0;
	char sbuf[10];
	int i;
	
	if (ca310->driver == 0)
		return -1;
	
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			FMA[len] = 0;
			len++;
		}
	}
	
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,6\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	len--;
	*FMA=(float)atof(ca310->recive+7);
	
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		FMA++;
		len--;
		*FMA=(float)atof(ca310->recive+7);
	}
	return 1;	
}

/**
  * �������� : ��ѯCA310̽ͷΨһ���к�
  * ������� : 1.ca310 -- CA310̽ͷ��Ϣ�ṹ
  *           2.PN -- Ĭ��Ϊ1,CA310��̽ͷ���ʱ,ѡ��bit[x](x = 0..5) = 1ָ��̽ͷ���
  * ������� : 1.id -- ̽ͷΨһID
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetSerialNO(Ca310Interface *ca310, uint8_t PN,int* id)
{
	if (ca310->driver == 0)
		return -1;
	int len = 0;
	char sbuf[10];
	int i;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			id[len] = 0;
			len++;
		}
	}
		
	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	sprintf(sbuf,"RPR,");
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			sprintf(sbuf+strlen(sbuf),"%d",i+1);
		}
	}
	sprintf(sbuf+strlen(sbuf),"\r");
	COMRunCommond(ca310, sbuf,ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	
	*id = atof(ca310->recive+5);
	return 1;
}

/**
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������
  * ������� : 1. CH -- CA310̽ͷͨ���Ż�����
  *              ��ѯͨ��ֵ��Ϊ0 ~ 99,��Ӧͨ��CH00 ~ CH99
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA310_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH)
{
	int i;
	int len = 0;
	char str_ch[10];

	memset(str_ch, 0, 10);
    if (ca310->driver == 0)
	{
        return -1;
	}
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			CH[len] = 0;
			len++;
		}
	}
	
	if (PN != ca310->PNumberFlag)
	{
	}
	
	/* ��ѯ��ǰ����ͨ���� */
	COMRunCommond(ca310, "STR,3\r", ca310->recive, 3000);
	if (ca310->recive != strstr(ca310->recive, "OK")) 
    { 
	    return -1;
	}
	
	/* ��ȡ�ظ���Ϣ��ͨ���� */
	*CH = atoi(ca310->recive + 5);

    return 1;
}

/**
  * �������� : ���ò���ͨ����
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������
  *           3. CH -- CA310̽ͷͨ����,ֵ��Ϊ0 ~ 99,����ͨ��CH00 ~ CH99
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA310_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca310->driver == 0) 
	{
		return -1;
	}
	
	/* ����ͨ���� */
	if (CH >= 100 || CH < 0)
	{
		return -1;
	}
	
	/* ����ͨ������ */
	sprintf(str_buff, "MCH,%d\r", CH);
	COMRunCommond(ca310, str_buff, ca310->recive, 3000);
    if (ca310->recive != strstr(ca310->recive, "OK"))  
	{
	    return -1;
	}
	
	return 1;	
}

/**
  * �������� : ���ò�������ģʽ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������
  *           3. speed -- 0:SLOW  1:FAST 2:AUTO 
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA310_SetMeasureSpeed(Ca310Interface *ca310, uint8_t PN, int speed)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca310->driver == 0) 
	{
		return -1;
	}
	
	/* ����������� */
	if (speed >= 3 || speed < 0)
	{
		return -2;
	}
	
	/* ������������ */
	sprintf(str_buff, "FSC,%d\r", speed);
	COMRunCommond(ca310, str_buff, ca310->recive, 3000);
    if (ca310->recive != strstr(ca310->recive, "OK"))  
	{
	    return -3;
	}
	
	return 1;	
}

/**
  * �������� : ����ͬ��Ƶ��
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN  -- ��������
  *           3. fre -- ͬ��Ƶ��,ֵ��Χ40.0 ~ 200.0,����ΪС�����һλ
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA310_SetINTFrequency(Ca310Interface *ca310, uint8_t PN, float fre)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca310->driver == 0) 
	{
		return -1;
	}
	
	/* ����������� */
	if (fre > 200.0 || fre < 40.0)
	{
		return -1;
	}
	
	/* ����ͬ��Ƶ�� */
	sprintf(str_buff, "SCS,%.1f\r", fre);
	COMRunCommond(ca310, str_buff, ca310->recive, 3000);
    if (ca310->recive != strstr(ca310->recive, "OK"))  
	{
	    return -2;
	}
	
	return 1;	
}
/** @}
  * @end of CA-310
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
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA410_Init(Ca310Interface *ca410, int ch,int sync)
{
	char sbuf[10];
	int i = 0;
	
	if (ca410->driver == 0)
		return -1;
	
	while(1)
	{
		COMRunCommond(ca410, "COM,1\r",ca410->recive,3000); 
		if(ca410->recive!=strstr(ca410->recive,"OK")) { 
			i++;
			if (i > 3)
				return -1;
		}
		else {
			break;
		}
	}
	
	/* ����ģʽѡ�� */
	COMRunCommond(ca410, "FSC,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}//ͨ��ģʽ �Զ�
	
	/* �ڴ����ͨ��ѡ�� */
	if (ch >= 0)
	{
		sprintf(sbuf,"MCH,%d\r",ch);
		COMRunCommond(ca410, sbuf,ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;} 
	}
	
	if (sync >= 0)
	{
		sprintf(sbuf,"SCS,%d\r",sync);
		COMRunCommond(ca410, sbuf,ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;} 
	}
	
	return 1;
}

/**
  * �������� : У��CA410̽ͷ
  * ������� : 1. ca410 -- CA410̽ͷ��Ϣ�ṹ
  * �� �� ֵ : = 1 -- У��ɹ�,  < 0 -- У��ʧ��
  */
int COMCA410_Cal0(Ca310Interface *ca410)
{
	int comm_cnt = 0;
	
	if (ca410->driver == 0)
		return -1;
	
	/* ��ʼԶ��ͨ�� */
	while(1)
	{
		COMRunCommond(ca410, "COM,1\r",ca410->recive,3000); 
		if(ca410->recive!=strstr(ca410->recive,"OK")) { 
			comm_cnt++;
			if (comm_cnt > 3)
				return -1;
		}
		else {
			break;
		}
	}

	/* У�� */
    COMRunCommond(ca410, "ZRC\r",ca410->recive,20000);  
    if(ca410->recive!=strstr(ca410->recive,"OK")) 
    { 
    	return -2;
    }
	
	return 1;
}

/**
  * �������� : ��ѯCA410ΨһID
  * ������� : 1. ca410 -- CA410̽ͷ��Ϣ�ṹ
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 1. id -- ����ֵ,��λcd/m^2
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMCA410_GetSerialNO(Ca310Interface *ca410, uint8_t PN, int* id)
{
	char* buf;
	uint8_t cnt = 0;
	
	if (ca410->driver == 0)
		return -1;
	
	/* Զ��ͨ�� */
	COMRunCommond(ca410, "COM,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1; }
	/* ��ȡ̽ͷ��Ϣ */
    COMRunCommond(ca410, "IDO,1,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2; }
	
	buf = strtok(ca410->recive, ",");
	while(cnt < 4)
	{
        cnt++;
        buf = strtok(NULL, ",");
	}
	buf = strtok(NULL, ",");
	*id = (int)atoi(buf);  
	return 1;
}

/**
  * �������� : ��ѯx, y, Lv
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. Lv -- ����ֵ,��λcd/m^2
  *            2. X -- XYɫ��ͼ��X����  
  *            3. Y -- XYɫ��ͼ��Y����
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MDS,0\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MES,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	
	buf=strtok(ca410->recive+9,",");
	*X=(float)atof(buf);  
	buf=strtok(NULL,",");
	*Y=(float)atof(buf);
	buf=strtok(NULL,",");
	*Lv=(float)atof(buf);
		
	return 1;	
}

/**
  * �������� : ��ѯTcp, duv, Lv
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. Tcp -- ɫ��
  *            2. duV -- ɫƫ��ֵ
  *            3. Lv -- ����ֵ,��λcd/m^2
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetTcpduvLv(Ca310Interface *ca410, uint8_t PN,float* Tcp,float* duv,float* Lv)
{
	char* buf;
	uint8_t cnt = 0;

	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MDS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	COMRunCommond(ca410, "MES,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -3;}
	
	buf = strtok(ca410->recive, ",");
	//���и�����
	while(cnt < 2)
	{
        cnt++;
        buf = strtok(NULL, ",");
	}
	buf = strtok(NULL, ",");
	*Tcp = (float)atof(buf);  
	buf = strtok(NULL, ",");
	*duv = (float)atof(buf);
	buf = strtok(NULL, ",");
	*Lv = (float)atof(buf);
		
	return 1;	
}

/**
  * �������� : ��ѯu, v, Lv
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *           2. PN -- ��������,ֵ��Ч
  * ������� : 1. u -- ɫ��uֵ
  *           2. v -- ɫ��vֵ
  *           3. Lv -- ����ֵ,��λcd/m^2
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetuvLv(Ca310Interface *ca410, uint8_t PN, float* u, float* v, float* Lv)
{
	char* buf;
	uint8_t cnt = 0;

	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MDS,5\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	COMRunCommond(ca410, "MES,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -3;}
	
	buf = strtok(ca410->recive, ",");
	//���и�����
	while(cnt < 2)
	{
        cnt++;
        buf = strtok(NULL, ",");
	}
	buf = strtok(NULL, ",");
	*u = (float)atof(buf);  
	buf = strtok(NULL, ",");
	*v = (float)atof(buf);
	buf = strtok(NULL, ",");
	*Lv = (float)atof(buf);
		
	return 1;	
}

/**
  * �������� : ��ѯX, Y, Z
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. X -- ���̼�ֵ֮X
  *            2. Y -- ���̼�ֵ֮Y 
  *            3. Z -- ���̼�ֵ֮Z
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,1\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MDS,7\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "MES,2\r",ca410->recive,3000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	
	buf=strtok(ca410->recive+9,",");
	*X=(float)atof(buf);  
	buf=strtok(NULL,",");
	*Y=(float)atof(buf);
	buf=strtok(NULL,",");
	*Z=(float)atof(buf);
		
	return 1;	
}

/**
  * �������� : ��ѯflick �ٷ�ֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;
	
	COMRunCommond(ca410, "MMS,2\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "FCS,0,0\r",ca410->recive,1000); if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	COMRunCommond(ca410, "MES,1\r",ca410->recive,3000);  if(ca410->recive!=strstr(ca410->recive,"OK")) { return -3;}
	
	
	buf=strtok(ca410->recive,",");
	for (uint8_t i = 0; i< 7; i++) {
		
		buf=strtok(NULL,",");
	}	
	*FMA=(float)atof(buf);

	return 1;	
}

/**
  * �������� : ��ѯflick dBֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetdB(Ca310Interface *ca410, uint8_t PN, float* dB)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;
	COMRunCommond(ca410, "MMS,2\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -1;}
	COMRunCommond(ca410, "FMS,1\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -2;}
	COMRunCommond(ca410, "MDS,6\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -3;}
	COMRunCommond(ca410, "FCS,1,1\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -4;}
	COMRunCommond(ca410, "JCS,1,10\r", ca410->recive, 1000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -5;}
	COMRunCommond(ca410, "MES,1\r", ca410->recive, 5000); 	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -6;}
	COMRunCommond(ca410, "JDR,1,0\r", ca410->recive, 10000);  	if(ca410->recive!=strstr(ca410->recive,"OK")) { return -7;}
	
	
	buf = strtok(ca410->recive,",");
	/* ��ȡ���������� */ 
	for (uint8_t i = 0; i< 3; i++) {
		buf=strtok(NULL,",");
	}	
	*dB = (float)atof(buf);

	return 1;	
}

/**
  * �������� : ����INTģʽ�µ�ͬ��Ƶ��
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  *            3. fre -- ͬ��Ƶ��,���֧��С�������λ
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA410_SetINTFrequency(Ca310Interface *ca410, uint8_t PN, float fre)
{
	char str_buff[20];

	memset(str_buff, 0, 10);
	if (ca410->driver == 0) 
	{
		return -1;
	}
	/* ��ʼԶ��ͨ�� */
	COMRunCommond(ca410, "COM,1\r", ca410->recive, 3000);
	if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
    /* INTģʽ����ͬ��Ƶ�� */
	sprintf(str_buff, "SCS,4,%.2f\r", fre);
	COMRunCommond(ca410, str_buff, ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -2;
	}
	
	return 1;	
}

/**
  * �������� : ��ѯINTģʽ�µ�ͬ��Ƶ��
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. fre -- ͬ��Ƶ��
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetINTFrequency(Ca310Interface *ca410, uint8_t PN, float* fre)
{

	if (ca410->driver == 0) 
	{
		return -1;
	}
	/* ��ʼԶ��ͨ�� */
	COMRunCommond(ca410, "COM,1\r", ca410->recive, 3000);
	if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
    /* ��ѯͬ��Ƶ�� */
	COMRunCommond(ca410, "STR,28\r", ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK��"))  
	{
	    return -2;
	}
	
	/* ��ȡ�ظ���Ϣ��Ƶ�� */
	*fre = atof(ca410->recive + 5);

	return 1;	
}

/**
  * �������� : ��ѯINTģʽ�µ�ͬ��Ƶ��
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. fre -- ͬ��Ƶ��
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  * ע������ : �˺�������C33ʹ��
  */
int COMCA410_OnlyC33_GetINTFrequency(Ca310Interface *ca410, uint8_t PN, float* fre)
{
	char* str_buff;

	if (ca410->driver == 0) 
	{
		return -1;
	}
	/* ��ʼԶ��ͨ�� */
	COMRunCommond(ca410, "COM,1\r", ca410->recive, 3000);
	if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
    /* ��ѯͬ��Ƶ�� */
	COMRunCommond(ca410, "STR,1\r", ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -2;
	}
	
	/* ��ȡ�ظ���Ϣ��Ƶ�� */
	str_buff = strtok(ca410->recive + 5, ",");
	str_buff = strtok(NULL, ",");
	str_buff = strtok(NULL, ",");
	*fre = (float)atof(str_buff);

	return 1;	
}

/**
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. CH -- CA410̽ͷͨ���Ż�����
  *               ��ѯͨ��ֵ��Ϊ0 ~ 99,��Ӧͨ��CH00 ~ CH99
  * �� �� ֵ : = 1 -- ��ѯ�ɹ�,  < 0 -- ��ѯʧ��
  */
int COMCA410_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH)
{
	char str_ch[10];

	memset(str_ch, 0, 10);
    if (ca410->driver == 0)
	{
        return -1;
	}
	
	if (PN != ca410->PNumberFlag)
	{
	}
	
	/* ��ѯ��ǰ����ͨ���� */
	COMRunCommond(ca410, "STR,3\r", ca410->recive, 3000);
	if (ca410->recive != strstr(ca410->recive, "OK")) 
    { 
	    return -1;
	}
	
	/* ��ȡ�ظ���Ϣ��ͨ���� */
	*CH = atoi(ca410->recive + 5);

    return 1;
}

/**
  * �������� : ���ò���ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  *            3. CH -- CA410̽ͷͨ����,ֵ��Ϊ0 ~ 99,����ͨ��CH00 ~ CH99
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA410_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca410->driver == 0) 
	{
		return -1;
	}
	
	/* ����ͨ���� */
	if (CH >= 100 || CH < 0)
	{
		return -1;
	}
	
	/* ����ͨ������ */
	sprintf(str_buff, "MCH,%d\r", CH);
	COMRunCommond(ca410, str_buff, ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
	
	return 1;	
}

/**
  * �������� : ���ò����ٶ�
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  *            3. speedmode -- CA410�����ٶ�ģʽ
  *               0.SLOW 1.FAST 2.LTD.AUTO 3.AUTO
  * �� �� ֵ : = 1 -- ���óɹ�,  < 0 -- ����ʧ��
  */
int COMCA410_SetMeasureSpeed(Ca310Interface *ca410, uint8_t PN, int speedmode)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca410->driver == 0) 
	{
		return -1;
	}
	
	/* �����ٶ�ģʽ���� */
	if (speedmode > 3 || speedmode < 0)
	{
		return -1;
	}
	
	/* �����ٶ����� */
	sprintf(str_buff, "FSC,%d\r", speedmode);
	COMRunCommond(ca410, str_buff, ca410->recive, 3000);
    if (ca410->recive != strstr(ca410->recive, "OK"))  
	{
	    return -1;
	}
	
	return 1;	
}

/** @}
  * @end of CA-410
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
  *            2. ch -- δʹ��
  *            3. sync -- δʹ��
  * �� �� ֵ : ����״̬
  *            1. = 1 -- �ɹ�  2. < 0 -- ʧ��
  */
int COMMSE_Init(Ca310Interface *ca310, int ch,int sync)
{
	int i = 0;
//    uint32_t id;
	
	if (ca310->driver == 0)
		return -1;
	while(1)
	{
		COMRunCommond(ca310, "COM\r",ca310->recive,300); //�����������ݣ��з���˵����·ͨ
		if(ca310->recive!=strstr(ca310->recive,">>")) 
		{ 
			i++;
			if (i > 3)
				return -1;
		}else
		{
			break;
		}
	}
	return 1;
}

/**
  * �������� : COMMSE_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y)
  * �������� : ��ѯɫ���꼰����ֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. Lv -- ����ֵ,��λcd/m^2
  *            2. X -- XYɫ��ͼ��X����  
  *            3. Y -- XYɫ��ͼ��Y����
  * �� �� ֵ : ��ѯ״̬
  *            1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMMSE_GetLvXY(Ca310Interface *ca410, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;

	
	if (ca410->driver == 0)
		return -1;
	
	if(COMRunCommond(ca410, ":MEASure:Yxy\r",ca410->recive,3000)<=0){ return -1;}
	
	buf=strtok(ca410->recive,",");
	*Lv=(float)atof(buf);  
	buf=strtok(NULL,",");
	*X=(float)atof(buf);
	buf=strtok(NULL,",");
	*Y=(float)atof(buf);
	
	return 1;	
}

/**
  * �������� : COMMSE_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z)
  * �������� : ��ѯ���̼�ֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. X -- ���̼�ֵ֮X
  *            2. Y -- ���̼�ֵ֮Y 
  *            3. Z -- ���̼�ֵ֮Z
  * �� �� ֵ : ��ѯ״̬
  *            1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMMSE_GetXYZ(Ca310Interface *ca410, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;
	
	if (ca410->driver == 0)
		return -1;

	if(COMRunCommond(ca410, ":MEASure:XYZ\r",ca410->recive,3000)<=0){ return -1;}
	
	buf=strtok(ca410->recive,",");
	*X=(float)atof(buf);  
	buf=strtok(NULL,",");
	*Y=(float)atof(buf);
	buf=strtok(NULL,",");
	*Z=(float)atof(buf);
	return 1;	
}

/**
  * �������� : COMMSE_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA)
  * �������� : ��ѯ��˸ֵ
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ : ��ѯ״̬
  *            1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMMSE_GetFMA(Ca310Interface *ca410, uint8_t PN,float* FMA)
{
	if (ca410->driver == 0)
		return -1;
	
	
	if(COMRunCommond(ca410, ":MEASure:FLICker 512 \r",ca410->recive,3000)>0)
	
//	buf=strtok(ca410->recive,",");
//	for (uint8_t i = 0; i< 7; i++) {
//		
//		buf=strtok(NULL,",");
//	}	
	*FMA=(float)atof(ca410->recive);
	

	return 1;	
}

/**
  * �������� : COMMSE_GetSN(Ca310Interface *ca410, uint8_t PN,char* SN)
  * �������� : ��ѯ���к�
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 3. SN -- MSE̽ͷΨһ���к�
  * �� �� ֵ : ��ѯ״̬
  *            1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  */
int COMMSE_GetSN(Ca310Interface *ca410, uint8_t PN,char* SN)
{
	
	memset(SN, 0, 7);
	
	if (ca410->driver == 0)
		return -1;
	
	if(COMRunCommond(ca410, ":EEPROM:READ:SN \r",ca410->recive,3000)>0)	
	memcpy(SN, ca410->recive, 7);

	return 1;	
}

/**
  * �������� : COMMSE_GetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 3. CH -- MSE̽ͷͨ�����ַ���
  *                     a."factory"   b."user1" ~ "user30"  c."off" 
  * �� �� ֵ : ��ѯ״̬
  *             1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  * ע������ : �˺�����COMMSE_GetMeasureCH��ͻ,����ͬʱʹ��,����ʹ��
  *            COMMSE_GetMeasureCH()
  */
int COMMSE_GetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
{
	memset(CH, 0, 8);
	
	if (ca410->driver == 0)
		return -1;
	
	if(COMRunCommond(ca410, ":SENSe:SBW? \r",ca410->recive,3000)>0)	
	memcpy(CH, ca410->recive, strlen(ca410->recive)+1);
	return 1;	
}

/**
  * �������� : COMMSE_SetCH(Ca310Interface *ca410, uint8_t PN,char* CH)
  * �������� : ���ò���ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  *            3. CH -- MSE̽ͷͨ�����ַ���
  *                     a."factory\r"   b."user1\r" ~ "user30\r"  c."off\r" 
  * �� �� ֵ : ����״̬
  *            1. = 1 -- ���óɹ�  2. < 0 -- ����ʧ��
  * ע������ : �˺�����COMMSE_SetMeasureCH��ͻ,����ͬʱʹ��,����ʹ��
  *            COMMSE_SetMeasureCH()
  */
int COMMSE_SetCH(Ca310Interface *ca410, uint8_t PN, char* CH)
{
	char str1[50]=":SENSe:SBW ";
	
	memset(CH, 0, 8);
	
	if (ca410->driver == 0)
		return -1;
	
	
	strcat(str1,CH);
	strcat(str1,"\r");
	
	if(COMRunCommond(ca410, str1,ca410->recive,3000)>0)	
	memcpy(CH, ca410->recive, strlen(ca410->recive)+1);
	return 1;	
}

/**
  * �������� : COMMSE_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH)
  * �������� : ���ò���ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  *            3. CH -- MSE̽ͷͨ����
  *                     a.0 -- factoryͨ��ģʽ    b.1 ~ 30 -- userͨ��ģʽ
  *                     c.31 -- offͨ��ģʽ
  * �� �� ֵ : ����״̬
  *            1. = 1 -- ���óɹ�  2. < 0 -- ����ʧ��
  * ע������ : �˺�����COMMSE_SetCH()��ͻ,����ͬʱʹ��
  */
int COMMSE_SetMeasureCH(Ca310Interface *ca410, uint8_t PN, int CH)
{
	char buff[10];
	char fixed_str[20] = ":SENSe:SBW ";
	char *str_match[10] = {"factory\r", "off\r", "user"};
	char match_number = 0;    /* Ĭ��Ϊ"factory"ģʽ*/

	/* ������״̬ */
	if (ca410->driver == 0) 
	{
		return -1;
	}
	
	/* ����ͨ���� */
	if (CH > 31)
	{
		return -1;
	}
	
	/* ƥ��ͨ��,�������� */
	if (CH == 0)
	{
		match_number = 0;
        strcat(fixed_str, str_match[match_number]);
	}	
    else if ((CH > 0) && (CH < 31))
	{
		match_number = 2;
        strcat(fixed_str, str_match[match_number]);
		sprintf(buff, "%d\r", CH);
        strcat(fixed_str, buff);
	}
	else if (CH == 31)
	{
		match_number = 1;
        strcat(fixed_str, str_match[match_number]);
	}
	
	/* ͨ������ */
    if (COMRunCommond(ca410, fixed_str, ca410->recive, 3000) < 0)  
	{
	    return -1;
	}
	
	return 1;	
}

/**
  * �������� : COMMSE_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH)
  * �������� : ��ѯ����ͨ����
  * ������� : 1. ca410 -- Ca310Interface���ͽṹ��
  *            2. PN -- ��������,ֵ��Ч
  * ������� : 1. CH -- MSE̽ͷͨ����
  *                    a.0 -- factoryͨ��ģʽ    b.1 ~ 30 -- userͨ��ģʽ
  *                    c.31 -- offͨ��ģʽ
  * �� �� ֵ : ��ѯ״̬
  *            1. = 1 -- ��ѯ�ɹ�  2. < 0 -- ��ѯʧ��
  * ע������ : �˺�����COMMSE_GetCH()��ͻ,����ͬʱʹ��
  */
int COMMSE_GetMeasureCH(Ca310Interface *ca410, uint8_t PN, int* CH)
{
	char str_ch[10];
	char *str_match[10] = { "factory", "off", "user"};
	int ch_num = 0;    	/* ͨ����0: factory */

	/* ��ջ�����  */
	memset(str_ch, 0, 10);
	if (ca410->driver == 0)
	{
		return -1;
	}
	
	if (PN != ca410->PNumberFlag)
	{
	}
	
	/* ��ѯ��ǰͨ�� */
	if (COMRunCommond(ca410, ":SENSe:SBW? \r",ca410->recive,3000) > 0)	
	{
	    memcpy(str_ch, ca410->recive, strlen(ca410->recive) + 1);
	}
    /* ƥ��ͨ�� */
	if (str_ch == strstr(str_ch, str_match[0]))
	{
		/* factory */
	    *CH = 0; 
	}
	else if (str_ch == strstr(str_ch, str_match[1]))
	{
		/* off */
	    *CH = 31; 
	}
	else if (str_ch == strstr(str_ch, str_match[2]))
	{
		/* ��ȡͨ���� user1 ~ user30 */
		memset(str_ch, '0', 4);
		ch_num = atoi(str_ch);
		*CH = ch_num;
	}
	else 
	{
		*CH = -1;
		return -1;
	}
	 
	return 1;	
}


/** @}
  * @}end of MSE
  */


/**=================================================================================
 *                      ###  C32ͨ�����ù��ܺ���  ###
 * =================================================================================
 * @{
 */

/**
  * �������� : COMC32_Init(Ca310Interface *ca310, int ch,int sync)
  * �������� : ��ʼ��CA-310̽ͷ
  * ������� : 1. ca310 -- CA310̽ͷ��Ϣ�ṹ
  *            2. ch -- �ڴ�ͨ��,ֵ��0 ~ 31
  *            3. sync -- ����
  * �� �� ֵ : ����״̬
  *            1. = 1 -- �ɹ�  2. = -1 -- ʧ��
  */
int COMC32_Init(Ca310Interface *ca310, int ch,int sync)
{
	char sbuf[10];
	int i = 0;

	if (ca310->driver == 0)
		return -1;
	while(1)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,3000); 
		if(ca310->recive!=strstr(ca310->recive,"OK")) 
		{ 
			i++;
			if (i > 3)
				return -1;
		}
		else
		{
			break;
		}
	}
	
	/* �ڴ����ͨ��ѡ�� */
	if (ch >= 0 && ch <= 31)
	{
		/* C32�涨,ͨ���ű���������λ�ַ� */
		if(ch < 10)
			sprintf(sbuf,"MCH,0%d\r",ch);
		else
			sprintf(sbuf,"MCH,%d\r",ch);
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1; } 
	}
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}

	return 1;
}

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
int COMC32_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;
	
	if (ca310->driver == 0)
		return -1;

	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MDS,0\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}

	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf)/10000;  //X��Y�Ŵ���һ�򱶴��ݻ�����
	buf=strtok(NULL,";");
	*Y=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	return 1;	
}

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
int COMC32_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;

	if (ca310->driver == 0)
		return -1;

	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MDS,7\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf);  //X��Y�Ŵ���һ�򱶴��ݻ�����
	buf=strtok(NULL,";");
	*Y=(float)atof(buf);
	buf=strtok(NULL,";");
	*Z=(float)atof(buf);
		
	return 1;
}

/**
  * �������� : COMC32_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA)
  * �������� : ��ѯ��˸ֵ
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ����
  * ������� : 1. FMA -- flickֵ
  * �� �� ֵ : ��ѯ״̬
  *           1. = 1 -- ��ѯ�ɹ�  2. = -1 -- ��ѯʧ��
  */
int COMC32_GetFMA(Ca310Interface *ca310, uint8_t PN, float* FMA)
{
	
	if (ca310->driver == 0)
		return -1;

	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1; }
	COMRunCommond(ca310, "MDS,6\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1; }
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);   if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2; }
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	*FMA=(float)atof(ca310->recive+7);

	return 1;	
}


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
int COMC32_GetMeasureCH(Ca310Interface *ca310, uint8_t PN, int* CH)
{

    if (ca310->driver == 0)
	{
        return -1;
	}
	
	
	/* ��ѯ��ǰ����ͨ���� */
	COMRunCommond(ca310, "STR,3\r", ca310->recive, 3000);
	if (ca310->recive != strstr(ca310->recive, "OK")) 
    { 
	    return -1;
	}
	
	/* ��ȡ�ظ���Ϣ��ͨ���� */
	*CH = atoi(ca310->recive + 5);
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}


    return 1;
}

/**
  * �������� : COMC32_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH)
  * �������� : ���ò���ͨ����
  * ������� : 1. ca310 -- Ca310Interface���ͽṹ��
  *           2. PN -- ����
  *           3. CH -- CA310̽ͷͨ����,ֵ��Ϊ0~31
  * �� �� ֵ : ����״̬
  *           1. = 1 -- ���óɹ�  2. = -1 -- ����ʧ��
  */
int COMC32_SetMeasureCH(Ca310Interface *ca310, uint8_t PN, int CH)
{
	char str_buff[10];

	memset(str_buff, 0, 10);
	if (ca310->driver == 0) 
	{
		return -1;
	}
	
	/* ����ͨ���� */
	if (CH > 31 || CH < 0)
	{
		return -1;
	}

	/* C32�涨,ͨ���ű���������λ�ַ� */
	if(CH < 10)
		sprintf(str_buff, "MCH,0%d\r", CH);
	else
		sprintf(str_buff, "MCH,%d\r", CH);
	COMRunCommond(ca310, str_buff, ca310->recive, 3000); if (ca310->recive != strstr(ca310->recive, "OK"))  { return -1; }
	COMRunCommond(ca310, "COM,0\r",ca310->recive,3000);  if (ca310->recive!=strstr(ca310->recive,"OK"))     { return -2;}

	
	return 1;	
}

/** @}
  * @}end of C32
  */
