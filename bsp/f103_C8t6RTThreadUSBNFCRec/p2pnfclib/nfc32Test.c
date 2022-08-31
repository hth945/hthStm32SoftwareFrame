#include "phTalTama.h"
#include "phDalNfc.h"
#include "phHalNfc.h"
#include "string.h"
#include "commonIOInterface.h"

#include <string.h>
#include "systick.h"
#include "delay.h"
#include <stdio.h>
#include "bsp_usart1.h"	  
#include "bsp_usart5.h"	  
#include "Public_SoftStringCmd.h"
#include "communication.h"
#include <rtthread.h>
#include "mypwm.h"
#include "stmflash.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"	
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <phDalNfc.h>


#define MAX_BOARD_NUMBER                1

typedef struct phcsExampleInitParam
{
    phHal_sHwReference_t    HwRef[MAX_BOARD_NUMBER];
    uint8_t                 NbrOfDevDetected;
    uint8_t                 index;
    uint32_t                MTU;
    uint8_t                 MultiPoll;
    NFCSTATUS               status;

} phcsExampleInitParam_t;

void ProcessEnumerateResult(phcsExampleInitParam_t* InitParams)
{
    uint8_t n, c;

    rt_kprintf("\nAmount of boards detected : %d\n\n", InitParams->NbrOfDevDetected);
    if (InitParams->status == NFCSTATUS_SUCCESS)
    {
        for (n = 0; n < (InitParams->NbrOfDevDetected); n++)
        {
            rt_kprintf("\n* %d ", n + 1);
            if (((InitParams->HwRef)[n].LinkType) == 1)          // COM Port Type
            {
                rt_kprintf(" Link: %.4s", (InitParams->HwRef)[n].DalContext.sLinkName);
                //rt_kprintf("      Type: %s \n", (InitParams->HwRef)[n].ChipName);
            }
            else
                rt_kprintf(" Link: Unknown\n");
            
        }
        rt_kprintf("   --> default choice\n");
    }

    if (((InitParams->status) == NFCSTATUS_SUCCESS) && (InitParams->NbrOfDevDetected) > 1)
    {
        InitParams->index = InitParams->NbrOfDevDetected - 1;
        InitParams->index = 0;
    }
}


void Initialise_NFC_Hardware(phcsExampleInitParam_t* InitParams)
{
    // Instantiate struct to get more HW information
    phHal_sDeviceCapabilities_t sDevCapabilities;

    InitParams->index = 0;
	InitParams->HwRef[0].UserTimeout  = 15000;
	InitParams->HwRef[0].Flags.AbortAllowed = 0;
	InitParams->HwRef[0].CompType = CID_NFC_TAL;
	
	
	
	
	phHal_sHwReference_t *pHwRef = &InitParams->HwRef[0];
	phDal_Context_t * pDalContext =  &(InitParams->HwRef[0].DalContext);

	
	pHwRef->Flags.LinkValid = 0;
	memset( pDalContext->sLinkName, 0, sizeof(pDalContext->sLinkName));
	strcpy(pDalContext->sLinkName, "COM0");
	pDalContext->AbortEvent     =   NULL;
	InitParams->HwRef[0].Sleeping =   1;
	InitParams->HwRef[0].ChipRevision =   0;  /*  Will be filled later on within the open function    */
	InitParams->HwRef[0].ChipName     =   phHal_eChipName53x;
	pHwRef->CompType = CID_NFC_TAL;
	pHwRef->DeviceNbr = 0;
	pHwRef->LinkType = PHHAL_SERIAL_DEVICE;
					
					
					
					
//    // Enumerate: Find out which hardware is connected to the PC
//    (InitParams->status) = phHalNfc_Enumerate(InitParams->HwRef, &(InitParams->NbrOfDevDetected));

//    // User interaction to decide which hardware shall be opened
//    ProcessEnumerateResult(InitParams);

    // Open Board according to user input
    (InitParams->status) = phHalNfc_Open(&(InitParams->HwRef[InitParams->index]));

    // Error handling
    if (InitParams->status == NFCSTATUS_SUCCESS)
        rt_kprintf("\nOpen SUCCESSFUL \n");
    else
    {
        rt_kprintf("\nOpen NOT SUCCESSFUL - Error code: %d \n\n !!! Please ENUMERATE AGAIN !!!\n\n", InitParams->status);
        //phOsalNfc_Suspend(3500);
        return;
    }

    // Get the maximum transfer unit size (MTU) to control data exchange
    InitParams->status = phHalNfc_GetDeviceCapabilities(&(InitParams->HwRef[InitParams->index]), &sDevCapabilities);
    InitParams->MTU = (sDevCapabilities.MTU);
    InitParams->MultiPoll = (sDevCapabilities.MultiPollSessionSup);

    // Error handling
    if (InitParams->status == NFCSTATUS_SUCCESS)
        rt_kprintf("Get Device Capabilities SUCCESSFUL \n");
    else
    {
        rt_kprintf("\nGet Device Capabilities NOT SUCCESSFUL - Error code: %d \n", InitParams->status);
        return;
    }

}


#define DATABUFFERSIZE      (uint16_t)  4000
// Defines the send and receive buffer for the reader/writer data exchange
#define RWBUFFERSIZE                    4000
// The maximum number of modes which are polled for
#define NB_OP_MODE_MAX                  6

#define NUMBER_GENERAL_BYTES            12

typedef struct phcsExampleTargetParam
{
    // Hardware information of the local NFC Target
    phHal_sHwReference_t* HwRef;
    // Data buffer to store receive and response (send) data
    uint8_t                 buffer[DATABUFFERSIZE];
    // Error handling
    NFCSTATUS               status;

    //------------------
    // START TARGET MODE
    //------------------

    // Data used by the target to establish the P2P communication
    // NFCIDs, General Bytes...
    phHal_sTargetInfo_t     communicationparams;
    // Table of modes which will be accepted during remote device poll
    phHal_eOpModes_t        PollModes[NB_OP_MODE_MAX];
    // Activation data from initiator
    uint8_t                 ConnectionReq[64];
    // Length of activation data from initiator
    uint8_t                 ConnectionReqBufLength;
    //// marker for DiD used
    //uint8_t                 DiDUsed; 

    //--------------
    // DATA EXCHANGE
    //--------------

    // DEP_RES command
    phHal_uCmdList_t        DEPcmd;
    // how many data (payload) bytes shall be sent to the initiator
    uint16_t                SendLength;
    // to keep in mind how many data bytes are still open
    // to be transmitted when using meta chaining
    uint16_t                OpenChainingData;
    // how many bytes were received as initiaor request
    uint16_t                RcvLength;
    // is meta chaining needed to fulfill SendLength settings ?
    uint8_t                 MetaChaining;
    // keep track on number of meta chaining operations
    uint16_t                MetaChainingCounter;
    // more information for DEP_REQ e.g. the MI bit is set inhere
    phHal_sDepAdditionalInfo_t  DepMoreInfo;
    // counter to keep track on the iteration status which is set b
    // by the user inside p2pDataFlowParams_t
    uint16_t                iterations;
    // keep track on maximum amount of data without using meta chaining
    uint32_t                MTU;
    // General Bytes
    uint8_t                 GeneralBytes[NUMBER_GENERAL_BYTES];
    //quit DEP exchange
    uint8_t                 QuitDEP;
    //from which index of the buffer onwards data is send
    uint16_t                index;


} phcsExampleTargetParam_t;


void Init_Target_Parameters(phcsExampleTargetParam_t* TParams)
{
    uint16_t i = 0;

    // set the major structs to zero
    memset(&TParams->communicationparams, 0, sizeof(TParams->communicationparams));
    memset(&TParams->DepMoreInfo, 0, sizeof(TParams->DepMoreInfo));
    memset(&TParams->DEPcmd, 0, sizeof(TParams->DEPcmd));
    memset(&TParams->GeneralBytes, 0, sizeof(TParams->GeneralBytes));
    memset(&TParams->PollModes, 0, sizeof(TParams->PollModes));
    memset(&TParams->ConnectionReq, 0, sizeof(TParams->ConnectionReq));
    memset(&TParams->buffer, 0, sizeof(TParams->buffer));

    TParams->status = NFCSTATUS_SUCCESS;
    TParams->ConnectionReqBufLength = 48;

    // DEP Request and Response steering data
    TParams->SendLength = 0;
    TParams->OpenChainingData = 0;
    TParams->RcvLength = 0;        //DATABUFFERSIZE;
    TParams->MetaChaining = 0;
    TParams->MetaChainingCounter = 0;
    TParams->iterations = 0;
    TParams->QuitDEP = 0;
    TParams->index = 0;
    TParams->DepMoreInfo.DepFlags.NADPresent = 0;

    TParams->PollModes[0] = phHal_eOpModesArrayTerminator;

    for (i = 0; i < NUMBER_GENERAL_BYTES; i++)
    {
        TParams->communicationparams.GeneralBytes[i] = (0xB0 + (uint8_t)i);
    }

    //init the response / send buffer with some data until end of buffer size
    for (i = 0; i < DATABUFFERSIZE; i++)
    {
        TParams->buffer[i] = i % 255;                     //fill with hex numbers 0x00 -> 0xFE
        //TParams->buffer[i] = ((i%26) + 97);                   //for the alphabeth 'a' till 'z'
        //TParams->buffer[i] = i%26 + 97 - ('a' -'A');          //for the alphabeth 'A' till 'Z'
    }
}

void SetTargetCommunicationParams(phcsExampleTargetParam_t* TParams)
{
    // initialize the communication parameters
    uint8_t i = 0;

    // set general target flow values
    TParams->communicationparams.RandomNFCIDt = 0;
    TParams->communicationparams.Options.AutoConnectionResponse = 1;
    TParams->communicationparams.Options.PassiveOnly = 0;

    // Enter Sense response data
    TParams->communicationparams.SensRes[0] = 0x04;
    TParams->communicationparams.SensRes[1] = 0x03;

    TParams->communicationparams.SelRes = 0x40; // bit 6 must be set to 1 (NFCIP1 Data exchange protocol compliance)

    // set NFCID 1
    for (i = 0; i < 10; i++)
    {
        TParams->communicationparams.NFCID1t[i] = (uint8_t)(i + 1);
    }

    //// set NFCID 2
    TParams->communicationparams.NFCID2t[0] = 0x01;
    TParams->communicationparams.NFCID2t[1] = 0xFE;
    for (i = 2; i < 8; i++)
    {
        TParams->communicationparams.NFCID2t[i] = (uint8_t)(i + 0xA0);
    }

    // set NFCID 3
    for (i = 0; i < 10; i++)
    {
        TParams->communicationparams.NFCID3t[i] = (uint8_t)(i + 1);
    }

    // set PAD Information
    for (i = 0; i < 8; i++)
    {
        TParams->communicationparams.PAD[i] = (uint8_t)(0xAA - i);
    }

    TParams->communicationparams.SystemCode[0] = 0xFF;
    TParams->communicationparams.SystemCode[1] = 0xFF;

    // remark : The maximum possible length is defined in ISO18092 standard
    TParams->communicationparams.GeneralBytesLength = NUMBER_GENERAL_BYTES;

    TParams->communicationparams.GeneralBytes[0] = 'N';
    TParams->communicationparams.GeneralBytes[1] = 'F';
    TParams->communicationparams.GeneralBytes[2] = 'C';
    TParams->communicationparams.GeneralBytes[3] = ' ';

    TParams->communicationparams.GeneralBytes[4] = 'f';
    TParams->communicationparams.GeneralBytes[5] = 'r';
    TParams->communicationparams.GeneralBytes[6] = 'o';
    TParams->communicationparams.GeneralBytes[7] = 'm';

    TParams->communicationparams.GeneralBytes[8] = ' ';
    TParams->communicationparams.GeneralBytes[9] = 'N';
    TParams->communicationparams.GeneralBytes[10] = 'X';
    TParams->communicationparams.GeneralBytes[11] = 'P';

    // set the modes the target shall respond to
    TParams->PollModes[0] = phHal_eOpModesArrayTerminator;
}

void SelectTargetPollingMode(phcsExampleTargetParam_t* IParam)
{
    // set all modes to be able to connect to them
    IParam->PollModes[0] = phHal_eOpModesNfcPassive106;
    IParam->PollModes[1] = phHal_eOpModesNfcPassive212;
    IParam->PollModes[2] = phHal_eOpModesNfcPassive424;
    IParam->PollModes[3] = phHal_eOpModesNfcActive106;
    IParam->PollModes[4] = phHal_eOpModesNfcActive212;
    IParam->PollModes[5] = phHal_eOpModesNfcActive424;
    IParam->PollModes[6] = phHal_eOpModesArrayTerminator;

    return;
}

void ProcessTargetModeData(uint8_t* ConnectionDataBuffer, uint8_t ConnectionDataBufferLength)
{
    uint8_t i = 0;
    rt_kprintf("\n---------------------------------\n*** NFC Protocol ***\n");

    // Display the total length of ATR_REQ
    rt_kprintf("Length of ATR_REQ: %02x\n", ConnectionDataBuffer[0]);
    // Index 1 and 2 of ATR_REQ indicate the CMD0 and CMD1
    // Index 3 till 13 shows the NFCID3
    rt_kprintf("NFCID3i: ");
    for (i = 3; i <= 13; i++)
    {
        rt_kprintf("%02X ", ConnectionDataBuffer[i]);
    }
    // DiD used?
    rt_kprintf("\nDiDi: %02x\n", ConnectionDataBuffer[14]);

    rt_kprintf("BSi:  %02x\nBRi:  %02x\nPPi:  %02x\nInGeneralBytesLength : 0x%02x\nInGeneralBytes  (hex): ", ConnectionDataBuffer[15], ConnectionDataBuffer[16], ConnectionDataBuffer[17], ConnectionDataBufferLength - 17);
    for (i = 17; i < ConnectionDataBufferLength; i++)
    {
        rt_kprintf("%02X ", ConnectionDataBuffer[i]);
    }
    rt_kprintf("\nInGeneralBytes (char): ");

    for (i = 17; i < ConnectionDataBufferLength; i++)
    {
        rt_kprintf("%2c ", ConnectionDataBuffer[i]);
    }
}


void PrepareResponseData(phcsExampleTargetParam_t* TParams)
{
    if (((TParams->SendLength) > (TParams->MTU)) &&
        ((TParams->OpenChainingData) > (TParams->MTU)) &&
        (TParams->QuitDEP != 0x01))
    {
        // Meta-Chaining needed as amount of response data too big
        // rt_kprintf("Use MI\n");

        if (TParams->MetaChaining = 0)
        {
            //first run in chaining sequence
            TParams->index = 0;
        }
        TParams->DepMoreInfo.DepFlags.MetaChaining = 1;         // MI bit SET
        TParams->OpenChainingData = (TParams->OpenChainingData) - ((uint16_t)TParams->MTU);
        TParams->SendLength = ((uint16_t)TParams->MTU);
        TParams->index = TParams->MetaChainingCounter * ((uint16_t)TParams->MTU);
        TParams->MetaChainingCounter += 1;
        TParams->MetaChaining = 1;
    }
    else if ((TParams->SendLength == (TParams->MTU)) &&
        ((TParams->OpenChainingData) >= (TParams->MTU)) &&
        ((TParams->MetaChaining) == 1) &&
        (TParams->QuitDEP != 0x01))
    {
        // If Meta Chaining is in use and used for next transceive, too
        // rt_kprintf("Further Use of MI\n");
        TParams->DepMoreInfo.DepFlags.MetaChaining = 1;         // KEEP MI bit set
        TParams->SendLength = ((uint16_t)(TParams->MTU));
        TParams->OpenChainingData = (TParams->OpenChainingData) - ((uint16_t)(TParams->MTU));
        TParams->MetaChaining = 1;
        TParams->index = TParams->MetaChainingCounter * ((uint16_t)TParams->MTU);
        TParams->MetaChainingCounter += 1;
    }
    else if ((TParams->SendLength <= (TParams->MTU)) &&
        ((TParams->OpenChainingData) <= (TParams->MTU) &&
            ((TParams->MetaChaining) == 1)) &&
        (TParams->QuitDEP != 0x01))
    {
        // If Meta Chaining was used and is not needed any more: MI RESET
        // rt_kprintf("Reset Use of MI\n");

        TParams->DepMoreInfo.DepFlags.MetaChaining = 0;
        TParams->SendLength = TParams->OpenChainingData;
        TParams->MetaChaining = 0;
        TParams->index = TParams->MetaChainingCounter * ((uint16_t)TParams->MTU);
        TParams->MetaChainingCounter = 0;
    }
    else if ((TParams->buffer[0] == 'e' && TParams->buffer[1] == 'n' && TParams->buffer[2] == 'd') || ((TParams->QuitDEP == 0x01) && TParams->status != NFCSTATUS_SUCCESS))
    {
        // In case 'end' was detected from initiator close the data transfer and send 'end' as response, too
        // rt_kprintf("\nPrepare 'end' to be send back as DEP_RES, too.\n");
        TParams->buffer[0] = 'e';
        TParams->buffer[1] = 'n';
        TParams->buffer[2] = 'd';
        TParams->QuitDEP = 1;
        TParams->SendLength = 3;
        TParams->index = 0;
        TParams->OpenChainingData = 3;
        TParams->MetaChaining = 0;
        TParams->DepMoreInfo.DepFlags.MetaChaining = 0;
    }
    else
    {
        // no meta chaining needed - normal DEP_RES send
        TParams->index = 0;
        // rt_kprintf("MI not set (not needed)\n");
    }
}







// All data needed during NFC Initiator mode
typedef struct phcsExampleInitiatorParam
{
    // Error handling
    NFCSTATUS               status;       /*!< \brief [in/out]  */
    // Variable to store user interface input
    uint8_t                 menu_choice;
    // Data buffer to store transmit data
    uint8_t                 buffer[DATABUFFERSIZE];
    // Data buffer to store receive data
    uint8_t                 receivebuffer[DATABUFFERSIZE];

    //---------------------
    // INITIATOR ACTIVATION
    //---------------------

    // Hardware information of the NFC Initiator
    phHal_sHwReference_t* HwRef;

    //------------------
    // TARGET ACTIVATION
    //------------------

    // Number of NFC remote devices found
    uint8_t                 nbRemoteDev;
    // Table of target modes accepted
    phHal_eOpModes_t        PollModes[NB_OP_MODE_MAX];
    // Target device information struct, only one target shall be accepted as P2P
    phHal_sRemoteDevInformation_t   remoteDevInfoList[1];
    // Table of target data used by the initiator to establish the P2P communication
    // NFCIDs, Use DiD, NAD, General Bytes...
    phHal_sDevInputParam_t  communicationparams[2];

    //--------------
    // DATA EXCHANGE
    //--------------

    // DEP_REQ command
    phHal_uCmdList_t        DEPcmd;
    // how many data (payload) bytes shall be transmitted to the target
    uint16_t                SendLength;
    // how many bytes were received as target response
    uint16_t                RcvLength;
    // more information for DEP_REQ e.g. the MI bit is set inhere
    phHal_sDepAdditionalInfo_t DepMoreInfo;
    // is meta chaining needed to fulfill SendLength settings ?
    uint8_t                 MetaChaining;
    // to keep in mind how many data bytes are still open
    // to be transmitted when using meta chaining
    uint16_t                OpenChainingData;
    // keep track on number of meta chaining operations
    uint16_t                MetaChainingCounter;
    // counter to keep track on the iteration status which is set b
    // by the user inside p2pDataFlowParams_t
    uint16_t                iterations;
    // keep track on maximum amount of data without using meta chaining
    uint16_t                MTU;
    //from which index of the buffer onwards data is send
    uint16_t                index;
    // is incoming data chained using MI?
    uint16_t                receiveindex;
    // receive loop finished?
    uint8_t                 receivecomplete;

} phcsExampleInitiatorParam_t;

void Init_Initiator_Parameters(phcsExampleInitiatorParam_t* IParam)
{
    uint16_t i = 0;

    // set the major structs to zero
    memset(&IParam->remoteDevInfoList, 0, sizeof(IParam->remoteDevInfoList));
    memset(&IParam->communicationparams, 0, sizeof(IParam->communicationparams));
    memset(&IParam->DepMoreInfo, 0, sizeof(IParam->DepMoreInfo));
    memset(&IParam->DEPcmd, 0, sizeof(IParam->DEPcmd));
    memset(&IParam->PollModes, 0, sizeof(IParam->PollModes));

    IParam->status = NFCSTATUS_SUCCESS;
    IParam->menu_choice = 0;
    IParam->nbRemoteDev = 2;

    IParam->DEPcmd.NfcInCmd = 0;
    IParam->SendLength = 0;
    IParam->RcvLength = 0;    //DATABUFFERSIZE;
    IParam->MetaChainingCounter = 0;
    IParam->OpenChainingData = 0;
    IParam->MetaChaining = 0;
    IParam->index = 0;
    IParam->receiveindex = 0;
    IParam->receivecomplete = 0;
    IParam->communicationparams->DIDiUsed = 0;
    IParam->communicationparams->CIDiUsed = 0;
    IParam->communicationparams->NfcNADiUsed = 0;

    IParam->PollModes[0] = phHal_eOpModesArrayTerminator;

    for (i = 0; i < NUMBER_GENERAL_BYTES; i++)
    {
        IParam->communicationparams->GeneralByte[i] = (0xB0 + (uint8_t)i);
    }

    //init the transceive buffer with some data
    for (i = 0; i < DATABUFFERSIZE; i++)
    {
        IParam->buffer[i] = i % 255;                      //fill with hex numbers 0x00 -> 0xFE
        IParam->receivebuffer[i] = i % 26 + 97;
        //IParam->buffer[i] = i%26 + 97;                    //for the alphabeth 'a' till 'z'
        //IParam->buffer[i] = i%26 + 97 - ('a' -'A');       //for the alphabeth 'A' till 'Z'
    }
}

void SetCommunicationParams(phcsExampleInitiatorParam_t* IParams)
{
    // Set the DiD usage bit according to P2PDataFlow Params
    IParams->communicationparams[0].DIDiUsed = 0;
    IParams->communicationparams[1].DIDiUsed = 0;
    // Set the NAD usage bit according to P2PDataFlow Params
    IParams->communicationparams[0].NfcNADiUsed = 0;
    IParams->communicationparams[1].NfcNADiUsed = 0;

    // Generate NFCIDs automatically
    IParams->communicationparams[0].NFCIDAuto = 1;
    IParams->communicationparams[1].NFCIDAuto = 1;

    /*
    // NFCID3i will be generated by lower layers (or by HW)
    // no need to initialise since NFCIDAuto = 1
    IParams->communicationparams[0].NFCID3i[0] = 0xA0;
    IParams->communicationparams[0].NFCID3i[1] = 0xA1;
    IParams->communicationparams[0].NFCID3i[2] = 0xA2;
    IParams->communicationparams[0].NFCID3i[3] = 0xA3;
    IParams->communicationparams[0].NFCID3i[4] = 0xA4;
    IParams->communicationparams[0].NFCID3i[5] = 0xA5;
    IParams->communicationparams[0].NFCID3i[6] = 0xA6;
    IParams->communicationparams[0].NFCID3i[7] = 0xA7;
    IParams->communicationparams[0].NFCID3i[8] = 0xA8;
    IParams->communicationparams[0].NFCID3i[9] = 0xA9;
    */

    // remark : The maximum possible length is defined in ISO18092 standard
    IParams->communicationparams[0].GeneralByteLength = NUMBER_GENERAL_BYTES;

    IParams->communicationparams[0].GeneralByte[0] = 'W';
    IParams->communicationparams[0].GeneralByte[1] = 'h';
    IParams->communicationparams[0].GeneralByte[2] = 'o';

    IParams->communicationparams[0].GeneralByte[3] = ' ';
    IParams->communicationparams[0].GeneralByte[4] = 'a';
    IParams->communicationparams[0].GeneralByte[5] = 'r';
    IParams->communicationparams[0].GeneralByte[6] = 'e';

    IParams->communicationparams[0].GeneralByte[7] = ' ';
    IParams->communicationparams[0].GeneralByte[8] = 'y';
    IParams->communicationparams[0].GeneralByte[9] = 'o';
    IParams->communicationparams[0].GeneralByte[10] = 'u';
    IParams->communicationparams[0].GeneralByte[11] = '?';

    // needed for 212 / 424kbps activation passive
    IParams->communicationparams[0].NfcPollPayload[0] = 0x00;
    IParams->communicationparams[0].NfcPollPayload[1] = 0xFF;
    IParams->communicationparams[0].NfcPollPayload[2] = 0xFF;
    IParams->communicationparams[0].NfcPollPayload[3] = 0x00;
    IParams->communicationparams[0].NfcPollPayload[4] = 0x01;

    IParams->communicationparams[1].GeneralByteLength = 0;
    return;
}



//phcsExampleInitParam_t  InitParams;
//phcsExampleInitiatorParam_t IParam;

//int nfcTest(sky_comDriver *dev)
//{
//	
//	char c;
//	int i = 0;

//	
//    rt_kprintf("\n1111---------------------------------\n");
//	
//	// set the major structs to zero
//    memset(&InitParams.HwRef[0], 0, sizeof(InitParams.HwRef));
//	
//	InitParams.HwRef[0].pBoardDriver = dev;
//    InitParams.index = 0;
//    InitParams.NbrOfDevDetected = MAX_BOARD_NUMBER;
//    InitParams.status = NFCSTATUS_SUCCESS;
//    InitParams.MTU = 0;
//    InitParams.MultiPoll = 1;
//	
//	
//    Initialise_NFC_Hardware(&InitParams);
//    rt_kprintf("\n222---------------------------------\n");


//    IParam.HwRef = &(InitParams.HwRef)[InitParams.index];
//    // Take over number of maximum transfer unit
//    IParam.MTU = InitParams.MTU;

//    Init_Initiator_Parameters(&IParam);

//    IParam.PollModes[i] = phHal_eOpModesNfcPassive106;
//    i++;
//    IParam.PollModes[i] = phHal_eOpModesNfcPassive212;
//    i++;
//    IParam.PollModes[i] = phHal_eOpModesNfcPassive424;
//    i++;
//    IParam.PollModes[i] = phHal_eOpModesNfcActive106;
//    i++;
//    IParam.PollModes[i] = phHal_eOpModesNfcActive212;
//    i++;
//    IParam.PollModes[i] = phHal_eOpModesNfcActive424;
//    i++;
//    IParam.PollModes[i] = phHal_eOpModesArrayTerminator;

//    SetCommunicationParams(&IParam);
//	
//	
//	 while (1)
//    {
//        //Poll for available remote devices
//        IParam.status = phHalNfc_Poll(IParam.HwRef,
//            IParam.PollModes,
//            IParam.remoteDevInfoList,
//            &IParam.nbRemoteDev,
//            IParam.communicationparams);

//        rt_kprintf(">%d, phHalNfc_Poll\n", IParam.status);

//        if ((IParam.status == NFCSTATUS_SUCCESS))
//        {
//            for (int j = 0; j < IParam.nbRemoteDev; j++)
//            {
//                phHal_eOpModes_t currentOpMode;

//                rt_kprintf("------------------------------------------------------------\n");
//                rt_kprintf("Poll Result :  \n");
//                rt_kprintf("hLogHandle : %d\n", IParam.remoteDevInfoList[j].hLogHandle);
//                rt_kprintf("Session Opened : %d\n", IParam.remoteDevInfoList[j].SessionOpened + 1);
//                currentOpMode = IParam.remoteDevInfoList[j].OpMode;
//                /*  if (currentOpMode == phHal_eOpModesMifare)
//                  if (currentOpMode == phHal_eOpModesNfcPassive106)*/
//                printf("------------------------------------------------------------\n");
//            }
//            if (IParam.status == NFCSTATUS_SUCCESS)
//            {
//                IParam.status = phHalNfc_Connect(IParam.HwRef,
//                    IParam.PollModes[0],
//                    &(IParam.remoteDevInfoList[0]),
//                    &(IParam.communicationparams[0]));
//            }

//            // Feedback to the user after Connect operation
//            if (IParam.status == NFCSTATUS_SUCCESS)
//            {
//                rt_kprintf("> Connect Successful! OK\n");
//                break;
//            }
//            else
//            {
//                rt_kprintf("> Connect NOT Successful! OK\n");
//                break;
//            }
//        }
//        else
//        {
//            rt_kprintf("Poll : NO_DEVICE_FOUND\n");
//            break;
//        }
//    }
//    
//}


phcsExampleInitParam_t  InitParams;
phcsExampleTargetParam_t TParams;


int nfcTest(sky_comDriver *dev)
{
	
	char c;
int i = 0;
	uint8_t     FrameAsleep[]   =   {0x00,0x00,0xFF,0x05,0xFB,0xD4,0x14,0x02,0x00,0x00,0x16,0x00};
	//unsigned char ssData[] = {0x00, 0xff, 0x03, 0xfd, 0xd4, 0x10, 0x04, 0x18, 0x00};
	uint16_t sendLIen;
	
    rt_kprintf("\n1111---------------------------------\n");
	
	// set the major structs to zero
    memset(&InitParams.HwRef[0], 0, sizeof(InitParams.HwRef));
	
	InitParams.HwRef[0].pBoardDriver = dev;
    InitParams.index = 0;
    InitParams.NbrOfDevDetected = MAX_BOARD_NUMBER;
    InitParams.status = NFCSTATUS_SUCCESS;
    InitParams.MTU = 0;
    InitParams.MultiPoll = 1;
	
	
	//if(!Serial232Write(NULL, handle, &FrameAsleep[0], sizeof(FrameAsleep), &dwDone, T_RS232_RETRY, T_RS232_TIMEOUT_DEFAULT))
//	(phHal_sHwReference_t *psHwReference,
//                                uint8_t              *pBuffer,  
//							    uint16_t              nNumberOfBytesToWrite,
//							    uint16_t             *pNumberOfBytesWritten)
//								+
//	rt_kprintf("\nFrameAsleep------------------------------\n");
//	phDalNfc_Write(&InitParams.HwRef[0],FrameAsleep, sizeof(FrameAsleep),&sendLIen);
//////	
//////	
////	rt_kprintf("\nFrameAsleepEND------------------------------\n");
//	rt_thread_mdelay(500);
//	TAMABaudrateChange(&can_communicate);
//	rt_thread_mdelay(100);
//	rt_kprintf("\nTAMABaudrateChangen:%d\r\n",can_communicate);
	
    Initialise_NFC_Hardware(&InitParams);
    rt_kprintf("\n222---------------------------------\n");

    
    // Take over target hardware information
    TParams.HwRef = &(InitParams.HwRef)[InitParams.index];
    // Take over number of maximum transfer unit
    TParams.MTU = InitParams.MTU;


    Init_Target_Parameters(&TParams);
    SetTargetCommunicationParams(&TParams);

    SelectTargetPollingMode(&TParams);

    rt_kprintf("\n322---------------------------------\n");
	
	
	
	
    while (1)
    {
		i = 0;
        while (1)
        {
            TParams.status = phHalNfc_StartTargetMode(TParams.HwRef,
                &(TParams.communicationparams),
                TParams.PollModes,
                TParams.ConnectionReq,
                &(TParams.ConnectionReqBufLength));

            rt_kprintf("\n333---------------------------------\n");
            if (TParams.status == NFCSTATUS_SUCCESS)
            {
                // Display initiator activation information here
                ProcessTargetModeData(TParams.ConnectionReq, TParams.ConnectionReqBufLength);
                rt_kprintf("\nNADi: %02x", TParams.DepMoreInfo.DepFlags.NADPresent);
                rt_kprintf("\n---------------------------------\n");
                break;
            }
            else
            {
				i++;
				if (i > 5)
					return -1;
                continue;
            }

        }



        rt_kprintf("\n444---------------------------------\n");
        // Receive data from the initiator
        do {
            // loop running as long as the initator wants to, hint: 'end'
            TParams.index = 0;
            TParams.RcvLength = DATABUFFERSIZE;
            
            TParams.status = phHalNfc_Receive(TParams.HwRef,
                &(TParams.DepMoreInfo),
                (TParams.buffer),
                &(TParams.RcvLength));


            /* ProcessReceivedData(&TParams);*/
            if ((TParams.status == NFCSTATUS_SUCCESS) )
            {
               /* for (i = 0; i < TParams.RcvLength; i++)
                {
                    if ((i > 0) && ((i % 16) == 0)) { printf("\n"); }
                    printf("%02X ", TParams.buffer[i]);
                    TParams.buffer[i] = TParams.buffer[i] + 1;
                }
                printf("\n");*/
            }
            else
            {
                rt_kprintf(">recstatus :%d, phHalNfc_Receive!\n", TParams.status);
                break;
            }

            TParams.index = 0;
            TParams.SendLength = TParams.RcvLength;
            //TParams.SendLength = 0;
            TParams.DepMoreInfo.DepFlags.MetaChaining = 0;
            
            
            rt_thread_mdelay(10);

            // TX
            TParams.status = phHalNfc_Send(TParams.HwRef,
                &(TParams.DepMoreInfo),
                (TParams.buffer ),
                TParams.SendLength);

            if (TParams.status != NFCSTATUS_SUCCESS)
            {
                rt_kprintf(">recstatus :%d, phHalNfc_Send!\n", TParams.status);
                break;
            }
            


        } while (TParams.status == NFCSTATUS_SUCCESS && TParams.QuitDEP == 0); // until release or deselect was detected

    }

	
	return 0;
}







