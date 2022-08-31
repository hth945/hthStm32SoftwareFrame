#include "bsp_usart1.h"	  
#include "myFIFO.h"
#include "commonIOInterface.h"
  
static sky_comDriver Usart1_Driver;
static int uart1Write(struct _Tsky_comDriver *self, uint8_t *data,int len);

sky_comDriver* uart1_init(uint32_t bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	memset(&Usart1_Driver,0,sizeof(sky_comDriver));
	Usart1_Driver.write = uart1Write;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
	
	
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 

	return &Usart1_Driver;
}

static int uart1Write(struct _Tsky_comDriver *self, uint8_t *data,int len)
{
	while(len > 0)
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = (uint8_t) *data;      
		len--;
		data++;
	}
	return 0;
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		if (Usart1_Driver.fifo != 0)
			myFIFOWrite(Usart1_Driver.fifo, &Res, 1);

		if (Usart1_Driver.single != 0)
			Usart1_Driver.single(Usart1_Driver.userData, &Res, 1);
		 
     } 
} 

