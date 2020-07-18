#include "mypwm.h"
#include "string.h"
#include "Public_util.h"

//开方
uint32_t axis_sqrt(uint32_t x)
{
  register uint32_t xr;  // result register
  register uint32_t q2;  // scan-bit register
  register uint8_t f;   // flag (one bit)

  xr = 0;                     // clear result
  q2 = 0x40000000L;           // higest possible result bit
  do
  {
    if((xr + q2) <= x)
    {
      x -= xr + q2;
      f = 1;                  // set flag
    }
    else{
      f = 0;                  // clear flag
    }
    xr >>= 1;
    if(f){
      xr += q2;               // test flag
    }
  } while(q2 >>= 2);          // shift twice
  if(xr < x){
    return xr +1;             // add for rounding
  }
  else{
    return xr;
  }
}
Servo servos[16];

void TIM3_Int_Init(u16 arr,u16 psc) //72M
{
	int i = 0;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	
	
	
	memset(servos, 0, sizeof(servos));
	
//	GPIOB->CRL&=0XFF0FFFFF; 
//	GPIOB->CRL|=0X00300000;//PB.5 推挽输出  
	
	GPIOA->CRL=0X33333333; 
	GPIOB->CRL=0X33333333; 
	for (i = 0; i < 8; i++)
	{
		servos[i].bitBand = BITBAND(GPIOA_ODR_Addr, i);
		servos[i+8].bitBand = BITBAND(GPIOB_ODR_Addr, i);
	}
	
	
	TIM_TimeBaseStructure.TIM_Period = arr; 	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM3, ENABLE);  
	
	servos[0].HighTime =  0;//20000-1;
	servos[0].run_state = 1;
	
}


uint32_t ServoCount = 0; //当前时间计数 单位:最小时间精度
void TIM3_IRQHandler(void)   //TIM3ÖÐ¶Ï
{
	int i;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
		
		ServoCount++;
		if (ServoCount > 20000)  //20ms
		{
			ServoCount = 0;
			GPIOA->ODR = 0;
			GPIOB->ODR = 0;
//			for (i = 0; i < 16; i++)
//			{
//				if (servos[i].run_state != 0)
//					MEM_ADDR(servos[i].bitBand) = 1;
//			}
		}
		
		for (i = 0; i < 16; i++)
		{
			if (servos[i].run_state != 0)
			{
				if (servos[i].HighTime == ServoCount)
				{
					MEM_ADDR(servos[i].bitBand) = 1;
				}
			}
		}
	}
}



void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
	                                                                     	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  
	
	

  TIM_CtrlPWMOutputs(TIM1,ENABLE);	//
  
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	TIM_Cmd(TIM1, ENABLE);  //Ê¹ÄÜTIM1
 
   
}


