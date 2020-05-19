/********************************* SKYCODE ************************************
* Copyright (c) 2015-2017, 深圳思凯测试技术有限公司
* All rights reserved.
*
* 文件名   ：usart7.h    
* 描述     ：
*
* 版    本 ：V1.0.0
* 作    者 ：
* 完成日期 ：2017-12-29
* 修改描述 ：
*					                          	
******************************************************************************/

#include "driverUsart7.h"
#include <stdarg.h>
#include "myFIFO.h"
#include "commonIOInterface.h"


static sky_comDriver Usart7_Driver;
static int uart7Write(u8 *data,int len);

/******************************************************************************
 * 函数名：USART7_Init
 * 描述  ：USART7 GPIO 配置,工作模式配置。115200 8bit o奇偶校 1停止位
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 *****************************************************************************/
sky_comDriver* USART7_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    
    /**/

	memset(&Usart7_Driver,0,sizeof(sky_comDriver));
	Usart7_Driver.write = uart7Write;
	
	/**/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7,ENABLE);//使能USART7时钟
 
	//串口7对应引脚复用映射
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource6,GPIO_AF_UART7); //复用为UART7
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7,GPIO_AF_UART7); //复用为UART7
	
	//USART7端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure); //初始化PA9，PA10
	
	//UART7 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART7_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =4;		//子优先级4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);
	
	/* USART7 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(UART7, &USART_InitStructure); 

	
    USART_ITConfig(UART7, USART_IT_RXNE, ENABLE);//开启相关中断
	
	USART_Cmd(UART7, ENABLE);
	
	USART_ITConfig(UART7, USART_IT_RXNE, ENABLE);//开启相关中断

	//UART7 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART7_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =5;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	return &Usart7_Driver;
}

sky_comDriver* CA310_USART7_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	memset(&Usart7_Driver,0,sizeof(sky_comDriver));
	Usart7_Driver.write = uart7Write;
	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7,ENABLE);//使能UART7时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource6,GPIO_AF_UART7); //GPIOA9复用为UART7
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7,GPIO_AF_UART7); //GPIOA10复用为UART7
	
	//UART7端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure); //初始化PA9，PA10
	
	/* UART7 mode config */
	USART_InitStructure.USART_BaudRate = 38400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_Even ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART7, &USART_InitStructure); 
	USART_Cmd(UART7, ENABLE);
	
	USART_ITConfig(UART7, USART_IT_RXNE, ENABLE);//开启相关中断

	//UART7 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART7_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =5;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	return &Usart7_Driver;
}


static int uart7Write(u8 *data,int len)
{
	while(len > 0)
	{
		while((UART7->SR&0X40)==0);//循环发送,直到发送完毕   
		UART7->DR = (u8) *data;      
		len--;
		data++;
	}
	
	return 0;
}


//$Sub$$:定义新功能函数,在UART7_IRQHandler函数(之前/之后)使用$Sub$$UART7_IRQHandler可以添加该函数一些新的程序代码
//void $Sub$$UART7_IRQHandler(void)
void UART7_IRQHandler(void)
{
//	u8 Clear=Clear;
//	
//	if(USART_GetITStatus(UART7,USART_IT_RXNE)!=RESET)
//	{
//		UART7_Rx_Buffer[UART7_Rx_ByteCount++]=UART7->DR;
//		if(UART7_Rx_ByteCount>=2)
//			UART7_Rx_DataFrameFlag=1;
//	}
//	else if(USART_GetITStatus(UART7,USART_IT_IDLE)!=RESET)
//	{
//		Clear=UART7->SR;
//		Clear=UART7->DR;
//		UART7_Rx_DataFrameFlag=1;
//	}
	u8 Res;
	if(USART_GetITStatus(UART7,USART_IT_RXNE)!=RESET)
	{
		Res =USART_ReceiveData(UART7);

		if (Usart7_Driver.fifo != 0)
			myFIFOWrite(Usart7_Driver.fifo, &Res, 1);

		if (Usart7_Driver.single != 0)
			Usart7_Driver.single(Usart7_Driver.userData, &Res, 1);
	}
}



/******************************* 广州天码电子 *********************************/
