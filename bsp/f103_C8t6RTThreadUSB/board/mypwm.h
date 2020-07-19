#ifndef __MYPWM_H
#define	__MYPWM_H

#include "sys.h"
#include <stdio.h>


typedef struct
{
	//void* next;     //链表 中断里根据它来循环IO设置
	uint8_t run_state;
	uint8_t dir;
	uint32_t speed;// = 400.0  #脉冲每秒  速度最快为100k  最小 20
	uint32_t Acceleration;// = 1000.0
	uint32_t Deceleration;// = 2000.0
	uint32_t targetX;// =1000.0 # 位移 单位:脉冲
	
	uint32_t Ax; //加速位移 单位:脉冲
	uint32_t DX; //减速位移 单位:脉冲
	uint32_t Ux; //匀速位移 单位:脉冲
	
	uint32_t Atime;  //加速总时间 单位:最小时间精度
	uint32_t Dtime;  //减速总时间 单位:最小时间精度
	
	uint32_t count; //当前时间计数 单位:最小时间精度
	uint32_t T;     //当前周期 单位:最小时间精度
	uint32_t nowX;  //当前位置 单位:脉冲
	
}StepperMotor;

struct _TServo;
typedef struct _TServo
{
	//void* next;     //链表 中断里根据它来循环IO设置
	uint8_t run_state; //是否使能
	uint32_t HighTime; //高电平时间  单位 us
	uint32_t bitBand;     //位带操作写io 更快
//	int (*pinWrite)(struct _TServo *self, int);
//	uint32_t TTime;  //一个周期时间  单位 us  固定20000 us
//	uint32_t count; //当前时间计数 单位:最小时间精度
}Servo;

extern Servo servos[16];
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



