#include <string.h>
#include <stdio.h>
#include "sys.h"
#include "driverUsart1.h"
#include "Public_SoftStringCmd.h"
#include "communication.h"
#include "stm32f4xx_rcc.h"
#include "mypwm.h"
#include "myadc.h"

void RCC_Config(void)
{
	__IO uint32_t StartUpCounter = 0, HSEStatus = 0;

	/* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif
	/* Reset the RCC clock configuration to the default reset state ------------*/
	/* Set HSION bit */
	RCC->CR |= (uint32_t)0x00000001;

	/* Reset CFGR register */
	RCC->CFGR = 0x00000000;

	/* Reset HSEON, CSSON and PLLON bits */
	RCC->CR &= (uint32_t)0xFEF6FFFF;

	/* Reset PLLCFGR register */
	RCC->PLLCFGR = 0x24003010;

	/* Reset HSEBYP bit */
	RCC->CR &= (uint32_t)0xFFFBFFFF;

	/* Disable all interrupts */
	RCC->CIR = 0x00000000;

	/* Enable HSE */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	} while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}

	if (HSEStatus == (uint32_t)0x01)
	{
		/* Select regulator voltage output Scale 1 mode */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS;

		/* HCLK = SYSCLK / 1*/
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

		/* PCLK2 = HCLK / 2*/
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

		/* PCLK1 = HCLK / 4*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

		/* Configure the main PLL */
		//    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
		//                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
		RCC->PLLCFGR = 8 | (336 << 6) | (((2 >> 1) - 1) << 16) |
					   (RCC_PLLCFGR_PLLSRC_HSE) | (7 << 24);

		/* Enable the main PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till the main PLL is ready */
		while ((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}

		/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
		FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

		/* Select the main PLL as system clock source */
		RCC->CFGR &= (uint32_t)((uint32_t) ~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		/* Wait till the main PLL is used as system clock source */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
		{
		}
	}
	else
	{ /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
	}

	SCB->VTOR = FLASH_BASE; /* Vector Table Relocation in Internal FLASH */
}

void $Sub$$SystemInit(void)
{
	RCC_Config();
}

MyFIFO_t myfifo;
u8 fifoBuf[256];
stringCMD_t scmd;
u8 cmdBuf[256];
u8 exitFlag[3];

// USART1     GPIOA GPIO_Pin_9 | GPIO_Pin_10
// key        GPIOE  GPIO_Pin_3|GPIO_Pin_4
// LED        GPIOE  GPIO_Pin_8| GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15
// ADC        GPIOA GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5
// 舵机 TIM8  GPIOC GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9
// 电机 TIM9  GPIOE  GPIO_Pin_5|GPIO_Pin_6
// 步进马达   TIM10 B8    TIM11 B9

//按键初始化函数
void KEY_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); //使能GPIOA,GPIOE时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; //KEY2 KEY3对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		   //普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	   //100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		   //上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);				   //初始化GPIOE2,3,4
}

void LED_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); //使能GPIOA时钟

	//GPIOF9,F10初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; //LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																							   //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																							   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;																						   //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;																							   //上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);																									   //初始化GPIO

	GPIO_ResetBits(GPIOE, 0xff00); //设置高，灯灭
}

int main()
{
	char buf[32];
	int ch, value, re, len;
	sky_comDriver *uart1Dri;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //
	

	//	RCC_Config();
	SysTick_Init();

	Adc_Init();
	KEY_Init();
	LED_Init();
	//	EXTIX_Init();

	uart1Dri = USART1_Init();

	P_StringCmdBindFifo(&scmd, uart1Dri, fifoBuf, sizeof(fifoBuf), cmdBuf, sizeof(cmdBuf));

	TIM8_PWM_Init(9999, 167); //100hz.

	TIM_SetCompare1(TIM8, 499);	 //0.5ms
	TIM_SetCompare2(TIM8, 2499); //2.5ms
	TIM_SetCompare3(TIM8, 1499); //1.5ms
	TIM_SetCompare4(TIM8, 4999); //5ms
	
	//TIM_SetCompare1(TIM8, 1499);	 //0.5ms

	//	TIM8_PWM_Init(167,0);	//TIM4 PWM初始化, Fpwm=168M/168=1000Khz.
	//	TIM_SetCompare2(TIM8,83);	//输出
	//	TIM_SetCompare1(TIM8,41);	//输出

	TIM9_PWM_Init(99, 167);	   //10Khz.
	TIM_SetCompare2(TIM9, 49); //输出
	TIM_SetCompare1(TIM9, 89); //输出GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
	
	TIM10_PWM_Init(1000,167);
	TIM11_PWM_Init(1000,167);

	while (1)
	{
		if (P_StringCmdReadCMD(&scmd) > 0)
		{
			if (P_StringCmdScan(&scmd, "connection") >= 0)
			{
				len = sprintf(buf, "ok\n");
				uart1Dri->write(uart1Dri, (u8 *)buf, len);
			}
			else if (P_StringCmdScan(&scmd, "setmotorB%d:%d", &ch, &value) >= 0) //每秒脉冲数
			{
				if ((ch >= 8) && (ch <= 9) && (value >= 0) && (value <= 100000))
				{
					if (ch == 8)
					{
						TIM10_PWM_setarr(value);
					}else
					{
						TIM11_PWM_setarr(value);
					}
					len = sprintf(buf, "ok value< 20 value=0\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
				else
				{
					len = sprintf(buf, "ch is 2~3, value is 0~100000\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
			}
			else if (P_StringCmdScan(&scmd, "setPwmA%d:%d", &ch, &value) >= 0)
			{
				if ((ch >= 2) && (ch <= 3) && (value >= 0) && (value <= 99))
				{
					*((u32 *)(&TIM9->CCR1) + ch - 1) = value;
					len = sprintf(buf, "ok\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
				else
				{
					len = sprintf(buf, "ch is 2~3, value is 0~99\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
			}
			else if (P_StringCmdScan(&scmd, "setServoC%d:%d", &ch, &value) >= 0)
			{
				if ((ch >= 6) && (ch <= 9) && (value >= 0) && (value <= 9999))
				{
					*((u32 *)(&TIM8->CCR1) + ch - 6) = value;
					len = sprintf(buf, "ok\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
				else
				{
					len = sprintf(buf, "ch is 6~9, value is 0~9999\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
			}
			else if (P_StringCmdScan(&scmd, "setPinE%d:%d", &ch, &value) >= 0)
			{
				if ((ch >= 8) && (ch <= 15) && (value >= 0) && (value <= 1))
				{
					if (value == 0)
					{
						GPIO_ResetBits(GPIOE, 1 << (ch));
					}
					else
					{
						GPIO_SetBits(GPIOE, 1 << (ch));
					}
					len = sprintf(buf, "ok\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
				else
				{
					len = sprintf(buf, "ch is 8~15, value is 0~1\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
			}
			else if (P_StringCmdScan(&scmd, "getPinE%d", &ch) >= 0)
			{
				if ((ch >= 3) && (ch <= 4))
				{
					re = GPIO_ReadInputDataBit(GPIOE, 1 << (ch));
					len = sprintf(buf, "%d\n", re);
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
				else
				{
					len = sprintf(buf, "ch is 3~4\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
			}
			else if (P_StringCmdScan(&scmd, "getADCA%d", &ch) >= 0)
			{
				if ((ch >= 1) && (ch <= 5))
				{
					re = Get_Adc((uint8_t)ch);
					//					Delay_ms(1);
					//					re = Get_Adc((uint8_t)ch);
					len = sprintf(buf, "%d\n", re);
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
				else
				{
					len = sprintf(buf, "ch is 1~5\n");
					uart1Dri->write(uart1Dri, (u8 *)buf, len);
				}
			}
			else
			{
				len = sprintf(buf, "error\n");
				uart1Dri->write(uart1Dri, (u8 *)buf, len);
			}
		}
	}
}

/********************************* SKYCODE ************************************/
