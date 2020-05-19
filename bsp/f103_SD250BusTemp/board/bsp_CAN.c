#include "bsp_CAN.h"
//#include "Bus.h"
#include "stm32f10x.h"

static can_adapter canDev1;


static void BSPCAN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef CAN_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//使能PORTA时钟	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
    GPIO_Init(GPIOB, &GPIO_InitStructure);		//初始化IO
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化IO
    
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE); 	
    
 	//初始化CAN1 为500K波特率 

	//CAN单元设置
 	CAN_InitStructure.CAN_TTCM=DISABLE;			//非时间触发通信模式  //
 	CAN_InitStructure.CAN_ABOM=DISABLE;			//软件自动离线管理	 //
  	CAN_InitStructure.CAN_AWUM=DISABLE;		    //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)//
//  	CAN_InitStructure.CAN_NART=DISABLE;         //自动发送至成功
	CAN_InitStructure.CAN_NART=ENABLE;		    //禁止报文自动传送 //
  	CAN_InitStructure.CAN_RFLM=DISABLE;			//报文不锁定,新的覆盖旧的 // 
  	CAN_InitStructure.CAN_TXFP=ENABLE;		    //优先级由请求顺序决定 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;//模式设置： mode:0,普通模式;1,回环模式; //
  	
	//500k
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;//CAN_BS1_6tq;//5;  //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;//0;  Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = 9;  //分频系数8;    
	
    
    CAN_Init(CAN1, &CAN_InitStructure);      // 初始化CAN1
	
	//CAN中断向量
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级 = 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //3       //子优先级 = 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


int CAN1SendMsg(uint32_t id, uint8_t *data, int len)
{
	CanTxMsg can_tx_msg;
	u8 mbox; //邮箱号
	unsigned int i=0;
	
	//注意  注意 注意  id的最高7位不能全为1
	//id 有3位不能用 只有29位
	if ((id&0x1fffffff) > 0x1fc00000)
	{
		while(1);
	}
	
	memset(&can_tx_msg, 0, sizeof(CanTxMsg));
	memcpy(&can_tx_msg.Data, data, len);
	
	//can_tx_msg.ExtId |= (0x1<<26);//CAN StdId的最高3位	
	can_tx_msg.ExtId = id;
	can_tx_msg.DLC = len;
	can_tx_msg.IDE = CAN_ID_EXT; //CAN 的IDE 扩展标识符
	can_tx_msg.RTR = CAN_RTR_DATA; //CAN 的 RTR 远程帧标识 设为数据帧
	
	mbox = CAN_Transmit(CAN1, &can_tx_msg);
	
	if(mbox == CAN_NO_MB)
	{
		return -1;
	}
	
	while((CAN_TransmitStatus(CAN1, mbox)!=CANTXOK)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)
	{
		return -2;
	}
	
	return mbox;
}

int BSP_CAN1AddFilter(int filterN, uint32_t id, uint32_t mask)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	if(filterN >= 28) //滤波器用完
	{
		return -1; //没有可用过滤器
	}
	
	CAN_FilterInitStructure.CAN_FilterNumber=filterN;//	  //过滤器 
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 

	CAN_FilterInitStructure.CAN_FilterIdHigh=(u16)((id>>16)&0xffff);

	CAN_FilterInitStructure.CAN_FilterIdLow=(u16)((id&0xffff) | 0x0004);

	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=(u16)((mask >> 13)&0xffff);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=(u16)((mask << 3)&0xffff);
	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh |= 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow |= 0x0006;  //最低三位为帧标识
	
  
 	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;//过滤器x 关联到FIFO0
 	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
    
	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
    
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.
    
	return filterN;
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	P_CAN_IRQHandler(&canDev1, RxMessage.ExtId, RxMessage.Data, RxMessage.DLC);	
}

can_adapter* BSP_CAN1_Init(void)
{
	//硬件初始化
	BSPCAN_Init();
	
	//句柄初始化
	P_CAN_DeInit(&canDev1);
	canDev1.sendMsg = CAN1SendMsg;
	canDev1.addFilter = BSP_CAN1AddFilter;
	P_CAN_Init(&canDev1);
	
	return &canDev1;
}






