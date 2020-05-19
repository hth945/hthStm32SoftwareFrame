#include "commonCa310.h"
#include <string.h>
#include "delay.h"
#include "stdlib.h"


int COMCa310InterfaceInit(Ca310Interface *ca310, sky_comDriver *driver)
{
	memset(ca310,0,sizeof(Ca310Interface));
	myFIFOInit(&ca310->ca310FIFO,(void *)ca310->caFIFOBuf,sizeof(ca310->caFIFOBuf));
	ca310->driver = driver;
	ca310->driver->fifo = &ca310->ca310FIFO;
	return 0;
}


/*摘要 发送命令并接受返回
 * 参数    com  要执行的命令 （不带换行符） 
 *         recive  执行命令后返回的数据，长度至少1024
 *         time 此命令最长等待时间  单位 ms
 * 返回值 recive长度  错误返回-1
 */
static int COMRunCommond(Ca310Interface *ca310, char* com,char* rec,u32 time)
{
	char *buf=com;
	if (ca310->driver == 0)
		return -1;
	
	myFIFORead(&ca310->ca310FIFO, (void *)ca310->recive, sizeof(ca310->recive));
	//ca310->driver->write((uint8_t *)buf,strlen(buf));
	Write2dev(ca310->driver, (uint8_t *)buf,strlen(buf));
	
	while(time--)
	{
		if(myFIFORead(&ca310->ca310FIFO, (void *)rec, 1) > 0)
		{
			*rec = (0x7f&(*rec));
			if(*rec==0x0d)
			{
				rec++;
				*rec=0;
				return 1;
			}
			rec++;
		}
		
		Delay_ms(1); 		
	}
	
	strcpy(ca310->recive,"ER:timeOut");
	return -1;
}

static int COMRunCommond_getData(Ca310Interface *ca310, char* rec,u32 time)
{
	if (ca310->driver == 0)
		return -1;
	while(time--)
	{
		if(myFIFORead(&ca310->ca310FIFO, (void *)rec, 1) > 0)
		{
			if(*rec==0x0d)
			{
				rec++;
				*rec=0;
				return 1;
			}
			rec++;
		}
		
		Delay_ms(1); 		
	}
	strcpy(ca310->recive,"ER:timeOut");
	return -1;
}

/********************************************************************************
*函得：CA310_Init
*描述  ：初始化 CA310,
*参数  ：ch   选择哪个内存通道 -1默认 0~99
*		 sync  同步模式  -1：默认  0：NTSC 1：PAL mode 2：EXT mode 3：UNIV mode
*返回  ：正确返回1  负数为错误码 
********************************************************************************/
int COMCA310_Init(Ca310Interface *ca310, int ch,int sync)
{
	char sbuf[10];
//    u32 id;
	if (ca310->driver == 0)
		return -1;
	
	COMRunCommond(ca310, "COM,1\r",ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}//FSC,2  OPR,1
	COMRunCommond(ca310, "FSC,2\r",ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}//通信模式 自动
	
	
	if (ch >= 0)
	{
		sprintf(sbuf,"MCH,%d\r",ch);
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;} 
	}
	if (sync >= 0)
	{
		sprintf(sbuf,"SCS,%d\r",sync);
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;} 
	}
	
	return 1;
}


int COMCA310_QueryPNumber(Ca310Interface *ca310)
{
	uint8_t PNumber = 0;
	uint8_t i;
	if (ca310->driver == 0)
		return -1;
	
	COMRunCommond(ca310, "OPR,0\r",ca310->recive,1000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	COMRunCommond(ca310, "MDS,0\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	i = ca310->recive[6] - 0x31;
	if(i > 4) return -3;
	PNumber |= (1 << i);
	while(1)
	{
		COMRunCommond_getData(ca310, ca310->recive,1000);if(ca310->recive!=strstr(ca310->recive,"OK")) {break;}
		i = ca310->recive[6] - 0x31;
		if(i > 4) return -3;
		PNumber |= (1 << i);
	}
	
	return PNumber;
}


int COMCA310_Cal0(Ca310Interface *ca310)
{
	if (ca310->driver == 0)
		return -1;
	COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "ZRC\r",ca310->recive,5000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	return 1;
}


int COMCA310_GetLvXY(Ca310Interface *ca310, uint8_t PN,float* Lv,float* X,float* Y)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			X[len] = 0;
			Y[len] = 0;
			Lv[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,0\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf)/10000;  //X与Y放大了一万倍传递回来的
	buf=strtok(NULL,";");
	*Y=(float)atof(buf)/10000;
	buf=strtok(NULL,";");
	*Lv=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		X++;
		Y++;
		Lv++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*X=(float)atof(buf)/10000;  //X与Y放大了一万倍传递回来的
		buf=strtok(NULL,";");
		*Y=(float)atof(buf)/10000;
		buf=strtok(NULL,";");
		*Lv=(float)atof(buf);
	}
	return 1;	
}


int COMCA310_GetXYZ(Ca310Interface *ca310, uint8_t PN,float* X,float* Y,float* Z)
{
	char* buf;
	int len = 0;
	char sbuf[10];
	int i;
	if (ca310->driver == 0)
		return -1;
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			X[len] = 0;
			Y[len] = 0;
			Z[len] = 0;
			len++;
		}
	}
		
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,7\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	
	len--;
	buf=strtok(ca310->recive+7,";");
	*X=(float)atof(buf);  //X与Y放大了一万倍传递回来的
	buf=strtok(NULL,";");
	*Y=(float)atof(buf);
	buf=strtok(NULL,";");
	*Z=(float)atof(buf);
		
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		X++;
		Y++;
		Z++;
		len--;
		buf=strtok(ca310->recive+7,";");
		*X=(float)atof(buf);  //X与Y放大了一万倍传递回来的
		buf=strtok(NULL,";");
		*Y=(float)atof(buf);
		buf=strtok(NULL,";");
		*Z=(float)atof(buf);
	}
	return 1;
}


int COMCA310_GetFMA(Ca310Interface *ca310, uint8_t PN,float* FMA)
{
	int len=0;
	char sbuf[10];
	int i;
	
	if (ca310->driver == 0)
		return -1;
	
	
	for (i = 0; i < 5;i++)
	{
		if (PN & (1 << i))
		{
			FMA[len] = 0;
			len++;
		}
	}
	
	if (PN != ca310->PNumberFlag)
	{
		COMRunCommond(ca310, "COM,1\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		sprintf(sbuf,"OPR,");
		for (i = 0; i < 5;i++)
		{
			if (PN & (1 << i))
			{
				sprintf(sbuf+strlen(sbuf),"%d",i+1);
			}
		}
		sprintf(sbuf+strlen(sbuf),"\r");
		COMRunCommond(ca310, sbuf,ca310->recive,3000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
		ca310->PNumberFlag = PN;
	}
	
	COMRunCommond(ca310, "MDS,6\r",ca310->recive,1000); if(ca310->recive!=strstr(ca310->recive,"OK")) { return -1;}
	COMRunCommond(ca310, "MES\r",ca310->recive,3000);  if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
	len--;
	*FMA=(float)atof(ca310->recive+7);
	
	while(len > 0)
	{
		COMRunCommond_getData(ca310, ca310->recive,3000);if(ca310->recive!=strstr(ca310->recive,"OK")) { return -2;}
		FMA++;
		len--;
		*FMA=(float)atof(ca310->recive+7);
	}
	return 1;	
}







