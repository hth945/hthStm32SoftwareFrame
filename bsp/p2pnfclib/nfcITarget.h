#ifndef __NFCTARGET_H
#define __NFCTARGET_H


#include "phTalTama.h"
#include "phDalNfc.h"
#include "phHalNfc.h"
#include "commonIOInterface.h"


typedef struct _Tnfc_Target
{
	phHal_sHwReference_t    HwRef;
	phHal_sDepAdditionalInfo_t  DepMoreInfo;
	
	MyFIFO_t sendFifo;
	uint8_t sendfifoBuf[512];
	MyFIFO_t recFifo;
	uint8_t recfifoBuf[512];
	
	sky_comDriver *dev;
	
}nfc_Target_t; 

int nfcTargetOpen( nfc_Target_t *nft,sky_comDriver *dev);
int nfcTargetConnect( nfc_Target_t *nit);
int nfc_send(nfc_Target_t *nit,uint8_t *pSendBuf,int sendLen);
int nfc_recve(nfc_Target_t *nit,uint8_t *pRecvBuf,int recLen);



#endif