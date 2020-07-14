#ifndef __MYPWM_H
#define	__MYPWM_H

#include "sys.h"
#include <stdio.h>


//72M
void TIM3_Int_Init(u16 arr,u16 psc);


void TIM1_PWM_Init(u16 arr,u16 psc);
	
//void TIM8_PWM_Init(u16 arr,u16 psc);
//void TIM9_PWM_Init(u16 arr,u16 psc);

//void TIM10_PWM_Init(u16 arr,u16 psc);
//void TIM11_PWM_Init(u16 arr,u16 psc);

//void TIM10_PWM_setarr(int v);
//void TIM11_PWM_setarr(int v);

//	舵机常用
//	TIM8_PWM_Init(9999,167);	//100hz.
//	
//	TIM_SetCompare1(TIM8,499);	//0.5ms
//	TIM_SetCompare2(TIM8,2499);	//2.5ms
//	TIM_SetCompare3(TIM8,1499);	//1.5ms
//	TIM_SetCompare4(TIM8,4999);	//5ms
//	
//	
////	TIM8_PWM_Init(167,0);	//TIM4 PWM初始化, Fpwm=168M/168=1000Khz.
////	TIM_SetCompare2(TIM8,83);	//输出
////	TIM_SetCompare1(TIM8,41);	//输出
//  电机常用
//	TIM9_PWM_Init(99,167);	//10Khz.
//	TIM_SetCompare2(TIM9,49);	//输出
//	TIM_SetCompare1(TIM9,89);	//输出GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	


#endif



