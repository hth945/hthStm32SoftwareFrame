#include "iapSoftware.h"
#include "delay.h"
#include "stmflash.h"

void iap_Init(iap_adapter *iapAda,sky_comDriver *driver, int timeOut,u32 appxaddr, char *version)
{
	iapAda->appxaddr = appxaddr; 
	iapAda->timeOut = timeOut; 
	iapAda->startTime = MeasureTimeStart_ms();
	iapAda->iapFlag = 0;
	strcpy(iapAda->version,version);
	sky_ConmmunicationInit(&iapAda->iapRec,(u8 *)iapAda->iapRecData, sizeof(iapAda->iapRecData),0xbb, 0xee,2);
	iapAda->iapRec.getTime = MeasureTimeStart_ms; 
	sky_ConmBindDriver(&iapAda->iapRec, driver);
}

int iapUpRun(iap_adapter *iapAda)
{
	u32 start,offset, len;
	sky_MyByteReceiveDataTypeDef *recB = &iapAda->iapRec;
	DataHead *dataHead = (DataHead *)recB->data;
	u8 *data = &recB->data[6];
	
	//
	
	
	if (recB->saveFlag == 3)
	{
		switch(dataHead->cmd)
		{
		case 0x01://
			iapAda->iapFlag = 1;
			break;
		case 0x40://����IAP
			iapAda->iapFlag = 1;
			break;
		case 0x41://�������
			start = *((u32 *)(data));
			len = *(u32 *)(data + 4);
			STMFLASH_ERASE(start,(len+4));
			break;
		case 0x42://
			offset = *((u32 *)(data));
			len = *(u32 *)(data + 4);
			STMFLASH_Write_NO_ERASE(offset, data + 8, len);
			break;
		case 0x43:
			sky_sendData(recB,dataHead->cmd | 0x10, 0, 0);
			Delay_ms(20);
			__set_FAULTMASK(1);
			NVIC_SystemReset(); 
			break;
				
				
		case 0x44:  //����д����
//			offset = *((u32 *)(data));
//			len = *(u32 *)(data + 4);
//	
//			//deAes(data + 8, len, key);
//			STM32_AES_CTR_Decrypt( data + 8, len, key, iv, sizeof(iv), data + 8, &OutputMessageLength);
//			
//			STMFLASH_Write_NO_ERASE(offset, data + 8, len);
			//UsbUpdate(data + 8, offset, len);
			break;
		case 0x45:  //ΨһID����д����
//			offset = *((u32 *)(data));
//			len = *(u32 *)(data + 4);
//	
//			//deAes(data + 8, len, key);
//			STM32_AES_CTR_Decrypt( data + 8, len, key, iv, sizeof(iv), data + 8, &OutputMessageLength);
//			STMFLASH_Write_NO_ERASE(offset, data + 8, len);
		
			//UsbUpdate(data + 8, offset, len);
			break;
		case 0x46:  //�Ա��ͺ�
//			sprintf(ss,"%s+%d",message,hv[0]);
//			if (strcmp(ss,data) != 0)
//			{
//				sky_sendData(recB,dataHead->cmd, 0, 0);
//				recB->saveFlag = 0;
//				return -1;
//			}else
//			{
//				sky_sendData(recB,dataHead->cmd | 0x10, 0, 0);
//				recB->saveFlag = 0;
//				return dataHead->cmd;
//			}
		break;
		case 0x47:  //�ض��ͺ�
			sky_sendData(recB,dataHead->cmd | 0x10, (uint8_t *)iapAda->version, strlen(iapAda->version));
			break;	
		case 0x61:  //ת������
//			type = *((u32 *)(data));
//			id = *(u32 *)(data + 4);
//			time = *(u32 *)(data + 8);
//			recLen = ForwardingSend(id, type, data+12, recB->dataLen - 12, &recData, time);
//			if (recLen < 0)
//			{
//				sky_sendData(recB,dataHead->cmd, 0, 0);
//				recB->saveFlag = 0;
//				return -1;
//			}else
//			{
//				sky_sendData(recB,dataHead->cmd | 0x10, recData, recLen);
//				recB->saveFlag = 0;
//				return dataHead->cmd;
//			}
			
			break;
		default:
			sky_sendData(recB,dataHead->cmd, 0, 0);
			recB->saveFlag = 0;
			return -1;
		}
		sky_sendData(recB,dataHead->cmd | 0x10, 0, 0);
		recB->saveFlag = 0;
		return dataHead->cmd;
	}
	
	return 0;
}

//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.

//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(iap_adapter *iapAda)
{
	iapfun jump2app; 
	u32 appxaddr = iapAda->appxaddr;
	
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}

