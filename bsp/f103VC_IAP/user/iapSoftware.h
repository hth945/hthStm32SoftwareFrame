#ifndef __IAPSoftware_H__
#define __IAPSoftware_H__
#include "sys.h"  
#include "communication.h"
#include "commonIOInterface.h"

typedef struct{
	int iapFlag;
	u32 appxaddr;
	int timeOut;  
	int startTime;
	char version[128];
    sky_MyByteReceiveDataTypeDef iapRec;
	char iapRecData[4096+512];
}iap_adapter;

void iap_Init(iap_adapter *iapAda,sky_comDriver *driver, int timeOut,u32 appxaddr, char *version);
int iapUpRun(iap_adapter *iapAda);
void iap_load_app(iap_adapter *iapAda);

#endif
