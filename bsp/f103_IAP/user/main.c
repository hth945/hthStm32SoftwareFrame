#include <string.h>
#include "systick.h"
#include "delay.h"
#include <stdio.h>
#include "bsp_usart1.h"	  
#include "bsp_usart5.h"	 
#include "iapSoftware.h"
#include "stmflash.h"

iap_adapter iapA;
int main()
{
	sky_comDriver*  uartDev;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	SysTick_Init();
	uartDev = uart1_init(460800);
	
	iap_Init(&iapA, uartDev, 200, 0x08008000, "hth+V1.0.0");
	
	while (1)
	{
		EncryRun();
		iapUpRun(&iapA);
		if ((iapA.iapFlag == 0) && (GetMeasureTime_ms(iapA.startTime) > iapA.timeOut))
		{
			iap_load_app(&iapA);
			iapA.iapFlag = 1;
			
		}
		
		
		
//		char *str = "hello\r\n";
//		devWrite(uartDev, (u8 *)str, strlen(str));
//		Delay_ms(100);
	}
}

/********************************* SKYCODE ************************************/
