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
		case 0x40://进入IAP
			iapAda->iapFlag = 1;
			break;
		case 0x41://清除命令
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
				
				
		case 0x44:  //密文写数据
//			offset = *((u32 *)(data));
//			len = *(u32 *)(data + 4);
//	
//			//deAes(data + 8, len, key);
//			STM32_AES_CTR_Decrypt( data + 8, len, key, iv, sizeof(iv), data + 8, &OutputMessageLength);
//			
//			STMFLASH_Write_NO_ERASE(offset, data + 8, len);
			//UsbUpdate(data + 8, offset, len);
			break;
		case 0x45:  //唯一ID密文写数据
//			offset = *((u32 *)(data));
//			len = *(u32 *)(data + 4);
//	
//			//deAes(data + 8, len, key);
//			STM32_AES_CTR_Decrypt( data + 8, len, key, iv, sizeof(iv), data + 8, &OutputMessageLength);
//			STMFLASH_Write_NO_ERASE(offset, data + 8, len);
		
			//UsbUpdate(data + 8, offset, len);
			break;
		case 0x46:  //对比型号
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
		case 0x47:  //回读型号
			sky_sendData(recB,dataHead->cmd | 0x10, (uint8_t *)iapAda->version, strlen(iapAda->version));
			break;	
		case 0x61:  //转发命令
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

//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(iap_adapter *iapAda)
{
	iapfun jump2app; 
	u32 appxaddr = iapAda->appxaddr;
	
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}

