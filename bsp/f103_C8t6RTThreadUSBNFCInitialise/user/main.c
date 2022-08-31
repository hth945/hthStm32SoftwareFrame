#include <string.h>
#include "systick.h"
#include "delay.h"
#include <stdio.h>
#include "bsp_usart1.h"	  
 #include "bsp_usart3.h"	  
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
#include "nfcITarget.h"
#include "nfcInitialise.h"

extern int nfcTest(sky_comDriver *dev);
//sky_comDriver *USBComDri;
sky_comDriver *uart3Dri;
sky_comDriver *uart1Dri;






nfc_Target_t nfc_Target;


void nfcTargetTest(sky_comDriver *dev)
{
	uint8_t                 buffer[262];
    uint8_t                 receivebuffer[262];
	int re;
	int len;
	re = nfcTargetOpen(&nfc_Target,dev);
	if (re < 0)
	{
		rt_kprintf("\nOPenNG\n");
		
	}
	
	rt_kprintf("\nOPen OK\n");
	while(1)
	{
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_0 );
		
		do{
			rt_kprintf("\nConnect ...\n");
			re = nfcTargetConnect(&nfc_Target);
			rt_thread_mdelay(2);
		}while (re < 0);
		
		GPIO_SetBits(GPIOB,GPIO_Pin_0 );
		rt_kprintf("\nConnect OK\n");
		
		while(1)
		{
			//re = nfc_Transceive(&nfc_Target,buffer, 262,receivebuffer,262);
			len = nfc_recve(&nfc_Target,receivebuffer,262);
			if (len < 0)
			{
				rt_kprintf("\nfc_recve NG\n");
				break;
			}
			
			myFIFOWrite(&nfc_Target.recFifo,receivebuffer,len);
			
			len = myFIFORead(&nfc_Target.sendFifo,buffer,262);
			
			re = nfc_send(&nfc_Target,buffer,len);
			if (re < 0)
			{
				rt_kprintf("\nfc_send NG\n");
				break;
			}
			
			rt_kprintf("\nfc_recve send OK\n");
			rt_thread_mdelay(2);
		}
		
		
	}
	
}





nfc_Initialise_t nfc_Initialise;

void nfcInitialiseTest(sky_comDriver *dev)
{
	uint8_t                 buffer[262];
    uint8_t                 receivebuffer[262];
	int re;
	int len;
	re = nfcInitialiseOpen(&nfc_Initialise,dev);
	if (re < 0)
	{
		rt_kprintf("\nOPenNG\n");
	}
	
	rt_kprintf("\nOPen OK\n");
	while(1)
	{
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_0 );
		
		do{
			rt_kprintf("\Connect ...\n");
			re = nfcInitialiseConnect(&nfc_Initialise);
			rt_thread_mdelay(2);
		}while (re < 0);
		
		rt_kprintf("\Connect OK\n");
		GPIO_SetBits(GPIOB,GPIO_Pin_0 );
		while(1)
		{
			
			len = myFIFORead(&nfc_Initialise.sendFifo,buffer,262);
			
			
			len = nfc_Transceive(&nfc_Initialise,buffer, len,receivebuffer,262);
			
			if (len < 0)
			{
				rt_kprintf("\nfc_Transceive NG\n");
				break;
			}
			else
			{
				rt_kprintf("\nfc_Transceive OK\n");
				myFIFOWrite(&nfc_Initialise.recFifo,receivebuffer,len);
			}
			rt_thread_mdelay(2);
		}
	}
	
}


void rt_hw_console_output(const char *str)
{
    //devWrite(uart3Dri, (uint8_t *)str, strlen(str));
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



unsigned char ssData[] = {0x00, 0xff, 0x03, 0xfd, 0xd4, 0x10, 0x04, 0x18, 0x00};
unsigned char reccData[128];
	
int runStrCmd(stringCMD_t *scmd)
{
	int re, len,t;
	uint8_t res,i=0;
	uint32_t outLen;
	if (P_StringCmdReadCMD(scmd) > 0)
	{
		re = 0;
		if (P_StringCmdScan(scmd, "connection") >= 0)
		{
			len = sprintf(buf, "ok\n");
			devWrite(scmd->dev, (u8 *)buf, len);
		}else if (P_StringCmdScan(scmd, "testT") >= 0)
		{
					
			//nfcTargetTest(uart1Dri);
			len = sprintf(buf, "ok\n");
			devWrite(scmd->dev, (u8 *)buf, len);
		}else if (P_StringCmdScan(scmd, "testI") >= 0)
		{
					
			//nfcInitialiseTest(uart1Dri);
			len = sprintf(buf, "ok\n");
			devWrite(scmd->dev, (u8 *)buf, len);
		}else if (P_StringCmdScan(scmd, "Rest") >= 0)
		{
			rt_thread_mdelay(20);
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}else 
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
		
void MultiKeyIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_3);
}



//void uart_thread_entry(void *parameter)
//{
//	uint8_t iOstate=*(uint8_t *)parameter; 
//	if (iOstate == 0)
//	{
//		GPIO_SetBits(GPIOB,GPIO_Pin_0 );
//		GPIO_SetBits(GPIOB,GPIO_Pin_3 );
//		nfcInitialiseTest(uart1Dri);
//	}
//	else
//	{
//		GPIO_ResetBits(GPIOB,GPIO_Pin_0 );
//		GPIO_ResetBits(GPIOB,GPIO_Pin_3);
//		nfcTargetTest(uart1Dri);
//	}
//}
//    



int main()
{
	int i = 0;
	int t = 0;
	uint8_t iOstate=0;
	rt_thread_t tid;
	int len;
	MyFIFO_t *sendFifo;
	MyFIFO_t *recFifo;
	
	rt_thread_mdelay(100);
	uart1Dri = uart1_init(115200);
	uart3Dri = uart3_init(115200);
	
	myFIFOInit(&nfcfifo,nfcfifoBuf, sizeof(nfcfifoBuf));
	uart1Dri->fifo = &nfcfifo;
	
	
//	while(1)
//	{
//		nfcTest(uart1Dri);
//		rt_thread_mdelay(100);
//	}
	
//	USBComDri = USBComInit();
	
	//P_StringCmdBindFifo(&scmd, uart3Dri, fifoBuf, 128, cmdBuf, 128);
	
	rt_thread_mdelay(100);
	
	MultiKeyIO_Init();
	
	
	iOstate += GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
	iOstate += GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
	iOstate += GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
	iOstate += GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
	
	if (iOstate == 0)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0 );
		GPIO_SetBits(GPIOB,GPIO_Pin_3 );
//		nfcInitialiseTest(uart1Dri);
		tid = rt_thread_create("nfc_thread_entry", (void (*)(void *parameter))nfcInitialiseTest, uart1Dri,
                           4*1024, 5, 20);
		
		sendFifo = &nfc_Initialise.sendFifo;
		recFifo = &nfc_Initialise.recFifo;
		
		
	}
	else
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_0 );
		GPIO_ResetBits(GPIOB,GPIO_Pin_3);
//		nfcTargetTest(uart1Dri);
		tid = rt_thread_create("nfc_thread_entry", (void (*)(void *parameter))nfcTargetTest, uart1Dri,
                           4*1024, 5, 20);
		
		sendFifo = &nfc_Target.sendFifo;
		recFifo = &nfc_Target.recFifo;
	}
	
	
	
	rt_thread_startup(tid);					   
	
	
//	nfcTest(uart1Dri);
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


	uart3Dri->fifo = sendFifo;
	while (1)
	{
		
		
//		myFIFOWrite(&nfc_Target.recFifo,receivebuffer,len);
//			
		while ((len = myFIFORead(recFifo,reccData,128) ) > 0)
		{
			devWrite(uart3Dri, (uint8_t *)reccData, len);
		}
		
		rt_thread_mdelay(10);
//			
//			re = nfc_send(&nfc_Target,buffer,len);
//		
		
		
		
		
//		EncryRun();

//			runStrCmd(&scmd);
//		rt_thread_mdelay(100);
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
