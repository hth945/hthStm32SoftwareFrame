/********************************* SKYCODE *************************************
* Copyright (c) 2017, 深圳思凯测试技术有限公司（广州天码电子技术有限公司）
* All rights reserved.
*
* 文件名   ：main.c    
* 描述     ：
*
*
* 版    本 ：V0.1
* 作    者 ：
* 完成日期 ：
* 修改描述 ：
*
*******************************************************************************/

/*-----------------Includes--------------------------------------------------*/
#include <string.h>
#include "systick.h"
#include "delay.h"
#include <stdio.h>
#include "bsp_sd250x.h"
#include "bsp_BUSSD250.h"


void SetSysClockTo36()
{
    __IO uint32_t HSIStartUpStatus = 0;

    HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;

    if (HSIStartUpStatus == RCC_CR_HSIRDY)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 2 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        RCC_PCLK2Config(RCC_HCLK_Div1);

        RCC_PCLK1Config(RCC_HCLK_Div1);

        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);

        RCC_PLLCmd(ENABLE);

        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {}

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        while (RCC_GetSYSCLKSource() != 0x08){
        }
    }else{
        while (1){
        }
    }
	
	SystemCoreClock = 36000000;
}


//int $Sub$$main(void)
void $Sub$$SystemInit (void)
{
	SetSysClockTo36();
}


int main()
{
//	Time_Def td;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	
	
	SysTick_Init();
	Bus_Init();
	
	SD250X_Init();
	BusSD250X_Init();
	
	while(1)
	{
		busRunProcess();
		
		Delay_ms(1);
	}
}

/********************************* SKYCODE ************************************/
