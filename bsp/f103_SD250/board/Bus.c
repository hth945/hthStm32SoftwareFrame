/********************************* SKYCODE *************************************
* Copyright (c) 2015-2016, 深圳思凯测试技术有限公司（广州天码电子技术有限公司）
* All rights reserved.
*
* 文件名   ：Bus.c    
* 描述     ：Bus 通讯总线驱动程序
*
* 版    本 ：V0.4
* 作    者 ：SKYCODE2013
* 完成日期 ：2016-12-16
* 修改描述 ：1. 结合Bus1.0 和 mbus
*            2. Bus 1.0 波特率是500k
*
* 版    本 ：V0.3
* 作    者 ：SKYCODE2013
* 完成日期 ：2016-11-16
* 修改描述 ：1. 修改添加接收滤波器的漏洞
*            2. 更名BUS
*            3. 删除了64B PC13脚的修正
*
* 版    本 ：V0.2
* 作    者 ：SKYCODE2013
* 完成日期 ：2016-05-28
* 修改描述 ：CAN中断向量包含在此
*
* 版    本 ：V0.1
* 作    者 ：SKYCODE2013
* 完成日期 ：2016-04-01
* 修改描述 ：
*					                          	
*******************************************************************************/

#include "Bus.h"
#include "stm32f10x.h"

BusFilterStruct sg_bus_filter[BUS_MAX_FILTER]; //BUS_MAX_FILTER个滤波器的状态

/********************************************************************************
*函得：CANBus_Init
*描述  ：CAN底层 初始化。此函数只能调用一次，它会清除所有Bus模块的添加记录
*参数  ：1.bus_type 选择总线类型； =0 mbus, =1 Bus1.0 
*返回  ：无      
********************************************************************************/
static void CANBus_Init(unsigned int bus_type)
{
	unsigned int i;
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
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
  	
    //设置波特率  
    if(bus_type == 0)
    {
        //250k
        CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
        CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;//CAN_BS1_6tq;//5;  //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
        CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;//0;  Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
        CAN_InitStructure.CAN_Prescaler = 18;  //分频系数17;    
    }
    else if(bus_type == 1)
	{
        //500k
        CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
        CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;//CAN_BS1_6tq;//5;  //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
        CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;//0;  Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
        CAN_InitStructure.CAN_Prescaler = 9;  //分频系数8;    
	}
    
    CAN_Init(CAN1, &CAN_InitStructure);      // 初始化CAN1
	
	//CAN中断向量
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级 = 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //3       //子优先级 = 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//清除滤波器组
	for(i=0; i<BUS_MAX_FILTER; i++)
	{
		sg_bus_filter[i].filt_dst = 0;
		sg_bus_filter[i].filt_src = 0;
	}
}

/********************************************************************************
*函得：CANBus_AddRxMdl
*描述  ：增加接收模块
*参数  ：1. bus_type        -  =0 mbus, =1 bus 1.0
*        2. BusFilterStruct -  结构体指针
*
*返回  ：接收过滤器编号 0~ (BUS_MAX_FILTER-1)
*       =BUS_ADDRXMLD_ERROR  错误 没有要过滤的
*       =BUS_ADDRXMLD_REPEAT 重复添加    
*		=BUS_ADDRXMLD_NOFILT 滤波器用完，增加失败         
********************************************************************************/
static u8 CANBus_AddRxMdl(unsigned int bus_type, BusFilterStruct* filt_struct)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	u8 fmi;

	//判断没有要过滤的就是错
	if((filt_struct->filt_dst==0) && (filt_struct->filt_src==0))
	{
		return BUS_ADDRXMLD_ERROR;
	}

	//判断是否重复添加
	if((filt_struct->filt_dst==1) && (filt_struct->filt_src==1))
	{
		for(fmi=0; fmi<BUS_MAX_FILTER; fmi++)
		{
			if((sg_bus_filter[fmi].filt_dst == 1) && (sg_bus_filter[fmi].filt_src==1) &&
			   (sg_bus_filter[fmi].dst_type == filt_struct->dst_type) && //目标类型和ID重复 ？ 
			   (sg_bus_filter[fmi].dst_id == filt_struct->dst_id) &&
			   (sg_bus_filter[fmi].src_type == filt_struct->src_type) && //源类型和ID重复 ？ 
			   (sg_bus_filter[fmi].src_id == filt_struct->src_id))
			{
				return BUS_ADDRXMLD_REPEAT; //重复添加接收
			}
		}
	}
	else if((filt_struct->filt_dst==1) && (filt_struct->filt_src==0))
	{
		for(fmi=0; fmi<BUS_MAX_FILTER; fmi++)
		{
			if((sg_bus_filter[fmi].filt_dst == 1) && (sg_bus_filter[fmi].filt_src==0) &&
			   (sg_bus_filter[fmi].dst_type == filt_struct->dst_type) && //目标类型和ID重复 ？ 
			   (sg_bus_filter[fmi].dst_id == filt_struct->dst_id))
			{
				return BUS_ADDRXMLD_REPEAT; //重复添加接收
			}
		}
	}
	else if((filt_struct->filt_dst==0) && (filt_struct->filt_src==1))
	{
		for(fmi=0; fmi<BUS_MAX_FILTER; fmi++)
		{
			if((sg_bus_filter[fmi].filt_dst == 0) && (sg_bus_filter[fmi].filt_src==1) &&
			   (sg_bus_filter[fmi].src_type == filt_struct->src_type) && //源类型和ID重复 ？ 
			   (sg_bus_filter[fmi].src_id == filt_struct->src_id))
			{
				return BUS_ADDRXMLD_REPEAT; //重复添加接收
			}
		}
	}
	
	
	
	//判断过滤器是否用完 和分配过滤器
	for(fmi=0; fmi<BUS_MAX_FILTER; fmi++)
	{
		if((sg_bus_filter[fmi].filt_dst==0) && (sg_bus_filter[fmi].filt_src==0))break;
	}
	
	if(fmi >= BUS_MAX_FILTER) //滤波器用完
	{
		return BUS_ADDRXMLD_NOFILT; //没有可用过滤器
	}
		
	CAN_FilterInitStructure.CAN_FilterNumber=fmi;//	  //过滤器 
 	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 

    if(bus_type == 0) //mbus
    {
        CAN_FilterInitStructure.CAN_FilterIdHigh=(0x5<<13)|
          ((u16)(filt_struct->dst_type&0x0f)<<9)|((u16)(filt_struct->dst_id&0x0f)<<5) | //目标(本地)类型和ID
          ((filt_struct->src_type&0x0f)<<1)|((filt_struct->src_id&0x0f)>>3);
        CAN_FilterInitStructure.CAN_FilterIdLow=((u16)(filt_struct->src_id&0x0f)<<13) | 0x0004;
        
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xE000;//32位MASK
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0006;
        if(filt_struct->filt_dst == 1) //目标类型 ID 要滤波
        {
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh |= 0xFFE0;
        }
        if(filt_struct->filt_src == 1) //源类型 ID 要滤波
        {
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh |= 0x001F;
            CAN_FilterInitStructure.CAN_FilterMaskIdLow |= 0xE000;
        }
    }
    else if(bus_type == 1) //bus 1.0
    {
        CAN_FilterInitStructure.CAN_FilterIdHigh=(0x1<<13)|  //^
          ((u16)(filt_struct->dst_type&0x03f)<<7)|((u16)(filt_struct->dst_id&0x07)<<4) | //目标(本地)类型和ID
          ((filt_struct->src_type&0x3f)>>2); //源类型[5:2]
        
        CAN_FilterInitStructure.CAN_FilterIdLow=((u16)((filt_struct->src_type&0x03)<<14)| //源类型[1:0]
                                                 ((filt_struct->src_id&0x7)<<11)|  //源ID
                                                  0x0004);
        
        
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xE000;//32位MASK
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0006;
        if(filt_struct->filt_dst == 1) //目标类型 ID 要滤波
        {
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh |= 0xFFF0;
        }
        if(filt_struct->filt_src == 1) //源类型 ID 要滤波
        {
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh |= 0x000F;
            CAN_FilterInitStructure.CAN_FilterMaskIdLow |= 0xF800;
        }
    }

  
 	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;//过滤器x 关联到FIFO0
 	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
    
	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
    
	//记录
	sg_bus_filter[fmi].rx_irq_handler = filt_struct->rx_irq_handler;
	sg_bus_filter[fmi].userData = filt_struct->userData;
	sg_bus_filter[fmi].filt_dst = filt_struct->filt_dst;
	sg_bus_filter[fmi].filt_src = filt_struct->filt_src; 
	sg_bus_filter[fmi].dst_type = filt_struct->dst_type;
	sg_bus_filter[fmi].dst_id = filt_struct->dst_id;
	sg_bus_filter[fmi].src_type = filt_struct->src_type;
	sg_bus_filter[fmi].src_id = filt_struct->src_id;
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.
    

	return fmi;
}

/********************************************************************************
*函得：CANBus_SendMsg
*描述  ：Bus l送消息
*参数  ：1.bus_type 选择总线类型； =0 mbus, =1 Bus1.0 
*        2.msg 消息结构体 请看BusTxMsgStruct 结构体的声明
*返回  ：l送消息用到的邮箱号， 0~2
*        =BUS_NO_MB 说明没有空邮箱，l送失败
*        =BUS_TXFAILED 
********************************************************************************/
static u8 CANBus_SendMsg(unsigned int bus_type, BusTxMsgStruct* bus_tx_msg)
{
	CanTxMsg* can_tx_msg;
	u8 mbox; //邮箱号
	unsigned int i=0;

	
	bus_tx_msg->resev2 = 0;
	bus_tx_msg->resev3 = 0;
	
	can_tx_msg = (CanTxMsg*)bus_tx_msg;
	
	if(bus_type == 0) //mbus
    {
        can_tx_msg->ExtId |= (0x5<<26);//CAN StdId的最高3位	
    }
    else if(bus_type == 1) //bus 1.0
    { 
        can_tx_msg->ExtId |= (0x1<<26);//CAN StdId的最高3位	
    }
    
	can_tx_msg->IDE = CAN_ID_EXT; //CAN 的IDE 扩展标识符
	can_tx_msg->RTR = CAN_RTR_DATA; //CAN 的 RTR 远程帧标识 设为数据帧
	
	mbox = CAN_Transmit(CAN1, can_tx_msg);
	
	if(mbox == CAN_NO_MB)
	{
		return BUS_NO_MB;
	}
	
	while((CAN_TransmitStatus(CAN1, mbox)!=CANTXOK)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)
	{
		return BUS_TXFAILED;
	}
		
	return mbox;
	
}


/********************************************************************************
*函得：USB_LP_CAN1_RX0_IRQHandler
*描述  ：CAN1接收中断
*参数  ：无
*返回  ：无     
********************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	if((sg_bus_filter[RxMessage.FMI].filt_dst==1) || (sg_bus_filter[RxMessage.FMI].filt_src==1))
	{
		//sg_bus_filter[RxMessage.FMI].rx_irq_handler((BusRxMsgStruct* )(&RxMessage));
		
		sg_bus_filter[RxMessage.FMI].rx_irq_handler((BusRxMsgStruct* )(&RxMessage),sg_bus_filter[RxMessage.FMI].userData);
	}
	
}

////////////////////////////////////Bus 1.0//////////////////////////////////////

/********************************************************************************
*函得：Bus_Init
*描述  ：Bus 总线初始化。此函数只能调用一次，它会清除所有Bus模块的添加记录
*参数  ：无
*返回  ：无      
********************************************************************************/
 void Bus_Init(void)
{
    CANBus_Init(1);
}


/********************************************************************************
*函得：Bus_AddRxMdl
*描述  ：增加接收模块
*参数  ：BusFilterStruct 结构体指针
*
*返回  ：接收过滤器编号 0~(BUS_MAX_FILTER-1)
*       =BUS_ADDRXMLD_ERROR  错误 没有要过滤的
*       =BUS_ADDRXMLD_REPEAT 重复添加    
*		=BUS_ADDRXMLD_NOFILT 滤波器用完，增加失败         
********************************************************************************/
u8 Bus_AddRxMdl(BusFilterStruct* filt_struct)
{
    return CANBus_AddRxMdl(1, filt_struct);
}

void Bus_RemoveRxMdl(u8 filter_num)
{

}

/********************************************************************************
*函得：Bus_SendMsg
*描述  ：Bus l送消息
*参数  ：msg 消息结构体 请看BusTxMsgStruct 结构体的声明
*返回  ：l送消息用到的邮箱号， 0~2
*        =BUS_NO_MB 说明没有空邮箱，l送失败
*        =BUS_TXFAILED 
********************************************************************************/
u8 Bus_SendMsg(BusTxMsgStruct* bus_tx_msg)
{
	return CANBus_SendMsg(1, bus_tx_msg);
}

/********************************************************************************
*函得：Bus_DisableRxIT
*描述  ：禁止Bus 接收中断
*参数  ：无
*返回  ：无     
********************************************************************************/
void Bus_DisableRxIT(void)
{
	/* Disable the selected CAN interrupt */
    CAN1->IER &= ~CAN_IT_FMP0;
}

/********************************************************************************
*函得：Bus_EnableRxIT
*描述  ：使能Bus 接收中断
*参数  ：无
*返回  ：无     
********************************************************************************/
void Bus_EnableRxIT(void)
{
	/* Enable the selected CAN interrupt */
    CAN1->IER |= CAN_IT_FMP0;
}




////////////////////////////////////mBus////////////////////////////////////////


/*
*函得：mBus_Init
*描述  ：mBus 总线初始化。此函数只能调用一次，它会清除所有mBus模块的添加记录
*参数  ：无
*返回  ：无      
*/
void mBus_Init(void)
{
    CANBus_Init(0);    
}


/*
*函得：mBus_AddRxMdl
*描述  ：增加接收模块
*参数  ：mBusFilterStruct 结构体指针
*
*返回  ：接收过滤器编号 0~13
*       =MBUS_ADDRXMLD_ERROR  错误 没有要过滤的
*       =MBUS_ADDRXMLD_REPEAT 重复添加    
*		=MBUS_ADDRXMLD_NOFILT 滤波器用完，增加失败  
*/
u8 mBus_AddRxMdl(mBusFilterStruct* filt_struct)
{
    return CANBus_AddRxMdl(0, filt_struct);
}


/*
*函得：mBus_SendMsg
*描述  ：mBus l送消息
*参数  ：mbus_tx_msg - 消息结构体 请看mBusTxMsgStruct 结构体的声明
*返回  ：l送消息用到的邮箱号， 0~2
*        =MBUS_NO_MB 说明没有空邮箱，l送失败
*        =MBUS_TXFAILED 
*/
u8 mBus_SendMsg(mBusTxMsgStruct* mbus_tx_msg)
{
    return CANBus_SendMsg(0, (BusTxMsgStruct*) mbus_tx_msg);
}

/********************************* SKYCODE ************************************/
