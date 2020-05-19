#include "driverUsart1.h"
#include <stdarg.h>
#include "myFIFO.h"
#include "commonIOInterface.h"


static sky_comDriver Usart1_Driver;
static int uart1Write(struct _Tsky_comDriver *self, uint8_t *data,int len);

/******************************************************************************
 * 函数名：USART7_Init
 * 描述  ：USART7 GPIO 配置,工作模式配置。115200 8bit o奇偶校 1停止位
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 *****************************************************************************/
sky_comDriver* USART1_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    
    /**/

	memset(&Usart1_Driver,0,sizeof(sky_comDriver));
	Usart1_Driver.write = uart1Write;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); 
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断


	//UART7 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =5;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
    
    USART_Cmd(USART1, ENABLE);
	
	return &Usart1_Driver;
}

static int uart1Write(struct _Tsky_comDriver *self, u8 *data,int len)
{
	while(len > 0)
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = (u8) *data;      
		len--;
		data++;
	}
	return 0;
}


//$Sub$$:定义新功能函数,在UART7_IRQHandler函数(之前/之后)使用$Sub$$UART7_IRQHandler可以添加该函数一些新的程序代码
//void $Sub$$UART7_IRQHandler(void)
void USART1_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
		Res =USART_ReceiveData(USART1);

		if (Usart1_Driver.fifo != 0)
			myFIFOWrite(Usart1_Driver.fifo, &Res, 1);

		if (Usart1_Driver.single != 0)
			Usart1_Driver.single(Usart1_Driver.userData, &Res, 1);
	}
}



/******************************* 广州天码电子 *********************************/
