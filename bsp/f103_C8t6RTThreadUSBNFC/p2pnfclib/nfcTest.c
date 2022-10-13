// nfcTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#include <iostream>
#include <stdio.h>
#include "phTalTama.h"
#include "phDalNfc.h"
#include "phHalNfc.h"
#include "phOsalNfc.h"


#define MAX_BOARD_NUMBER                5

typedef struct phcsExampleInitParam
{
    phHal_sHwReference_t    HwRef[MAX_BOARD_NUMBER];
    uint8_t                 NbrOfDevDetected;
    uint8_t                 index;
    uint32_t                MTU;
    uint8_t                 MultiPoll;
    NFCSTATUS               status;

} phcsExampleInitParam_t;

void Init_Init_Parameters(phcsExampleInitParam_t* InitParams)
{
    // set the major structs to zero
    memset(&InitParams->HwRef[0], 0, sizeof(InitParams->HwRef));

    InitParams->index = 0;
    InitParams->NbrOfDevDetected = MAX_BOARD_NUMBER;
    InitParams->status = NFCSTATUS_SUCCESS;
    InitParams->MTU = 0;
    InitParams->MultiPoll = 1;
}

void ProcessEnumerateResult(phcsExampleInitParam_t* InitParams)
{
    uint8_t n, c;

    printf("\nAmount of boards detected : %d\n\n", InitParams->NbrOfDevDetected);
    if (InitParams->status == NFCSTATUS_SUCCESS)
    {
        for (n = 0; n < (InitParams->NbrOfDevDetected); n++)
        {
            printf("\n* %d ", n + 1);
            if (((InitParams->HwRef)[n].LinkType) == 1)          // COM Port Type
            {
                printf(" Link: %.4s", (InitParams->HwRef)[n].DalContext.sLinkName);
                //printf("      Type: %s \n", (InitParams->HwRef)[n].ChipName);
            }
            else
                printf(" Link: Unknown\n");
            
        }
        printf("   --> default choice\n");
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

    // Initialise Parameters
    Init_Init_Parameters(InitParams);

    // Enumerate: Find out which hardware is connected to the PC
    (InitParams->status) = phHalNfc_Enumerate(InitParams->HwRef, &(InitParams->NbrOfDevDetected));

    // User interaction to decide which hardware shall be opened
    ProcessEnumerateResult(InitParams);

    // Open Board according to user input
    (InitParams->status) = phHalNfc_Open(&(InitParams->HwRef[InitParams->index]));

    // Error handling
    if (InitParams->status == NFCSTATUS_SUCCESS)
        printf("\nOpen SUCCESSFUL \n");
    else
    {
        printf("\nOpen NOT SUCCESSFUL - Error code: %d \n\n !!! Please ENUMERATE AGAIN !!!\n\n", InitParams->status);
        phOsalNfc_Suspend(3500);
        return;
    }

    // Get the maximum transfer unit size (MTU) to control data exchange
    InitParams->status = phHalNfc_GetDeviceCapabilities(&(InitParams->HwRef[InitParams->index]), &sDevCapabilities);
    InitParams->MTU = (sDevCapabilities.MTU);
    InitParams->MultiPoll = (sDevCapabilities.MultiPollSessionSup);

    // Error handling
    if (InitParams->status == NFCSTATUS_SUCCESS)
        printf("Get Device Capabilities SUCCESSFUL \n");
    else
    {
        printf("\nGet Device Capabilities NOT SUCCESSFUL - Error code: %d \n", InitParams->status);
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
    printf("\n---------------------------------\n*** NFC Protocol ***\n");

    // Display the total length of ATR_REQ
    printf("Length of ATR_REQ: %02x\n", ConnectionDataBuffer[0]);
    // Index 1 and 2 of ATR_REQ indicate the CMD0 and CMD1
    // Index 3 till 13 shows the NFCID3
    printf("NFCID3i: ");
    for (i = 3; i <= 13; i++)
    {
        printf("%02X ", ConnectionDataBuffer[i]);
    }
    // DiD used?
    printf("\nDiDi: %02x\n", ConnectionDataBuffer[14]);

    printf("BSi:  %02x\nBRi:  %02x\nPPi:  %02x\nInGeneralBytesLength : 0x%02x\nInGeneralBytes  (hex): ", ConnectionDataBuffer[15], ConnectionDataBuffer[16], ConnectionDataBuffer[17], ConnectionDataBufferLength - 17);
    for (i = 17; i < ConnectionDataBufferLength; i++)
    {
        printf("%02X ", ConnectionDataBuffer[i]);
    }
    printf("\nInGeneralBytes (char): ");

    for (i = 17; i < ConnectionDataBufferLength; i++)
    {
        printf("%2c ", ConnectionDataBuffer[i]);
    }
}


void PrepareResponseData(phcsExampleTargetParam_t* TParams)
{
    if (((TParams->SendLength) > (TParams->MTU)) &&
        ((TParams->OpenChainingData) > (TParams->MTU)) &&
        (TParams->QuitDEP != 0x01))
    {
        // Meta-Chaining needed as amount of response data too big
        // printf("Use MI\n");

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
        // printf("Further Use of MI\n");
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
        // printf("Reset Use of MI\n");

        TParams->DepMoreInfo.DepFlags.MetaChaining = 0;
        TParams->SendLength = TParams->OpenChainingData;
        TParams->MetaChaining = 0;
        TParams->index = TParams->MetaChainingCounter * ((uint16_t)TParams->MTU);
        TParams->MetaChainingCounter = 0;
    }
    else if ((TParams->buffer[0] == 'e' && TParams->buffer[1] == 'n' && TParams->buffer[2] == 'd') || ((TParams->QuitDEP == 0x01) && TParams->status != NFCSTATUS_SUCCESS))
    {
        // In case 'end' was detected from initiator close the data transfer and send 'end' as response, too
        // printf("\nPrepare 'end' to be send back as DEP_RES, too.\n");
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
        // printf("MI not set (not needed)\n");
    }
}


int main3()
{
    char c;
    int i = 0;

    phcsExampleInitParam_t  InitParams;

    printf("\n1111---------------------------------\n");
    Initialise_NFC_Hardware(&InitParams);
    printf("\n222---------------------------------\n");
   
    phcsExampleTargetParam_t TParams;
    // Take over target hardware information
    TParams.HwRef = &(InitParams.HwRef)[InitParams.index];
    // Take over number of maximum transfer unit
    TParams.MTU = InitParams.MTU;


    Init_Target_Parameters(&TParams);
    SetTargetCommunicationParams(&TParams);

    SelectTargetPollingMode(&TParams);

    while (1)
    {
        while (1)
        {
            TParams.status = phHalNfc_StartTargetMode(TParams.HwRef,
                &(TParams.communicationparams),
                TParams.PollModes,
                TParams.ConnectionReq,
                &(TParams.ConnectionReqBufLength));

            printf("\n333---------------------------------\n");
            if (TParams.status == NFCSTATUS_SUCCESS)
            {
                // Display initiator activation information here
                ProcessTargetModeData(TParams.ConnectionReq, TParams.ConnectionReqBufLength);
                printf("\nNADi: %02x", TParams.DepMoreInfo.DepFlags.NADPresent);
                printf("\n---------------------------------\n");
                break;
            }
            else
            {
                continue;
            }

        }


        TParams.SendLength = 52;
        TParams.OpenChainingData = 52;
        TParams.MetaChainingCounter = 0;
        TParams.iterations = 0;

        printf("\n444---------------------------------\n");
        // Receive data from the initiator
        do {
            // loop running as long as the initator wants to, hint: 'end'
            TParams.index = 0;
            do
            {
                // while loop needed if MI is used inside receive
                //Receive data from initiator
                TParams.RcvLength = DATABUFFERSIZE - (TParams.index);
                //RX
                printf("\n555---------------------------------\n");
                TParams.status = phHalNfc_Receive(TParams.HwRef,
                    &(TParams.DepMoreInfo),
                    (TParams.buffer + (TParams.index)),
                    &(TParams.RcvLength));

                /* ProcessReceivedData(&TParams);*/
                if ((TParams.status == NFCSTATUS_SUCCESS) && TParams.DepMoreInfo.DepFlags.MetaChaining == 0x00)
                {
                    TParams.RcvLength += TParams.index;
                    printf("\nNumber of bytes received from initiator is: %5d\n\n", TParams.RcvLength);
                    for (i = 0; i < TParams.RcvLength; i++)
                    {
                        if ((i > 0) && ((i % 16) == 0)) { printf("\n"); }
                        printf("%02X ", TParams.buffer[i]);
                        TParams.buffer[i] = TParams.buffer[i] + 1;
                    }
                    printf("\n");

                    // All data collected - go on with response data
                    TParams.index = 0;
                    TParams.RcvLength = 0;
                }
                else if ((TParams.DepMoreInfo.DepFlags.MetaChaining) == 0x01)
                {
                    if ((TParams.index += TParams.RcvLength) < DATABUFFERSIZE)
                    {
                        TParams.RcvLength = 0;
                        TParams.iterations--;
                    }
                }

                TParams.iterations++;

            } while (TParams.status == NFCSTATUS_SUCCESS && TParams.QuitDEP == 0 && TParams.DepMoreInfo.DepFlags.MetaChaining != 0x00);


            printf("%d,%d,%d,  1111111111111111111111111\r\n", TParams.status, TParams.QuitDEP, TParams.DepMoreInfo.DepFlags.MetaChaining);
            // To check WTX handling a delay might be introduced here
            phOsalNfc_Suspend(10);

            do {
                // How many bytes are send back as response? Meta chaining needed?
                PrepareResponseData(&TParams);
                // TX
                TParams.status = phHalNfc_Send(TParams.HwRef,
                    &(TParams.DepMoreInfo),
                    (TParams.buffer + (TParams.index)),
                    TParams.SendLength);

                if ((TParams.MetaChaining == 0) &&
                    (TParams.MetaChainingCounter == 0))
                {
                    // all data was reeived and send so prepare for next iteration
                    // printf("Reset DEP_RES data for next DEP_REQ\n");
                    TParams.SendLength = 52;  // reload value for next iteration
                    TParams.OpenChainingData = 52;
                    TParams.MetaChainingCounter = 0;                                // reset counter for next iteration
                    TParams.index = 0;
                }


            } while (TParams.MetaChaining != 0 && TParams.status == NFCSTATUS_SUCCESS && TParams.QuitDEP == 0);

            printf("%d,%d,%d,  222222222222\r\n", TParams.status, TParams.MetaChaining, TParams.QuitDEP);

        } while (TParams.status == NFCSTATUS_SUCCESS && TParams.QuitDEP == 0); // until release or deselect was detected

    }
    

   
}



int main()
{
    char c;
    int i = 0;

    phcsExampleInitParam_t  InitParams;

    printf("\n1111---------------------------------\n");
    Initialise_NFC_Hardware(&InitParams);
    printf("\n222---------------------------------\n");

    phcsExampleTargetParam_t TParams;
    // Take over target hardware information
    TParams.HwRef = &(InitParams.HwRef)[InitParams.index];
    // Take over number of maximum transfer unit
    TParams.MTU = InitParams.MTU;


    Init_Target_Parameters(&TParams);
    SetTargetCommunicationParams(&TParams);

    SelectTargetPollingMode(&TParams);

    printf("\n322---------------------------------\n");

    while (1)
    {
        while (1)
        {
            TParams.status = phHalNfc_StartTargetMode(TParams.HwRef,
                &(TParams.communicationparams),
                TParams.PollModes,
                TParams.ConnectionReq,
                &(TParams.ConnectionReqBufLength));

            printf("\n333---------------------------------\n");
            if (TParams.status == NFCSTATUS_SUCCESS)
            {
                // Display initiator activation information here
                ProcessTargetModeData(TParams.ConnectionReq, TParams.ConnectionReqBufLength);
                printf("\nNADi: %02x", TParams.DepMoreInfo.DepFlags.NADPresent);
                printf("\n---------------------------------\n");
                break;
            }
            else
            {
                continue;
            }

        }



        printf("\n444---------------------------------\n");
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
                printf(">recstatus :%d, phHalNfc_Receive!\n", TParams.status);
                break;
            }

            TParams.index = 0;
            TParams.SendLength = TParams.RcvLength;
            //TParams.SendLength = 0;
            TParams.DepMoreInfo.DepFlags.MetaChaining = 0;
            
            
            phOsalNfc_Suspend(10);

            // TX
            TParams.status = phHalNfc_Send(TParams.HwRef,
                &(TParams.DepMoreInfo),
                (TParams.buffer ),
                TParams.SendLength);

            if (TParams.status != NFCSTATUS_SUCCESS)
            {
                printf(">recstatus :%d, phHalNfc_Send!\n", TParams.status);
                break;
            }
            


        } while (TParams.status == NFCSTATUS_SUCCESS && TParams.QuitDEP == 0); // until release or deselect was detected

    }



}
