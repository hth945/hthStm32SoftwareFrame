#include "bsp_USART3.h"	  
#include "myFIFO.h"
#include "commonIOInterface.h"
  
static sky_comDriver USART3_Driver;
static int uart3Write(struct _Tsky_comDriver *self, uint8_t *data,int len);

sky_comDriver* uart3_init(uint32_t bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	memset(&USART3_Driver,0,sizeof(sky_comDriver));
	USART3_Driver.write = uart3Write;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3，GPIOA时钟

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART3，GPIOA时钟

	//USART3_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.9

	//USART3_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA10
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
	
	
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.10  

	//USART3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
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

	USART_Init(USART3, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART3, ENABLE);                    //使能串口1 

	return &USART3_Driver;
}

static int uart3Write(struct _Tsky_comDriver *self, uint8_t *data,int len)
{
	while(len > 0)
	{
		while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
		USART3->DR = (uint8_t) *data;      
		len--;
		data++;
	}
	return 0;
}

void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t Res;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		if (USART3_Driver.fifo != 0)
			myFIFOWrite(USART3_Driver.fifo, &Res, 1);

		if (USART3_Driver.single != 0)
			USART3_Driver.single(USART3_Driver.userData, &Res, 1);
		 
     } 
} 

