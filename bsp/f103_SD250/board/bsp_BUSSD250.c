#include "bsp_BUSSD250.h"
#include "communication.h"
#include "bsp_sd250x.h"
#include "Public_SoftFunpack.h"

u8 outFunData[256];
static const funpack_funNametab fun_nametab[]=
{
	(void*)SD250X_RTC_ReadDate,"SD250X_RTC_ReadDate",
	(void*)SD250X_RTC_WriteDate,"SD250X_RTC_WriteDate",	
	0,0
};

static bus2com_adapter sky_BusSD250Dri;
static sky_MyByteReceiveDataTypeDef SD250Rec;
static char SD250data[1024];


static int BusSD250X_Write(u8 *data,int len)
{
	return P_BUS2ComSendData(&sky_BusSD250Dri, data, len);
}

static int comBindDriver(sky_MyByteReceiveDataTypeDef *myRecType,sky_comDriver *driver)
{
	myRecType->send = driver->write;
	driver->userData = myRecType;
	driver->single = (int (*)(void *userData,uint8_t *data, int len))sky_receivePactData;
	
	return 1;
}

//初始化之前 必须已经调用过BUSInit()
void BusSD250X_Init(void)
{
	P_Bus2Com_DeInit(&sky_BusSD250Dri);
	sky_BusSD250Dri.self_type = BUS_SD250;
	sky_BusSD250Dri.self_id = 0;
	sky_BusSD250Dri.remote_type = BUS_MAIN;
	sky_BusSD250Dri.remote_id = 0;
	sky_BusSD250Dri.driver.write = BusSD250X_Write;
    P_Bus2ComInit(&sky_BusSD250Dri);
	
	sky_ConmmunicationInit(&SD250Rec,(u8 *)SD250data, sizeof(SD250data),0xbb, 0xee,2);
	SD250Rec.getTime = MeasureTimeStart_ms;  //获取当前系统时间ms数
	//SD250Rec.completeCallback = 0;//busCallbackProcess; //接收到完整数据的中断
	comBindDriver(&SD250Rec, &sky_BusSD250Dri.driver);
}

//static void busCallbackProcess(void *data)
//{
//	sky_MyByteReceiveDataTypeDef *busRec = &SD250Rec;
//	
//	if (busRec->saveFlag == 3)
//	{
//		switch(busRec->data[1])
//		{
//			case 0x01:  //请求 不通知
//				//直接返回全局变量中的数据（一直在更新的电流值）
//				//sky_recOffsetType(busRec,(u8 *)&myData,offest,len,CE48CoreRec.data[1] | 0x10,1);
//				
//				break;
//		}
//		
//		busRec->saveFlag = 0;
//	}
//}



int busRunProcess(void)
{
	sky_MyByteReceiveDataTypeDef *busRec = &SD250Rec;
	Time_Def td;
	int recLen;
	
	if (busRec->saveFlag == 3)
	{
		switch(busRec->data[1])
		{
			case 0x01:  //请求 不通知
				//直接返回全局变量中的数据（一直在更新的电流值）
				//sky_recOffsetType(busRec,(u8 *)&myData,offest,len,CE48CoreRec.data[1] | 0x10,1);
				
				break;
			case 0x62:
				
				SD250X_RTC_ReadDate(&td);
			
				sky_sendData(busRec,busRec->data[1]|0x10, (u8 *)&td, sizeof(td));
				busRec->saveFlag = 0;
				return 0;
			
			case 0xC1:
				if (P_unPack(fun_nametab,&busRec->data[6], outFunData,&recLen) < 0)
				{
					sky_sendData(busRec,busRec->data[1], 0, 0);
					busRec->saveFlag = 0;
					return -1;
				}else
				{
					sky_sendData(busRec,busRec->data[1]|0x10, outFunData, recLen);
					busRec->saveFlag = 0;
					return 0;
				}
			default:
				sky_sendData(busRec,busRec->data[1], 0, 0);
				busRec->saveFlag = 0;
				return -1;
		}
		sky_sendData(busRec,busRec->data[1]|0x10, 0, 0);
		busRec->saveFlag = 0;
	}
	return 0;
}





