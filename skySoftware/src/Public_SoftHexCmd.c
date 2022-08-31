#include "Public_SoftHexCmd.h"
#include <stdarg.h>
#include "stdio.h"	  
#include "string.h"
#include "myFIFO.h"



int P_HexCmdBindFifo(hexCMD_t *userCmd,sky_comDriver* driver,uint8_t *fifoBuf,int fifolen,uint8_t *data,int len,uint8_t **cmd_list,int cmd_list_len)
{
	if(userCmd==NULL||driver==NULL||fifoBuf==NULL||data==NULL||cmd_list==NULL)
		return -1;	
	if(myFIFOInit(&userCmd->fifo,fifoBuf, fifolen)<0)
		return -2;
	userCmd->dev = driver;
	driver->fifo = &userCmd->fifo;		
	userCmd->CMDBuf = data;
	userCmd->CMDBufLen = len;	
	userCmd->CMDList = cmd_list;
	userCmd->CMDListLen = cmd_list_len;
	userCmd->cmdNum = 0;		
	return 0;
}


int P_HexCmdReadCMD(hexCMD_t *userCmd)  
{
	uint8_t res=0; 				
	int k=0;                     
	while(myFIFORead(&userCmd->fifo,&res,1) > 0)
	{
		if (userCmd->cmdNum > (userCmd->CMDBufLen-2))
		{
			userCmd->CMDBuf[userCmd->cmdNum] = 0;
			userCmd->cmdNum = 0;
		}		
		userCmd->CMDBuf[userCmd->cmdNum] = res;  
		userCmd->cmdNum++;				
		for (int row=0;row<userCmd->CMDListLen;row++)  
		{
			k = userCmd->cmdNum - userCmd->CMDList[row][0];  	
			while (k>= 0)	
			{
				for (int list_t=0;list_t<userCmd->CMDList[row][0];list_t++)   
				{					
					if ((userCmd->CMDList[row][list_t+1] & userCmd->CMDList[row][list_t+1+userCmd->CMDList[row][0]]) != \
						(userCmd->CMDBuf[k+list_t] & userCmd->CMDList[row][list_t+1+userCmd->CMDList[row][0]]) )    
					{								
						break;						
					}					
					if (list_t == userCmd->CMDList[row][0]-1)
					{	
						userCmd->cmdNum = 0;
						if(k>0)
						{
							for(int m=0;m<userCmd->CMDList[row][0];m++)
							{
								userCmd->CMDBuf[m]=userCmd->CMDBuf[k+m];
							}
						}						
						return row;
					}				
				}	
				k--;	
			}
		}	
	}	
	return -1;
}


void P_HexCmd_Buf_Clear(hexCMD_t *userCmd)
{
	userCmd->cmdNum = 0;
	memset(userCmd->CMDBuf,0,userCmd->CMDBufLen);
}


void P_HexCmd_Write(hexCMD_t *userCmd,uint8_t *data,int data_len)
{
    devWrite(userCmd->dev, data, data_len);
}
