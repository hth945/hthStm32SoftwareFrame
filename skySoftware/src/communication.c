#include "communication.h"

int sky_Delay_ms(sky_MyByteReceiveDataTypeDef *myRecType, int ms)
{
	uint32_t tmp;
	tmp = myRecType->getTime();
	while((myRecType->getTime() - tmp) < ms);
	
	return 0;
}

int sky_devSingle(sky_comDriver *dev,uint8_t *data, int len)
{
	sky_MyByteReceiveDataTypeDef *myRecType =(sky_MyByteReceiveDataTypeDef *) dev->Parent;
	return sky_receivePactData(myRecType, data, len);
}

int sky_ConmBindDriver(sky_MyByteReceiveDataTypeDef *myRecType,sky_comDriver *driver)
{
//	myRecType->send = driver->write;
//	driver->userData = myRecType;
//	driver->single = (int (*)(void *userData,uint8_t *data, int len))sky_receivePactData;
	
	myRecType->dev = driver;
//	myRecType->dev->userData = myRecType;
	myRecType->dev->Parent = myRecType;
	driver->single = sky_devSingle;
	return 1;
}

void sky_ConmmunicationInit(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t *data,uint32_t len,uint8_t startC,uint8_t endC,uint32_t waitTimeT,sky_conmmunicationGetTime getTime)
{
	int off;
	
	memset(myRecType,0,sizeof(sky_MyByteReceiveDataTypeDef));
	
	//因为需要保证接收到的数据正文段位4字节对齐的 因此接收缓冲区起始必须 off%4=2
	off = (uint32_t)data % 4;
	off = (6 - off) % 4;
	
	myRecType->data = (data + off);
	myRecType->startc = startC;
	myRecType->endc = endC;
	myRecType->maxlen = len-off;
	myRecType->saveFlag = 0;
	myRecType->saveNum = 0;
	myRecType->oldTime = 0;
	myRecType->dataLen = 0;
	myRecType->waitTimeT = waitTimeT;
	myRecType->getTime=getTime;
}

unsigned int sky_GetMeasureTime_ms(sky_MyByteReceiveDataTypeDef *myRecType, unsigned int ts)
{
	uint32_t tmp;
	tmp = myRecType->getTime();

    if(tmp < ts)
	{
		tmp = 0xffffffff - ts + tmp;
	}
	else
	{
		tmp = tmp - ts;
	}
	return tmp;
}

int sky_receivePactData(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t *data, int len)
{
	int i;
	int rec;
	
	for (i = 0; i < len; i++)
	{
		rec = sky_receivePact(myRecType,data[i]);
	}
	return rec;
}

/********************************************************************************
*函數名：receivePact
*描述  ：按协议接收一帧数据，放入接收中断中，每次传入一个字节，带校验与超时判断50ms
		若上一帧数据没有处理完（标志位没有清0），则不接收新数据。
*参数  ：1.myRecType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.newc       此时接收到的一个字节
*返回  ：返回通信状态标志  0 正在等待  
						1 正在接收数据头部提取数据长度  
						2 正在校验数据  
						3 接收到一帧完整的数据
********************************************************************************/
int sky_receivePact(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t newc)
{
	int i=0;
	uint8_t num=0;
	uint32_t T;
	
	/************************超时判断************************/
	T = sky_GetMeasureTime_ms(myRecType, myRecType->oldTime);  
	myRecType->oldTime = myRecType->getTime();
	
	if((T > 50)&&(myRecType->saveFlag !=3))  //超时 并且没有接收完毕 那么从新开始
		myRecType->saveFlag = 0;
	
	/*********************按协议接收数据**********************************/
	if (myRecType->saveFlag == 0) //等待接收到起始字符
	{
		if (newc == myRecType->startc)  //接收到起始字符
		{
			myRecType->saveFlag=1;  //标志致1 进入下一步
			myRecType->saveNum=1;
			myRecType->data[0] = newc; 
		}
		
	}else if (myRecType->saveFlag == 1) //正在接收数据头部提取数据长度 
	{
		myRecType->data[myRecType->saveNum] = newc; 
		myRecType->saveNum++;	
		
		if(myRecType->saveNum >= 6)  //接收到的数据长度大于6  数据长度已传送过来
		{
			myRecType->dataLen = *((uint32_t *)(myRecType->data+2));
			
			if(myRecType->dataLen > myRecType->maxlen - 10)  //判断数据长度快到缓冲区最大长度  
				myRecType->saveFlag = 0;       //清除 重新开始接收
			else
				myRecType->saveFlag = 2;      //长度合格 进入下一步 继续接收
		}
		
	}else if(myRecType->saveFlag == 2)  //等待接收数据完成
	{
        myRecType->data[myRecType->saveNum] = newc; 
		myRecType->saveNum++;	
        
		if(myRecType->saveNum >= myRecType->dataLen + 8)  //数据接收完成 进入校验
		{
			for(num =0,i=1;i<myRecType->dataLen + 6;i++)  //校验数据
			{
				num ^= myRecType->data[i];
			}
			if((num == myRecType->data[myRecType->dataLen + 6]) && (myRecType->data[myRecType->dataLen + 7] == myRecType->endc))
			{
				myRecType->saveFlag = 3;
				if (myRecType->completeCallback != 0)
					myRecType->completeCallback(myRecType);
			}
			else
				myRecType->saveFlag = 0;
		}
	}
	
	return myRecType->saveFlag;
}


//从接收到的数据中 提取数据 到全局变量中
int sky_extractData(sky_MyByteReceiveDataTypeDef *recType,uint8_t *data)
{
	uint16_t offest;
	uint16_t len;
	
	if (recType->saveFlag == 3)    //等待接收完成
	{
		offest = *((uint16_t*)(&recType->data[6]));
		len =  *((uint16_t*)(&recType->data[8]));
		memcpy(((uint8_t *)data) + offest, (uint8_t *)(&recType->data[10]), len);
		return 0;
	}
	return -1;
}

//等待数据反馈 
int sky_waitData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,int time)
{
	uint32_t t = recType->getTime();
	
	while(1)
	{
		if(recType->saveFlag == 3)    //等待接收完成
		{
			if(*(recType->data+1) == (cmd|0x10))  //返回的命令进行校验  若接收端执行成功 会把第五位置1
			{
				return 1;
			}else
			{
				recType->saveFlag  = 0;
				return -2;            //执行错误 返回的是错误的数据
			}
		}
	
		if (sky_GetMeasureTime_ms(recType,t) > time)
		{
			break;
		}
		sky_Delay_ms(recType, recType->waitTimeT);
		
	}
	return -1;
}

//等待数据反馈 
int sky_waitNData(uint8_t PN,sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,int time)
{
	uint8_t tem = 0;
	int i = 0;
//	int j = 0;
	uint32_t t = recType[0].getTime();
	
	
	while (tem != PN)
	{
		for (i = 0; i < 8;i++)
		{
			if (PN & (1 << i))
			{
				if (recType[i].saveFlag == 3)
				{
					
					if (recType[i].data[1] != (cmd|0x10))
					{

					}else
					{
						tem |= (1 << i);
					}
				}
			}
		}
		
		if (sky_GetMeasureTime_ms(recType,t) > time)
		{
			return -(tem ^ PN);
		}
		sky_Delay_ms(recType, recType->waitTimeT);
		
//		j++;
//		if(j > time)
//		{
//			return -(tem ^ PN);
//		}
//		Delay_ms(1);
	}
	return tem;
}


/*******************************************************************************
 * 函数名 : pcui_sendData
 * 描  述 : pcui发送数据
 * 参  数 : *recType:通讯协议结构体
 * 参  数 : cmd:指令
 * 参  数 : **str:字符串指针
 * 参  数 : strLen:字符串个数
 * 返  回 : >=0:发送成功
            -1:发送失败
*******************************************************************************/
int sky_sendStrNData(sky_MyByteReceiveDataTypeDef *recType, uint8_t cmd, char **str, int strLen)
{
	uint8_t sbuf[10];
	int i = 0, j = 0;
	int len = 0;
	
	sbuf[0] = recType->startc;
	sbuf[1] = cmd;
	
	for(i = 0; i < strLen; i++)
		len += strlen(str[i]);
	*((uint32_t *)(&sbuf[2])) = len;
	if(devWrite(recType->dev, sbuf, 6) < 0)
		return -1;
	
	sbuf[6] = 0;
	sbuf[7] = recType->endc;
	
	for( i = 1; i < 6; i++)
		sbuf[6] ^= sbuf[i];
	
	for(i = 0; i < strLen; i++)
	{
		len = strlen(str[i]);
		
		for (j = 0; j < len; j++)
			sbuf[6] ^= str[i][j];
		
		if(devWrite(recType->dev, (uint8_t *)str[i], len) < 0)
			return -1;
	}
	if(devWrite(recType->dev,sbuf + 6, 2) < 0)
		return -1;
	
	return 0;
}


/********************************************************************************
*函數名：sendData
*描述  ：按协议发送一帧数据,结构体中send函数指针实现之后才可以使用
*参数  ：1.recType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.cmd       发送数据包的命令
		 3.data     发送的数据
		 4.len       发送的数据长度
*返回  ：正数 发送的数据长度  -1发送错误
********************************************************************************/
int sky_sendData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data,uint32_t len)
{
	uint8_t sbuf[10];
	int i;
	
	sbuf[0] = recType->startc;   //一字节起始字符
	sbuf[1] = cmd;				 //一字节命令	
	*((uint32_t*)(&sbuf[2])) = len;   //四字节长度
	
	if (devWrite(recType->dev, sbuf,6) < 0)
		return -1;

	sbuf[6] = 0;                //校验初始化为0
	sbuf[7] = recType->endc;    //结束字符
	
	for( i = 1; i < 6;i++)      //校验头
		sbuf[6] ^= sbuf[i];
	
	for (i = 0; i < len; i++)   //校验数据段
		sbuf[6] ^= data[i];
	
	if (devWrite(recType->dev, data,len) < 0) return -1;   //发送数据段
	if (devWrite(recType->dev, sbuf + 6,2) < 0) return -1; //发送结尾
	return len + 8;
}

/********************************************************************************
*函數名：send2Data
*描述  ：按协议发送一帧数据,数据段分为两段,结构体中send函数指针实现之后才可以使用
*参数  ：1.recType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.cmd       发送数据包的命令
		 3.data1     发送的第一段数据
		 4.len1       发送的第一段数据长度
		 5.data2     发送的第二段数据
		 6.len2       发送的第二段数据长度
*返回  ：正数 发送的数据长度  -1发送错误
********************************************************************************/
int sky_send2Data(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data1,uint32_t len1,uint8_t *data2,uint32_t len2)
{
	uint8_t sbuf[10];
	int j,i;
	
	sbuf[0] = recType->startc;
	sbuf[1] = cmd;
	*((uint32_t*)(&sbuf[2])) = len1+len2;
	if (devWrite(recType->dev, sbuf,6) < 0) return -1;
	
	sbuf[6] = 0;
	sbuf[7] = recType->endc;
	for( i = 1; i < 6;i++)
		sbuf[6] ^= sbuf[i];
	
	for (j = 0; j < len1; j++)
		sbuf[6] ^= data1[j];
	
	for (j = 0; j < len2; j++)
		sbuf[6] ^= data2[j];
	
	
	if (devWrite(recType->dev, data1,len1) < 0) return -1;
	if (devWrite(recType->dev, data2,len2) < 0) return -1;
	if (devWrite(recType->dev, sbuf + 6,2) < 0) return -1;
	return len1+len2+8;
}


int sky_sendNData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t type,uint8_t **data,int *len)
{
	uint8_t sbuf[10];
	int j,i;
	
	sbuf[0] = recType->startc;
	sbuf[1] = cmd;
	
	*((uint32_t*)(&sbuf[2])) = 0;
	sbuf[6] = 0;
	sbuf[7] = recType->endc;
	
	for (i = 0; i < type; i++)
	{
		*((uint32_t*)(&sbuf[2])) += len[i];
	}
	
	if (devWrite(recType->dev, sbuf,6) < 0)
		return -1;
	
	for( i = 1; i < 6;i++)
		sbuf[6] ^= sbuf[i];
	
	for (i = 0; i < type; i++)
	{
		for (j = 0; j < len[i]; j++)
			sbuf[6] ^= data[i][j];
		
		if (devWrite(recType->dev, data[i],len[i]) < 0)
			return -1;
	}
	
	if (devWrite(recType->dev, sbuf + 6,2) < 0) return -1;
	return *((uint32_t*)(&sbuf[2]))+8;
}



/********************************************************************************
*函數名：recOffsetType
*描述  ：按协议发送一帧数据,数据段分两字节偏移，两字节长度，第三段根据tpye判断书否包含数据
		即功能为 请求数据 还是发送数据,结构体中send函数指针实现之后才可以使用
*注意  ：此函数只发送  接收到的数据怎么处理与超时 请外部处理
*参数  ：1.recType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.data      数据的基地址
		 4.offset    要发送数据的偏移
		 5.len       要发送数据的长度
		 5.cmd       发送数据包的命令
		 6.type      是否发送具体的数据  1发送  0不发送
*返回  ：正数 发送的数据长度  -1发送错误
********************************************************************************/
int sky_recOffsetType(sky_MyByteReceiveDataTypeDef *recType,uint8_t *data, uint16_t offset, uint16_t len,uint8_t cmd, uint8_t type)
{
	uint8_t sbuf[12];
	int j,i;
	
	sbuf[0] = recType->startc;
	sbuf[1] = cmd;
	if ((type&0x01) == 1)
	{
		*((uint32_t*)(&sbuf[2])) = 4 + len;  //type为1时 数据长度为 len+2+2  (发送载荷 + 偏移 + 长度的值)
	}
	else
	{
		*((uint32_t*)(&sbuf[2])) = 4;        //type为0时 数据长度为 2+2  （偏移 + 长度的值)
	}
		
	
	*((uint16_t*)(&sbuf[6])) = offset;
	*((uint16_t*)(&sbuf[8])) = len;
	if (devWrite(recType->dev, sbuf,10) < 0) return -1;
	
	sbuf[10] = 0;
	sbuf[11] = recType->endc;
	for (i = 1; i < 10; i++)
		sbuf[10] ^= sbuf[i];
	
	if (type == 1)
	{
		for (j = 0; j < len; j++)
			sbuf[10] ^= data[j+offset];
		if (devWrite(recType->dev, data+offset,len) < 0) return -1;
	}
	if (devWrite(recType->dev, sbuf + 10,2) < 0) return -1;
	
	return *((uint32_t*)(&sbuf[2])) + 8;
}



/********************************************************************************
*函數名：runCmdPact
*描述  ：运行一条命令，并等待反馈,反馈数据在结构体中,结构体中send函数指针实现之后才可以使用
*注意  ：命令运行后数据在结构体中，若后续想一直接收，请主动清0 结构体中的标志
*参数  ：1.recType  通信的上下文结构体，内部有缓冲区，通信状态标志等信息
		 2.cmd       发送数据包的命令
		 3.data     发送的数据
		 4.len       发送的数据长度
		 5.time     超时时间
*返回  ：1 执行成功  -1 超时   -2 执行错误
********************************************************************************/
int sky_runCmdPact(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data,uint32_t len,uint32_t time)	
{
	recType->saveFlag  = 0;         //清除接收标志 打开接收
	sky_sendData(recType,cmd,data,len); //发送命令
	return sky_waitData(recType,cmd,time);
}


int sky_runCmdType(sky_MyByteReceiveDataTypeDef *recType, uint8_t *data, uint16_t offset, uint16_t len,uint8_t cmd, uint8_t type, int time)
{
	recType->saveFlag  = 0;         //清除接收标志 打开接收
	sky_recOffsetType(recType,data,offset,len,cmd,type); //发送命令
	if  (sky_waitData(recType,cmd,time) < 0)
		return -1;
	
	if ((type&0x02))
	{
		memcpy((uint8_t *)(data + *((uint16_t*)(&recType->data[6]))) ,&(recType->data[10]),*((uint16_t*)(&recType->data[8])));
	}
	
	return 0;
}


int sky_recSetVirType(sky_MyByteReceiveDataTypeDef *recType, uint8_t cmd, int time, uint16_t offset, uint16_t olen,uint8_t *data, uint8_t *data2,int len2)
{
	uint8_t *bdata[4];
	int blen[4];
	
	bdata[0] = (uint8_t *)(&offset);
	bdata[1] = (uint8_t *)(&olen);
	bdata[2] = data;
	bdata[3] = data2;
	blen[0] = 2;
	blen[1] = 2;
	blen[2] = olen;
	blen[3] = len2;
	
	recType->saveFlag  = 0;
	sky_sendNData(recType, cmd, 4, bdata, blen);
	return sky_waitData(recType,cmd,time);
}

int sky_recSetVirTypePN(uint8_t PN,sky_MyByteReceiveDataTypeDef *recType, uint8_t cmd, int time, uint16_t offset, uint16_t olen,uint8_t *data, uint8_t *data2,int len2)
{
	uint8_t *bdata[4];
	int blen[4];
	int i;
	
	bdata[0] = (uint8_t *)(&offset);
	bdata[1] = (uint8_t *)(&olen);
	bdata[2] = data;
	bdata[3] = data2;
	blen[0] = 2;
	blen[1] = 2;
	blen[2] = olen;
	blen[3] = len2;
	
	for (i = 0; i < 8;i++)
	{
		if (PN & (1 << i))
		{
			recType[i].saveFlag  = 0;
		}
	}
		
	sky_sendNData(recType, cmd, 4, bdata, blen);
	return sky_waitNData(PN,recType,cmd,time);
}

/*
此函数专为一拖四所写 不需要考虑
数据先保存在全局变量中
type最低位表示发送时是否带上数据  第二位表示 是否要接收具体数据把数据拷贝到结构体中
,结构体中send函数指针实现之后才可以使用
*/
int sky_runOffsetCMD(sky_MyByteReceiveDataTypeDef *recType, uint8_t PN,uint8_t cmd,uint8_t **dataAdress, uint16_t offset, uint16_t len,uint8_t type,uint16_t time)
{
	int i,j;
	uint8_t tem = 0;
	
	for (i = 0; i < 8;i++)
	{
		if (PN & (1 << i))
		{
			recType[i].saveFlag = 0;
			if (sky_recOffsetType(recType + i,(uint8_t *)(dataAdress[i]), offset, len, cmd, (type&0x01)) < 0)
			{
				//return -(1 << i);
			}
			sky_Delay_ms(recType, 20);
		}
	}
	
	j=0;
	while (tem != PN)
	{
		for (i = 0; i < 8;i++)
		{
			if (PN & (1 << i))
			{
				if (recType[i].saveFlag == 3)
				{
					
					if (recType[i].data[1] != (cmd|0x10))
					{
//						return -(1 << i);
					}else
					{
						tem |= (1 << i);
					}
				}
			}
		}
		
		j++;
		if(j > time)
		{
			break;
		}
		sky_Delay_ms(recType, 1);
	}
	
	if ((type&0x02))
	{
		for (i = 0; i < 8;i++)
		{
			if (PN & (1 << i))
			{
				memcpy((uint8_t *)(dataAdress[i] + offset) ,&(recType[i].data[10]),*((uint16_t*)(&recType[i].data[8])));
			}
		}		
	}
	
	return -(tem ^ PN);
}


int sky_runNFun(int group, sky_MyByteReceiveDataTypeDef *recType,
					uint8_t cmd,uint8_t *data,uint32_t len,uint32_t time,
					uint8_t *outData,uint32_t outOnelen)	
{
	int i;
	uint8_t tem = 0;
	
	for (i = 0; i < 8;i++)
	{
		if (group & (1 << i))
		{
			
            recType[i].saveFlag = 0;
			if (tem != 0)
			{
				sky_Delay_ms(recType, 20); //防止反馈时间冲突 此时间应根据反馈的数据量来判断
			}
			tem = 1;
			if (sky_sendData(&recType[i], cmd, data, len) < 0) 
            {return -1;}
			
		}
	}
	
	
	if (sky_waitNData(group, recType, cmd, time) < 0){
		return -2;
	}
	
	
	for (i = 0; i < 8;i++)
	{
		if (group & (1 << i))
		{
			if (*((uint32_t*)(&recType[i].data[2])) != (outOnelen+ 4))
			{
				return -3;
			}
			memcpy(outData + i*outOnelen,&(recType[i].data[10]),outOnelen);
		}
	}
	
	return 0;
	
}
/*  常用命令
01 请求并通知        (发个邮箱后反馈)
02 设置并通知        (发个邮箱后反馈)
03 请求 不通知       (直接反馈)
04 设置 不通知       (直接反馈)   
05 设置 通知 并回读  (数据对象要求成对出现，一个读一个写，通过互斥量等，执行完毕后再反馈) 
06 请求 通知 并回读  (数据对象要求成对出现，一个读一个写，通过互斥量等，执行完毕后再反馈) 

21  不定长数据 长度较长 不拷贝的数据
22  不定长数据 发送字符串格式的命令，
23  不定长数据 请求字符串格式的命令,


40  进入IAP命令 查询是连接命令
41  IAP擦除flash命令
42  明文写数据命令
43  重启命令
44  密文写数据
45  通用密文写数据
46  下发版本号，做对比
47  回读硬件版本号
48  回读软件版本号
49  发送HTTp升级指令 0~4 数据长度，4~8起始地址，之后128为要升级程序识别码

61 转发功能
62 特殊板子通用读取功能


虚拟的意思是，以变量代替命令，执行具体的操作，每一条命令都要单独处理,传递的数据(如果有)并不在通信结构体中，需要根据变量来区分，然后动态的来看
81 虚拟请求  
82 虚拟设置
83 虚拟请求 需返回不在结构体中的数据
84 虚拟设置 需发送不在结构体中的数据

A1 ?

C1 调用函数

*/
