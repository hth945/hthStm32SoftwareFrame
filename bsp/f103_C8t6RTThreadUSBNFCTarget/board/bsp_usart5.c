#include "bsp_usart5.h"	  
#include "myFIFO.h"
#include "commonIOInterface.h"
  
static sky_comDriver Usart5_Driver;
static int uart5Write(struct _Tsky_comDriver *self, uint8_t *data,int len);

sky_comDriver* uart5_init(uint32_t bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	memset(&Usart5_Driver,0,sizeof(sky_comDriver));
	Usart5_Driver.write = uart5Write;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//使能Usart5，GPIOA时钟

	//Usart5_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA.9

	//Usart5_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart5 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(UART5, &USART_InitStructure); //初始化串口1
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(UART5, ENABLE);                    //使能串口1 

	return &Usart5_Driver;
}

static int uart5Write(struct _Tsky_comDriver *self, uint8_t *data,int len)
{
	while(len > 0)
	{
		while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
		UART5->DR = (uint8_t) *data;      
		len--;
		data++;
	}
	return 0;
}

void UART5_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t Res;

	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(UART5);	//读取接收到的数据
		Write2dev(&Usart5_Driver, &Res, 1);
    } 
} 

