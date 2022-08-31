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
	
	//��Ϊ��Ҫ��֤���յ����������Ķ�λ4�ֽڶ���� ��˽��ջ�������ʼ���� off%4=2
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
*��������receivePact
*����  ����Э�����һ֡���ݣ���������ж��У�ÿ�δ���һ���ֽڣ���У���볬ʱ�ж�50ms
		����һ֡����û�д����꣨��־λû����0�����򲻽��������ݡ�
*����  ��1.myRecType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.newc       ��ʱ���յ���һ���ֽ�
*����  ������ͨ��״̬��־  0 ���ڵȴ�  
						1 ���ڽ�������ͷ����ȡ���ݳ���  
						2 ����У������  
						3 ���յ�һ֡����������
********************************************************************************/
int sky_receivePact(sky_MyByteReceiveDataTypeDef *myRecType,uint8_t newc)
{
	int i=0;
	uint8_t num=0;
	uint32_t T;
	
	/************************��ʱ�ж�************************/
	T = sky_GetMeasureTime_ms(myRecType, myRecType->oldTime);  
	myRecType->oldTime = myRecType->getTime();
	
	if((T > 50)&&(myRecType->saveFlag !=3))  //��ʱ ����û�н������ ��ô���¿�ʼ
		myRecType->saveFlag = 0;
	
	/*********************��Э���������**********************************/
	if (myRecType->saveFlag == 0) //�ȴ����յ���ʼ�ַ�
	{
		if (newc == myRecType->startc)  //���յ���ʼ�ַ�
		{
			myRecType->saveFlag=1;  //��־��1 ������һ��
			myRecType->saveNum=1;
			myRecType->data[0] = newc; 
		}
		
	}else if (myRecType->saveFlag == 1) //���ڽ�������ͷ����ȡ���ݳ��� 
	{
		myRecType->data[myRecType->saveNum] = newc; 
		myRecType->saveNum++;	
		
		if(myRecType->saveNum >= 6)  //���յ������ݳ��ȴ���6  ���ݳ����Ѵ��͹���
		{
			myRecType->dataLen = *((uint32_t *)(myRecType->data+2));
			
			if(myRecType->dataLen > myRecType->maxlen - 10)  //�ж����ݳ��ȿ쵽��������󳤶�  
				myRecType->saveFlag = 0;       //��� ���¿�ʼ����
			else
				myRecType->saveFlag = 2;      //���Ⱥϸ� ������һ�� ��������
		}
		
	}else if(myRecType->saveFlag == 2)  //�ȴ������������
	{
        myRecType->data[myRecType->saveNum] = newc; 
		myRecType->saveNum++;	
        
		if(myRecType->saveNum >= myRecType->dataLen + 8)  //���ݽ������ ����У��
		{
			for(num =0,i=1;i<myRecType->dataLen + 6;i++)  //У������
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


//�ӽ��յ��������� ��ȡ���� ��ȫ�ֱ�����
int sky_extractData(sky_MyByteReceiveDataTypeDef *recType,uint8_t *data)
{
	uint16_t offest;
	uint16_t len;
	
	if (recType->saveFlag == 3)    //�ȴ��������
	{
		offest = *((uint16_t*)(&recType->data[6]));
		len =  *((uint16_t*)(&recType->data[8]));
		memcpy(((uint8_t *)data) + offest, (uint8_t *)(&recType->data[10]), len);
		return 0;
	}
	return -1;
}

//�ȴ����ݷ��� 
int sky_waitData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,int time)
{
	uint32_t t = recType->getTime();
	
	while(1)
	{
		if(recType->saveFlag == 3)    //�ȴ��������
		{
			if(*(recType->data+1) == (cmd|0x10))  //���ص��������У��  �����ն�ִ�гɹ� ��ѵ���λ��1
			{
				return 1;
			}else
			{
				recType->saveFlag  = 0;
				return -2;            //ִ�д��� ���ص��Ǵ��������
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

//�ȴ����ݷ��� 
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
 * ������ : pcui_sendData
 * ��  �� : pcui��������
 * ��  �� : *recType:ͨѶЭ��ṹ��
 * ��  �� : cmd:ָ��
 * ��  �� : **str:�ַ���ָ��
 * ��  �� : strLen:�ַ�������
 * ��  �� : >=0:���ͳɹ�
            -1:����ʧ��
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
*��������sendData
*����  ����Э�鷢��һ֡����,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
*����  ��1.recType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.cmd       �������ݰ�������
		 3.data     ���͵�����
		 4.len       ���͵����ݳ���
*����  ������ ���͵����ݳ���  -1���ʹ���
********************************************************************************/
int sky_sendData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data,uint32_t len)
{
	uint8_t sbuf[10];
	int i;
	
	sbuf[0] = recType->startc;   //һ�ֽ���ʼ�ַ�
	sbuf[1] = cmd;				 //һ�ֽ�����	
	*((uint32_t*)(&sbuf[2])) = len;   //���ֽڳ���
	
	if (devWrite(recType->dev, sbuf,6) < 0)
		return -1;

	sbuf[6] = 0;                //У���ʼ��Ϊ0
	sbuf[7] = recType->endc;    //�����ַ�
	
	for( i = 1; i < 6;i++)      //У��ͷ
		sbuf[6] ^= sbuf[i];
	
	for (i = 0; i < len; i++)   //У�����ݶ�
		sbuf[6] ^= data[i];
	
	if (devWrite(recType->dev, data,len) < 0) return -1;   //�������ݶ�
	if (devWrite(recType->dev, sbuf + 6,2) < 0) return -1; //���ͽ�β
	return len + 8;
}

/********************************************************************************
*��������send2Data
*����  ����Э�鷢��һ֡����,���ݶη�Ϊ����,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
*����  ��1.recType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.cmd       �������ݰ�������
		 3.data1     ���͵ĵ�һ������
		 4.len1       ���͵ĵ�һ�����ݳ���
		 5.data2     ���͵ĵڶ�������
		 6.len2       ���͵ĵڶ������ݳ���
*����  ������ ���͵����ݳ���  -1���ʹ���
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
*��������recOffsetType
*����  ����Э�鷢��һ֡����,���ݶη����ֽ�ƫ�ƣ����ֽڳ��ȣ������θ���tpye�ж�����������
		������Ϊ �������� ���Ƿ�������,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
*ע��  ���˺���ֻ����  ���յ���������ô�����볬ʱ ���ⲿ����
*����  ��1.recType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.data      ���ݵĻ���ַ
		 4.offset    Ҫ�������ݵ�ƫ��
		 5.len       Ҫ�������ݵĳ���
		 5.cmd       �������ݰ�������
		 6.type      �Ƿ��;��������  1����  0������
*����  ������ ���͵����ݳ���  -1���ʹ���
********************************************************************************/
int sky_recOffsetType(sky_MyByteReceiveDataTypeDef *recType,uint8_t *data, uint16_t offset, uint16_t len,uint8_t cmd, uint8_t type)
{
	uint8_t sbuf[12];
	int j,i;
	
	sbuf[0] = recType->startc;
	sbuf[1] = cmd;
	if ((type&0x01) == 1)
	{
		*((uint32_t*)(&sbuf[2])) = 4 + len;  //typeΪ1ʱ ���ݳ���Ϊ len+2+2  (�����غ� + ƫ�� + ���ȵ�ֵ)
	}
	else
	{
		*((uint32_t*)(&sbuf[2])) = 4;        //typeΪ0ʱ ���ݳ���Ϊ 2+2  ��ƫ�� + ���ȵ�ֵ)
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
*��������runCmdPact
*����  ������һ��������ȴ�����,���������ڽṹ����,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
*ע��  ���������к������ڽṹ���У���������һֱ���գ���������0 �ṹ���еı�־
*����  ��1.recType  ͨ�ŵ������Ľṹ�壬�ڲ��л�������ͨ��״̬��־����Ϣ
		 2.cmd       �������ݰ�������
		 3.data     ���͵�����
		 4.len       ���͵����ݳ���
		 5.time     ��ʱʱ��
*����  ��1 ִ�гɹ�  -1 ��ʱ   -2 ִ�д���
********************************************************************************/
int sky_runCmdPact(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,uint8_t *data,uint32_t len,uint32_t time)	
{
	recType->saveFlag  = 0;         //������ձ�־ �򿪽���
	sky_sendData(recType,cmd,data,len); //��������
	return sky_waitData(recType,cmd,time);
}


int sky_runCmdType(sky_MyByteReceiveDataTypeDef *recType, uint8_t *data, uint16_t offset, uint16_t len,uint8_t cmd, uint8_t type, int time)
{
	recType->saveFlag  = 0;         //������ձ�־ �򿪽���
	sky_recOffsetType(recType,data,offset,len,cmd,type); //��������
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
�˺���רΪһ������д ����Ҫ����
�����ȱ�����ȫ�ֱ�����
type���λ��ʾ����ʱ�Ƿ��������  �ڶ�λ��ʾ �Ƿ�Ҫ���վ������ݰ����ݿ������ṹ����
,�ṹ����send����ָ��ʵ��֮��ſ���ʹ��
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
				sky_Delay_ms(recType, 20); //��ֹ����ʱ���ͻ ��ʱ��Ӧ���ݷ��������������ж�
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
/*  ��������
01 ����֪ͨ        (�����������)
02 ���ò�֪ͨ        (�����������)
03 ���� ��֪ͨ       (ֱ�ӷ���)
04 ���� ��֪ͨ       (ֱ�ӷ���)   
05 ���� ֪ͨ ���ض�  (���ݶ���Ҫ��ɶԳ��֣�һ����һ��д��ͨ���������ȣ�ִ����Ϻ��ٷ���) 
06 ���� ֪ͨ ���ض�  (���ݶ���Ҫ��ɶԳ��֣�һ����һ��д��ͨ���������ȣ�ִ����Ϻ��ٷ���) 

21  ���������� ���Ƚϳ� ������������
22  ���������� �����ַ�����ʽ�����
23  ���������� �����ַ�����ʽ������,


40  ����IAP���� ��ѯ����������
41  IAP����flash����
42  ����д��������
43  ��������
44  ����д����
45  ͨ������д����
46  �·��汾�ţ����Ա�
47  �ض�Ӳ���汾��
48  �ض�����汾��
49  ����HTTp����ָ�� 0~4 ���ݳ��ȣ�4~8��ʼ��ַ��֮��128ΪҪ��������ʶ����

61 ת������
62 �������ͨ�ö�ȡ����


�������˼�ǣ��Ա����������ִ�о���Ĳ�����ÿһ�����Ҫ��������,���ݵ�����(�����)������ͨ�Žṹ���У���Ҫ���ݱ��������֣�Ȼ��̬������
81 ��������  
82 ��������
83 �������� �践�ز��ڽṹ���е�����
84 �������� �跢�Ͳ��ڽṹ���е�����

A1 ?

C1 ���ú���

*/
