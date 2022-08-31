#ifndef __NFCINITALISE_H
#define __NFCINITALISE_H

#include "phTalTama.h"
#include "phDalNfc.h"
#include "phHalNfc.h"
#include "commonIOInterface.h"



typedef struct _Tnfc_Initialise
{
	phHal_sHwReference_t    HwRef;
	phHal_sRemoteDevInformation_t   remoteDevInfoList;
	
	MyFIFO_t sendFifo;
	uint8_t sendfifoBuf[512];
	MyFIFO_t recFifo;
	uint8_t recfifoBuf[512];
	
	sky_comDriver *dev;
	
}nfc_Initialise_t; //Çý¶¯Ê¹ÓÃµÄ¾ä±ú£

int nfcInitialiseOpen( nfc_Initialise_t *nit,sky_comDriver *dev);
int nfcInitialiseConnect( nfc_Initialise_t *nit);
int nfc_Transceive(nfc_Initialise_t *nit,uint8_t *pSendBuf,int sendLen,uint8_t *pRecvBuf,int recLen);


#endif
