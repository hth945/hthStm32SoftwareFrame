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
#include <stdio.h>
#include "sys.h"
#include "driverUsart1.h"
#include "Public_SoftStringCmd.h"
#include "communication.h"
#include "stm32f4xx_rcc.h"





void RCC_Config(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
	
	  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
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
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

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
    RCC->PLLCFGR = 8 | (336 << 6) | (((2 >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (7 << 24);
    
    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
   

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;


    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }

  SCB->VTOR = FLASH_BASE ; /* Vector Table Relocation in Internal FLASH */
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

//初始化ADC															   
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

  //先初始化ADC1通道5 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;//PA5 通道5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
 
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	

}	

uint16_t Get_Adc(uint8_t ch)   
{
	  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}


//按键初始化函数
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; //KEY2 KEY3对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4
	
} 

void LED_Init(void)
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;//LED0和LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_ResetBits(GPIOE,0xff00);//设置高，灯灭

}

void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1| EXTI_Line2 ;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置


	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//外部中断4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	
 	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置 
}
void EXTI0_IRQHandler(void)
{
	exitFlag[0] = 1;
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}	

void EXTI1_IRQHandler(void)
{
	exitFlag[1] = 1;
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}	
void EXTI2_IRQHandler(void)
{
	exitFlag[2] = 1; 
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}	

/*
	可以输出到GPIO的TIM通道:

	TIM1_CH1, PA8,	PE9,
	TIM1_CH2, PA9,	PE11
	TIM1_CH3, PA10,	PE13
	TIM1_CH4, PA11,	PE14

	TIM2_CH1,
	TIM2_CH2, PA1,	PB3
	TIM2_CH3, PA2,	PB10
	TIM2_CH4, PA3,	PB11

	TIM3_CH1, PA6,  PB4, PC6
	TIM3_CH2, PA7,	PB5, PC7
	TIM3_CH3, PB0,	PC8
	TIM3_CH4, PB1,	PC9

	TIM4_CH1, PB6,  PD12
	TIM4_CH2, PB7,	PD13
	TIM4_CH3, PB8,	PD14
	TIM4_CH4, PB9,	PD15

	TIM5_CH1, PA0,  PH10
	TIM5_CH2, PA1,	PH11
	TIM5_CH3, PA2,	PH12
	TIM5_CH4, PA3,	PI10

	TIM8_CH1, PC6,  PI5
	TIM8_CH2, PC7,	PI6
	TIM8_CH3, PC8,	PI7
	TIM8_CH4, PC9,	PI2

	TIM9_CH1, PA2,  PE5
	TIM9_CH2, PA3,	PE6

	TIM10_CH1, PB8,  PF6

	TIM11_CH1, PB9,  PF7

	TIM12_CH1, PB14,  PH6
	TIM12_CH2, PB15,  PH9

	TIM13_CH1, PA6,  PF8
	TIM14_CH1, PA7,  PF9

	APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
	APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
*/
void TIM8_PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	memset((u8 *)&TIM_OCInitStructure,0,sizeof(TIM_OCInitStructure));
	memset((u8 *)&TIM_TimeBaseStructure,0,sizeof(TIM_TimeBaseStructure));
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  	//TIM9时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能PORTA时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PA3
 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM8); 

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseStructure);//初始化定时器9
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse=0;
	
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM9 OC2
	
	
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM9 OC2
	
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM9 OC2
	
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM9 OC2
	
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM9在CCR2上的预装载寄存器
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM9在CCR2上的预装载寄存器
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM9在CCR2上的预装载寄存器
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM9在CCR2上的预装载寄存器
	
	TIM_ARRPreloadConfig(TIM8,ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM8, ENABLE);  //使能TIM9
	
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
}

void TIM9_PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	//TIM9时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能PORTA时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3; //GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA3
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM9); //GPIOA3复用位定时器9 AF2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM9); //GPIOA3复用位定时器9 AF3

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);//初始化定时器9

	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse=0;
	
	TIM_OC2Init(TIM9, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM9 OC2
	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM9在CCR2上的预装载寄存器
	
	//
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM9 OC2
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM9在CCR2上的预装载寄存器
	//
 
  TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM9, ENABLE);  //使能TIM9
}


int main()
{
	int ch,value,re,len;
	sky_comDriver* uart1Dri;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	//
	char buf[32];
	
//	RCC_Config();
	SysTick_Init();
	
	Adc_Init();
	KEY_Init();
	LED_Init();
//	EXTIX_Init();
	
	
	uart1Dri = USART1_Init();

	P_StringCmdBindFifo(&scmd, uart1Dri, fifoBuf, sizeof(fifoBuf), cmdBuf,sizeof(cmdBuf));
	
	TIM8_PWM_Init(9999,167);	//TIM4 PWM初始化, Fpwm=168M/168=1000Khz.
	
	TIM_SetCompare1(TIM8,499);	//0.5ms
	TIM_SetCompare2(TIM8,2499);	//2.5ms
	TIM_SetCompare3(TIM8,1499);	//1.5ms
	TIM_SetCompare4(TIM8,4999);	//5ms
	
	
//	TIM8_PWM_Init(167,0);	//TIM4 PWM初始化, Fpwm=168M/168=1000Khz.
//	TIM_SetCompare2(TIM8,83);	//输出
//	TIM_SetCompare1(TIM8,41);	//输出
	
	TIM9_PWM_Init(99,167);	//TIM4 PWM初始化, Fpwm=168M/168=1000Khz.
	TIM_SetCompare2(TIM9,49);	//输出
	TIM_SetCompare1(TIM9,89);	//输出GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	
	
	while(1)
	{
		if (P_StringCmdReadCMD(&scmd) > 0)
		{
			if (P_StringCmdScan(&scmd,"connection")>=0)
			{
				len = sprintf(buf,"ok\n");
				uart1Dri->write(uart1Dri, (u8 *)buf,len);
			}else if (P_StringCmdScan(&scmd,"setPwmA%d:%d", &ch, &value)>=0)
			{
				if ((ch >= 2) && (ch<= 3) && (value>=0) && (value<=99))
				{
					*((u32 *)(&TIM9->CCR1) + ch - 1) = value;
					len = sprintf(buf,"ok\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
				else
				{
					len = sprintf(buf,"ch is 2~3, value is 0~99\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
			}else if (P_StringCmdScan(&scmd,"setServoC%d:%d", &ch, &value)>=0)
			{
				if ((ch >= 6) && (ch<=9) && (value>=0) && (value<=9999))
				{
					*((u32 *)(&TIM8->CCR1) + ch - 1) = value;
					len = sprintf(buf,"ok\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
				else
				{
					len = sprintf(buf,"ch is 6~9, value is 0~9999\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
			}else if (P_StringCmdScan(&scmd,"setPinE%d:%d", &ch, &value)>=0)
			{
				if ((ch >= 8) && (ch<=15) && (value>=0) && (value<=1))
				{
					if (value == 0)
					{
						GPIO_ResetBits(GPIOE,1 << (ch));
					}else
					{
						GPIO_SetBits(GPIOE,1 << (ch)); 
					}
					len = sprintf(buf,"ok\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
				else
				{
					len = sprintf(buf,"ch is 8~15, value is 0~1\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
			}else if (P_StringCmdScan(&scmd,"getPinE%d", &ch)>=0)
			{
				if ((ch >= 3) && (ch<=7))
				{
					re = GPIO_ReadInputDataBit(GPIOE,1 << (ch));
					len = sprintf(buf,"%d\n",re);
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
				else
				{
					len = sprintf(buf,"ch is 3~7\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
			}else if (P_StringCmdScan(&scmd,"getExitE%d:%d", &ch)>=0)  //读取是否已经发生中断 若中断则清0 
			{
				if ((ch >= 0) && (ch<= 2))
				{
					re = exitFlag[ch];
					exitFlag[ch] = 0;
					len = sprintf(buf,"%d\n",re);
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
				else
				{
					len = sprintf(buf,"ch is 0~2\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
			}else if (P_StringCmdScan(&scmd,"getADCA%d", &ch)>=0)
			{
				if ((ch >= 1) && (ch<=5))
				{
					re = Get_Adc((uint8_t)ch);
//					Delay_ms(1);
//					re = Get_Adc((uint8_t)ch);
					len = sprintf(buf,"%d\n",re);
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
				else
				{
					len = sprintf(buf,"ch is 1~5\n");
					uart1Dri->write(uart1Dri, (u8 *)buf,len);
				}
			}else
			{
				len = sprintf(buf,"error\n");
				uart1Dri->write(uart1Dri, (u8 *)buf,len);
			}
			
			
		}
		
	}
}

/********************************* SKYCODE ************************************/
