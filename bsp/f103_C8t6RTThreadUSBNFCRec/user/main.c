#include <string.h>
#include "systick.h"
#include "delay.h"
#include <stdio.h>
#include "bsp_usart1.h"	  
#include "bsp_usart5.h"	  
#include "Public_SoftStringCmd.h"
#include "communication.h"
#include <rtthread.h>
#include "mypwm.h"
#include "stmflash.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"	
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern int nfcTest(sky_comDriver *dev);
sky_comDriver *USBComDri;
sky_comDriver *uart1Dri;
//int fputc(int ch, FILE *f)
//{
//	uint8_t c = ch;
//    devWrite(USBComDri, &c, 1);
//   
//    return ch;
//}


void rt_hw_console_output(const char *str)
{
    devWrite(USBComDri, (uint8_t *)str, strlen(str));
}

void $Sub$$SystemInit (void)
{
	extern void $Super$$SystemInit (void);
	$Super$$SystemInit();
//	SCB->VTOR = 0x08008000;
}

stringCMD_t scmd;
u8 fifoBuf[128];
u8 cmdBuf[128];

MyFIFO_t nfcfifo;
u8 nfcfifoBuf[512];


char buf[32];

const char cg_ver_message[] __attribute__((at(0X8008F00)))="hth+1.1.0+" __DATE__  "+" __TIME__ "+";


extern int Serial232Read(sky_comDriver *dev, uint8_t *data, int len,uint32_t *pNumberOfBytesRead, int outTime);

extern int TAMABaudrateChange      (  uint8_t* can_communicate);


unsigned char ssData[] = {0x00, 0xff, 0x03, 0xfd, 0xd4, 0x10, 0x04, 0x18, 0x00};
unsigned char reccData[128];
	
int runStrCmd(stringCMD_t *scmd)
{
	int re, len,t;
	uint8_t res,i=0;
	uint32_t outLen;
	
	uint8_t can_communicate;
	
	if (P_StringCmdReadCMD(scmd) > 0)
	{
		re = 0;
		if (P_StringCmdScan(scmd, "connection") >= 0)
		{
					
			nfcTest(uart1Dri);
			len = sprintf(buf, "ok\n");
			devWrite(scmd->dev, (u8 *)buf, len);
		}else if (P_StringCmdScan(scmd, "Rest") >= 0)
		{
			rt_thread_mdelay(20);
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}else if (P_StringCmdScan(scmd, "test") >= 0)
		{
			rt_kprintf("123\r\n");
			devWrite(uart1Dri, ssData, sizeof(ssData));
			rt_thread_mdelay(100);
			Serial232Read(uart1Dri,reccData,sizeof(reccData),&outLen,2000);
			for (int i = 0; i < outLen; i++)
			{
				rt_kprintf("%02x ",reccData[i]);
			}
			rt_kprintf("\r\nrec:%d\r\n",outLen);
			
			rt_thread_mdelay(20);
			//printf("456");
		}else if (P_StringCmdScan(scmd, "ttt1") >= 0)
		{
			TAMABaudrateChange(&can_communicate);
			
			len = sprintf(buf, "\r\ncan_communicate:%d\n",can_communicate);
			devWrite(scmd->dev, (u8 *)buf, len);
		}else if (P_StringCmdScan(scmd, "clearFifo") >= 0)
		{
			Serial232Read(uart1Dri,reccData,sizeof(reccData),&outLen,2);
			for (int i = 0; i < outLen; i++)
			{
				rt_kprintf("%02x ",reccData[i]);
			}
			rt_kprintf("\r\nrec:%d\r\n",outLen);
		}
		else 
		{
			
			 
			rt_kprintf("%02x ",0x30);
			len = sprintf(buf, "NG\n");
			devWrite(scmd->dev, (u8 *)buf, len);
			re = -1;
		}
	}
	return re;
}



//t = rt_tick_get();
//			data[*pNumberOfBytesRead] = res;
//			(*pNumberOfBytesRead)++;
//			if (*pNumberOfBytesRead == len)
//			{
//				return 1;
//			}
//		}
//		
//		if (tmp_GetMeasureTime_ms(t) > outTime)
//		{
//			break;
//		}
//		rt_thread_mdelay(2);
		
		
int main()
{
	int i = 0;
	int t = 0;
	
	rt_thread_mdelay(100);
	uart1Dri = uart1_init(115200);
	
	myFIFOInit(&nfcfifo,nfcfifoBuf, sizeof(nfcfifoBuf));
	uart1Dri->fifo = &nfcfifo;
	
//	while(1)
//	{
//		nfcTest(uart1Dri);
//		rt_thread_mdelay(100);
//	}
	
	USBComDri = USBComInit();
	
	P_StringCmdBindFifo(&scmd, USBComDri, fifoBuf, 128, cmdBuf, 128);
	
	rt_thread_mdelay(100);
	
	
	nfcTest(uart1Dri);
	//nfcTest(uart1Dri);
//	TIM1_PWM_Init(9999, 71);
//	TIM_SetCompare1(TIM1,499);
//	TIM_SetCompare4(TIM1,1499);
	
//	TIM_SetCompare1(TIM8, 499);	 //0.5ms
//	TIM_SetCompare2(TIM8, 2499); //2.5ms
//	TIM_SetCompare3(TIM8, 1499); //1.5ms
//	TIM_SetCompare4(TIM8, 4999); //5ms

	
//	TIM3_Int_Init(719, 0);

	 t = rt_tick_get();
//	
	while (1)
	{
//		EncryRun();

			runStrCmd(&scmd);
		rt_thread_mdelay(100);
//		extern unsigned int tmp_GetMeasureTime_ms( unsigned int ts);
//		if (tmp_GetMeasureTime_ms(t) > i*1000)
//		{
//			
//			rt_kprintf("t:%d\r\n",i);
//			i++;
//			//t = rt_tick_get();
//		}
		
	}
//	while(1)
//	{
//		devWrite(uart1Dri, (u8 *)str, strlen(str));
//		rt_thread_mdelay(100);
//	}
}

/********************************* SKYCODE ************************************/
