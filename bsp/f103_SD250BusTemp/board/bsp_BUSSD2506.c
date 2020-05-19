#include "bsp_BUSSD2506.h"
#include "communication.h"
#include "systick.h"
#include "bsp_sd250x.h"
#include "Public_SoftFunpack.h"
#include "systick.h"

static u8 outFunData[256];
static const funpack_funNametab fun_nametab[]=
{
	(void*)SD250X_RTC_ReadDate,"SD250X_RTC_ReadDate",
	(void*)SD250X_RTC_WriteDate,"SD250X_RTC_WriteDate",	
	0,0
};

static can_node canNodeSD2506;
static sky_MyByteReceiveDataTypeDef SD250Rec;
static char SD2506data[1024];

void BusSD2506_Init(can_adapter *adap)
{
	bus_config bc;
	bc.src_type = BUS_MAIN;
	bc.src_id = 0;
	bc.filt_src  =1;
	bc.dst_type = BUS_SD250;
	bc.dst_id = 0;
	bc.filt_dst  =1;
	P_CAN_SetNodeByBus(&canNodeSD2506, &bc);
	
	
	sky_ConmmunicationInit(&SD250Rec,(uint8_t *)SD2506data, sizeof(SD2506data), 0xbb, 0xee, 2);
	SD250Rec.getTime = MeasureTimeStart_ms;  //获取当前系统时间ms数
	sky_ConmBindDriver(&SD250Rec, &canNodeSD2506.dev);
	
	P_CAN_AddNode(adap, &canNodeSD2506);
}

int busRunProcess2(void)
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
				if (P_unPackASM(fun_nametab,&busRec->data[6], outFunData,&recLen) < 0)
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

