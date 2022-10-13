#include "nfcInitialise.h"

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


int nfcInitialiseOpen( nfc_Initialise_t *nit,sky_comDriver *dev)
{
	NFCSTATUS  status;
	
	nit->dev = dev;
	//phHal_sDeviceCapabilities_t sDevCapabilities;
	memset(&nit->HwRef, 0, sizeof(phHal_sHwReference_t));
	nit->HwRef.pBoardDriver = nit->dev;
	
	myFIFOInit(&nit->recFifo,nit->recfifoBuf, sizeof(nit->recfifoBuf));
	myFIFOInit(&nit->sendFifo,nit->sendfifoBuf, sizeof(nit->sendfifoBuf));
	
	nit->HwRef.UserTimeout  = 500;
	nit->HwRef.Flags.AbortAllowed = 0;
	nit->HwRef.CompType = CID_NFC_TAL;
	nit->HwRef.Flags.LinkValid = 0;
	nit->HwRef.LinkType = 1;
	
	
	status = phHalNfc_Open(&nit->HwRef);
	
	if (status != 0 )
		return -1;
	
	return 0;
}


int nfcInitialiseConnect( nfc_Initialise_t *nit)
{
	NFCSTATUS  status;
	
	phHal_eOpModes_t        PollModes[6];
	//phHal_sRemoteDevInformation_t   remoteDevInfoList[1];
	uint8_t                 nbRemoteDev;
	phHal_sDevInputParam_t  communicationparams[1];
	
	
	phHal_eOpModes_t currentOpMode;
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
	
	
	communicationparams[0].DIDiUsed = 0;
    communicationparams[0].NfcNADiUsed = 0;
    communicationparams[0].NFCIDAuto = 1;

    communicationparams[0].GeneralByteLength = 7;

    communicationparams[0].GeneralByte[0] = 's';
    communicationparams[0].GeneralByte[1] = 'k';
    communicationparams[0].GeneralByte[2] = 'y';

    communicationparams[0].GeneralByte[3] = 'c';
    communicationparams[0].GeneralByte[4] = 'o';
    communicationparams[0].GeneralByte[5] = 'd';
    communicationparams[0].GeneralByte[6] = 'e';

    // needed for 212 / 424kbps activation passive
    communicationparams[0].NfcPollPayload[0] = 0x00;
    communicationparams[0].NfcPollPayload[1] = 0xFF;
    communicationparams[0].NfcPollPayload[2] = 0xFF;
    communicationparams[0].NfcPollPayload[3] = 0x00;
    communicationparams[0].NfcPollPayload[4] = 0x01;
	
	
	
	
	memset(&nit->remoteDevInfoList, 0, sizeof(phHal_sRemoteDevInformation_t));
	nbRemoteDev = 1;
	
	
	status = phHalNfc_Poll(&nit->HwRef,
            PollModes,
            &nit->remoteDevInfoList,
            &nbRemoteDev,
            communicationparams);
	
	if (status != 0 )
		return -1;
	
	
	rt_kprintf("hLogHandle : %d\n", nit->remoteDevInfoList.hLogHandle);
	rt_kprintf("Session Opened : %d\n", nit->remoteDevInfoList.SessionOpened + 1);
	
	currentOpMode = nit->remoteDevInfoList.OpMode;
	
	status = phHalNfc_Connect(&nit->HwRef,
                    currentOpMode,
                    &nit->remoteDevInfoList,
                    &(communicationparams[0]));
	
	if (status != 0 )
		return -2;
	
	return 0;
}


int nfc_Transceive(nfc_Initialise_t *nit,uint8_t *pSendBuf,int sendLen,uint8_t *pRecvBuf,int recLen)
{
	NFCSTATUS  status;
	
	phHal_uCmdList_t        DEPcmd;
	phHal_sDepAdditionalInfo_t DepMoreInfo;
	uint16_t RcvLength = recLen;
	
	memset(&DepMoreInfo, 0, sizeof(DepMoreInfo));
	memset(&DEPcmd, 0, sizeof(DEPcmd));
	
	DEPcmd.NfcInCmd = phHal_eNfcCmdListNfcDEP;
	
	
	status = phHalNfc_Transceive(&nit->HwRef,
            &nit->remoteDevInfoList,
            DEPcmd,
            &(DepMoreInfo),
            pSendBuf,
            sendLen,
            pRecvBuf,
            &(RcvLength));
	
	if (status != 0)
		return -1;
	
	return RcvLength;
	
}
