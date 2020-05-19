#include "Public_SoftStringCmd.h"
#include <stdarg.h>
#include "stdio.h"	  
#include "string.h"
#include "myFIFO.h"


int P_StringCmdBindFifo(stringCMD_t *scmd,sky_comDriver* driver, uint8_t *fifoBuf, int fifolen, uint8_t *data, int len)
{
	memset(scmd,0,sizeof(stringCMD_t));
	
	myFIFOInit(&scmd->fifo,fifoBuf, fifolen);
	driver->fifo = &scmd->fifo;
	
	scmd->CMDBuf = data;
	scmd->CMDBufLen = len;

	return 0;
}



int P_StringCmdReadCMD(stringCMD_t *scmd)
{
	uint8_t res;
	
	while (myFIFORead(&scmd->fifo,&res,1) > 0)
	{
		if (scmd->cmdNum > (scmd->CMDBufLen-1))
		{
			scmd->CMDBuf[scmd->cmdNum] = 0;
			scmd->cmdNum = 0;
		}
		
		if (res == '\n')
		{
			scmd->CMDBuf[scmd->cmdNum] = 0;
			scmd->cmdNum = 0;
			return 1;
		}else if (res != '\r')
		{
			scmd->CMDBuf[scmd->cmdNum] = res;
			scmd->cmdNum++;
		}
	}
	return -1;
}


int P_StringCmdScan(stringCMD_t *userCmd,char *fmt, ...)
{
	va_list argptr; 
	int cnt; 
	int i = 0;
	char *p = fmt;
	
	while(1)
	{
		if (*p == '%')
		{
			i++;
		}else if (*p == 0)
		{
			break;
		}
		p++;
	}
	
	if (i != 0)
	{
		va_start(argptr, fmt); 
		cnt = vsscanf((char *)userCmd->CMDBuf, fmt, argptr); 
		va_end(argptr); 
		if (i == cnt)
			return 0;
		else
			return -1;
	}else
	{
		if (strcmp((char *)userCmd->CMDBuf, fmt) == 0)
			return 1;
		else
			return -1;
	}
}

