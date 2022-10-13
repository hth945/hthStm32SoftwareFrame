#include "nfcITarget.h"

#include "string.h"

#include <rtthread.h>
#include "mypwm.h"
#include "stmflash.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"	
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <string.h>
#include "systick.h"
#include "delay.h"
#include <stdio.h>
#include "bsp_usart1.h"	  





//int nfc_send(nfc_Target_t *nit,uint8_t *pSendBuf,int sendLen)

//int nfc_recve(nfc_Target_t *nit,uint8_t *pRecvBuf,int recLen)
	



int nfcTargetOpen( nfc_Target_t *nft,sky_comDriver *dev)
{
	NFCSTATUS  status;
	
	nft->dev = dev;
	//phHal_sDeviceCapabilities_t sDevCapabilities;
	memset(&nft->HwRef, 0, sizeof(phHal_sHwReference_t));
	nft->HwRef.pBoardDriver = nft->dev;
	
	myFIFOInit(&nft->recFifo,nft->recfifoBuf, sizeof(nft->recfifoBuf));
	myFIFOInit(&nft->sendFifo,nft->sendfifoBuf, sizeof(nft->sendfifoBuf));
	
	
	nft->HwRef.UserTimeout  = 500;
	nft->HwRef.Flags.AbortAllowed = 0;
	nft->HwRef.CompType = CID_NFC_TAL;
	nft->HwRef.Flags.LinkValid = 0;
	nft->HwRef.LinkType = 1;
	
	
	status = phHalNfc_Open(&nft->HwRef);
	
	if (status != 0 )
		return -1;
	
	return 0;
}


int nfcTargetConnect( nfc_Target_t *nft)
{
	NFCSTATUS  status;
	
	phHal_sTargetInfo_t     communicationparams;
	phHal_eOpModes_t        PollModes[6];
	uint8_t                 ConnectionReq[64];
    uint8_t                 ConnectionReqBufLength;
	
	memset(&communicationparams, 0, sizeof(communicationparams));
	memset(&PollModes, 0, sizeof(PollModes));
	memset(&ConnectionReq, 0, sizeof(ConnectionReq));
	
	
	int i = 0;
	PollModes[i] = phHal_eOpModesNfcPassive106;
    i++;
    PollModes[i] = phHal_eOpModesNfcPassive212;
    i++;
    PollModes[i] = phHal_eOpModesNfcPassive424;
    i++;
    PollModes[i] = phHal_eOpModesNfcActive106;
    i++;
    PollModes[i] = phHal_eOpModesNfcActive212;
    i++;
    PollModes[i] = phHal_eOpModesNfcActive424;
    i++;
    PollModes[i] = phHal_eOpModesArrayTerminator;
	
	communicationparams.RandomNFCIDt = 0;
    communicationparams.Options.AutoConnectionResponse = 1;
    communicationparams.Options.PassiveOnly = 0;
    communicationparams.SensRes[0] = 0x04;
    communicationparams.SensRes[1] = 0x03;

    communicationparams.SelRes = 0x40; 

    // set NFCID 1
    for (i = 0; i < 10; i++)
    {
        communicationparams.NFCID1t[i] = (uint8_t)(i + 1);
    }

    //// set NFCID 2
    communicationparams.NFCID2t[0] = 0x01;
    communicationparams.NFCID2t[1] = 0xFE;
    for (i = 2; i < 8; i++)
    {
        communicationparams.NFCID2t[i] = (uint8_t)(i + 0xA0);
    }

    // set NFCID 3
    for (i = 0; i < 10; i++)
    {
        communicationparams.NFCID3t[i] = (uint8_t)(i + 1);
    }

    // set PAD Information
    for (i = 0; i < 8; i++)
    {
        communicationparams.PAD[i] = (uint8_t)(0xAA - i);
    }

    communicationparams.SystemCode[0] = 0xFF;
    communicationparams.SystemCode[1] = 0xFF;

    // remark : The maximum possible length is defined in ISO18092 standard
    communicationparams.GeneralBytesLength = 10;

    communicationparams.GeneralBytes[0] = 's';
    communicationparams.GeneralBytes[1] = 'k';
    communicationparams.GeneralBytes[2] = 'y';
    communicationparams.GeneralBytes[3] = 'c';

    communicationparams.GeneralBytes[4] = 'o';
    communicationparams.GeneralBytes[5] = 'd';
    communicationparams.GeneralBytes[6] = 'e';
    communicationparams.GeneralBytes[7] = 'r';

    communicationparams.GeneralBytes[8] = 'e';
    communicationparams.GeneralBytes[9] = 'c';



	
	
	status = phHalNfc_StartTargetMode(&nft->HwRef,
                &(communicationparams),
                PollModes,
                ConnectionReq,
                &(ConnectionReqBufLength));
				
				
	if (status != 0 )
		return -1;			
	return 0;
}


int nfc_send(nfc_Target_t *nit,uint8_t *pSendBuf,int sendLen)
{
	NFCSTATUS  status;
	
	status = phHalNfc_Send(&nit->HwRef,
                &(nit->DepMoreInfo),
                pSendBuf,
                sendLen);
	
	return 0;
}


int nfc_recve(nfc_Target_t *nit,uint8_t *pRecvBuf,int recLen)
{
	NFCSTATUS  status;
	uint16_t RcvLength = recLen;
	
	status = phHalNfc_Receive(&nit->HwRef,
                &(nit->DepMoreInfo),
                pRecvBuf,
                &(RcvLength));
	
	if (status != 0)
		return -1;
	
	return RcvLength;
}
