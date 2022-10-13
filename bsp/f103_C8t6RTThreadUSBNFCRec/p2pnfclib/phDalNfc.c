#include <stdio.h>


#include <phTalTama.h>
#include <phNfcStatus.h>
#include "commonIOInterface.h"
#include <rtthread.h>


//NFCSTATUS phDalNfc_Enumerate(phHal_sHwReference_t  sHwReference[],
//                                    uint8_t              *pNbrOfDevDetected )
//{
//		NFCSTATUS result = NFCSTATUS_SUCCESS;
//	
//	return result;
//}

//NFCSTATUS phDalNfc_Open(phHal_sHwReference_t *psHwReference)
//{
//	NFCSTATUS result = NFCSTATUS_SUCCESS;
//	phDal_Context_t * pDalContext;
//	
//	return result;
//}
int TAMABaudrateChange      (  uint8_t* can_communicate);

extern NFCSTATUS phDalNfc_Open(phHal_sHwReference_t *psHwReference)
{
    NFCSTATUS result                = NFCSTATUS_SUCCESS;
    
	uint8_t can_communicate;
    phDal_Context_t * pDalContext;
//#ifdef _UNICODE
//    wchar_t sLinkName[MAX_PATH];
//#else
//    int8_t  sLinkName[MAX_PATH];
//#endif
	char  sLinkName[10];

    #ifdef PHJALJOINER
        HANDLE                      h_serial_port;
        phDal_sBFLLowerFrame_t      *temp_bfl_lower_frame  = NULL;
        phJal_sBFLUpperFrame_t      *temp_bfl_upper_frame  = NULL;
        phcsBfl_Status_t             rclstatus = PH_ERR_BFL_SUCCESS;
        OVERLAPPED                  *overlapped_structure  = NULL;
        const uint8_t                jbaud_115200 = PHDAL_JBAUD_115K;
        const uint8_t                initiator = 1;
        const uint8_t                do_not_init_chip = 0;   /* Prevent attempt to init chip! */
    #endif /* PHJALJOINER */
    

    uint8_t   version[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    uint32_t  lenght;

    if (psHwReference == NULL)
    {
        result = PHNFCSTVAL(CID_NFC_DAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if (psHwReference->Flags.LinkValid)
        {
            result = PHNFCSTVAL(CID_NFC_DAL, NFCSTATUS_INVALID_DEVICE);
        }
    }

    /* Open a new link to the device (driver) */
    if (result == NFCSTATUS_SUCCESS)
    {
        pDalContext             = &psHwReference->DalContext;

        switch (psHwReference->CompType)
        {
            case CID_NFC_TAL:
                {

                    strcpy(sLinkName, pDalContext->sLinkName);

                    switch(psHwReference->LinkType)
                    {

                    case PHHAL_SERIAL_DEVICE:


						/*  make sure the baudrate is set to 115200.    */
//						if(!TAMABaudrateMatchWithHost(Handle, CBR_115200, 0))
//						{
//							return PHNFCSTVAL(CID_NFC_DAL, NFCSTATUS_INVALID_DEVICE);
//						}
//						else
//						{
//							psHwReference->pBoardDriver =   Handle;
					
//					rt_kprintf("for TAMABaudrateChange(NULL, handle,)\r\n\r\n\r\n");
//                
					TAMABaudrateChange(&can_communicate);


//					rt_kprintf("for TAMABaudrateChange(NULL,)END\r\n\r\n\r\n");
					
					
							psHwReference->Flags.LinkValid = 0x01;
							psHwReference->Sleeping =   1;
//						}
//                       
                        /* In case of PN 532 v1.5 or newer, we have to put the chip in Off mode after the Open is successfull */
                        break;

                    case PHHAL_USB_DEVICE:
                    default:

                       
                        break;

                    }   /*  switch  */
                }

                break;

            default:
                result = PHNFCSTVAL(CID_NFC_DAL, NFCSTATUS_INVALID_DEVICE);
                break;
        }


    } else
    {
        /* Initial check failure: Do not change the state! */
    }

     return result;
}




NFCSTATUS phDalNfc_Close(phHal_sHwReference_t *psHwReference)
{
	NFCSTATUS result = NFCSTATUS_SUCCESS;
	
	return result;
}

typedef unsigned int       DWORD;
typedef unsigned char       BOOL;

NFCSTATUS phDalNfc_Write(phHal_sHwReference_t *psHwReference,
                                uint8_t              *pBuffer,  
							    uint16_t              nNumberOfBytesToWrite,
							    uint16_t             *pNumberOfBytesWritten)
{
	NFCSTATUS result = NFCSTATUS_SUCCESS;
	extern sky_comDriver *uart1Dri;
	sky_comDriver *dev;
	if (psHwReference == 0)
		dev = uart1Dri;
	else
		dev = psHwReference->pBoardDriver;
	
//	rt_kprintf("\r\nwrite\r\n");
//	for (int i = 0; i < nNumberOfBytesToWrite; i++)
//	{
//		rt_kprintf("%02x ",pBuffer[i]);
//	}
//	rt_kprintf("\r\n");
	devWrite(dev, pBuffer, nNumberOfBytesToWrite);
	if (pNumberOfBytesWritten != 0)
		*pNumberOfBytesWritten = nNumberOfBytesToWrite;
	
	return result;
}


unsigned int tmp_GetMeasureTime_ms( unsigned int ts)
{
	uint32_t tmp;
	tmp = rt_tick_get();

	if(tmp < ts)
	{
		tmp = 0xffffffff - ts + tmp;
	}
	else
	{
		tmp = tmp - ts;
	}
	return tmp;
}

	


	
//sky_waitData(sky_MyByteReceiveDataTypeDef *recType,uint8_t cmd,int time);
BOOL Serial232Read(sky_comDriver *dev, uint8_t *data, int len,uint32_t *pNumberOfBytesRead, int outTime)
{
	
	uint32_t t = rt_tick_get();
	uint8_t res;
	*pNumberOfBytesRead = 0;
	
	while(1)
	{
		while (myFIFORead(dev->fifo,&res,1) > 0)
		{
			t = rt_tick_get();
			data[*pNumberOfBytesRead] = res;
			(*pNumberOfBytesRead)++;
			if (*pNumberOfBytesRead == len)
			{
				
//				rt_kprintf("\r\nRead:%d:",len);
//				rt_kprintf("\r\n");
//				for (int i = 0; i < len; i++)
//				{
//					rt_kprintf("%02x ",data[i]);
//				}
//				rt_kprintf("\r\n");
				return 1;
			}
		}
		
		if (tmp_GetMeasureTime_ms(t) > outTime)
		{
//			rt_kprintf("\r\nTimeOut len:%d,pNumberOfBytesRead:%d:",len,*pNumberOfBytesRead);
//				for (int i = 0; i < *pNumberOfBytesRead; i++)
//				{
//					rt_kprintf("%02x ",data[i]);
//				}
//				rt_kprintf("\r\n");
			break;
		}
		rt_thread_mdelay(2);
	}
	
	return 1;
	
}

NFCSTATUS phDalNfc_Read(phHal_sHwReference_t *psHwReference,
                               uint8_t              *pBuffer,  
							   uint16_t              nNumberOfBytesToRead,
							   uint16_t             *pNumberOfBytesRead,
                               uint32_t              nTimeout)
{
	NFCSTATUS result = NFCSTATUS_SUCCESS;
	BOOL        bResult;
	
	uint32_t    NumberOfBytesRead   =   0;
	*pNumberOfBytesRead =   0;
            
            if(PHHAL_SERIAL_DEVICE  ==  psHwReference->LinkType)
            {
                /* SERIAL Link */

                /*  With USB driver, no problem, we are sure that the number of bytes read
                    matches the number of bytes sent by TAMA. But for serial link,
                    we don't know in advance how many bytes we want to read.
                    Thus, we'll have to prefetch the 6 first bytes in order to analyze the packet type
                    sent by TAMA.   */

                DWORD           NumberOfBytesReadTmp;
                uint8_t         FrameError[]    =   {0x00,0x00,0xFF,0x01,0xFF,0x7F,0x81,0x00};
                uint8_t         FrameNAck[]     =   {0x00,0x00,0xFF,0xFF,0x00,0x00};
                uint8_t         FrameAck[]      =   {0x00,0x00,0xFF,0x00,0xFF,0x00};
                uint8_t         FrameExt[]      =   {0x00,0x00,0xFF,0xFF,0xFF};

                uint8_t         Buffer[9];
                uint32_t        Buffer_sz       =   0;

                size_t          PREFETCH_SZ     =   6;
                DWORD           toRead;

                bResult                     =   0;

                if( nNumberOfBytesToRead == 6 ) /*  15 mSec is to short for USB link. 100mSec is the typical value to used. */
                    nTimeout = 150;
                if( nTimeout == 0 ) /*  For TAL '0' means infinite, but 10 Sec is used to avoid blocking situation  */
                    nTimeout = 10000;

                // First, let's check whether the caller provided enough
                //  buffer size for intelligent processing
                if(nNumberOfBytesToRead <6)
                {
//                    bResult =   Serial232Read       (   psHwReference, Handle, pBuffer, nNumberOfBytesToRead, &NumberOfBytesRead, T_RS232_RETRY, nTimeout, 1);
					bResult =   Serial232Read       (   psHwReference->pBoardDriver,  pBuffer, nNumberOfBytesToRead, &NumberOfBytesRead, nTimeout);
                }
                else
                {
                    //  This is the intelligent processing (with prefetch mechanism)

                    /* first, read the PREFETCH_SZ first bytes  */
					
					Serial232Read       (   psHwReference->pBoardDriver,  &Buffer[0], (uint16_t)PREFETCH_SZ, &NumberOfBytesReadTmp, nTimeout);
//                    Serial232Read       (   psHwReference,
//                                            Handle,
//                                            &Buffer[0],
//                                            (uint16_t)PREFETCH_SZ,
//                                            &NumberOfBytesReadTmp,
//                                            T_RS232_RETRY,
//                                            nTimeout,
//                                            nNumberOfBytesToRead != 6 ?1:0  /*  Does not allow abort if reading an ACK */
//                                            );
					
					
                    NumberOfBytesRead   +=  NumberOfBytesReadTmp;
                    Buffer_sz            =  NumberOfBytesRead;

                    //  Standart infromation Frame : initialize toread with this value
                    toRead              =   Buffer[3] + 1;

                    if(PREFETCH_SZ  ==  NumberOfBytesReadTmp)
                    {
                        if(     0   ==  memcmp(FrameAck, Buffer, sizeof(FrameAck))
                            ||  0   ==  memcmp(FrameNAck, Buffer, sizeof(FrameNAck)))
                        {
                            /*  we've read a full ACK or NACK   */
                            toRead  =   0;
                        }
                        else
                        if(     0   ==  memcmp(FrameError, Buffer, sizeof(FrameError)))
                        {
                            /*  The begining of an Frame error  */
                            if(Buffer[4] != (uint8_t)(0x00 - Buffer[3]))
                            {
                                toRead  =   2;
                            }
                        }
                        else
                        if(     0   ==  memcmp(FrameExt, Buffer, sizeof(FrameExt)))
                        {
                            //  In the case of an Extended infromation Frame
                            //  toread is not defined in the same way as in the standart information frame
                            //Serial232Read       (   psHwReference, Handle, &Buffer[Buffer_sz], 3, &NumberOfBytesReadTmp, T_RS232_RETRY, nTimeout, 1);
							Serial232Read       (   psHwReference->pBoardDriver,  &Buffer[Buffer_sz], 3, &NumberOfBytesReadTmp, nTimeout);
							
							
                            NumberOfBytesRead   +=   NumberOfBytesReadTmp;
                            Buffer_sz            =  NumberOfBytesRead;
                            PREFETCH_SZ+=3;
                            toRead              =   ((uint16_t)Buffer[5]) * 256+ Buffer[6] + 1;
                        }

                        memcpy(pBuffer, Buffer, Buffer_sz);


                        if(toRead <=  nNumberOfBytesToRead - NumberOfBytesRead)
                        {
                            if(0    ==  toRead)
                            {
                                bResult =   1;
                            }
                            else
                            {
                                /*  Let's read the remaining bytes  */
                                //Serial232Read (psHwReference, Handle, &pBuffer[NumberOfBytesRead], (uint16_t)(min(nNumberOfBytesToRead, toRead)), &NumberOfBytesReadTmp, T_RS232_RETRY, T_RS232_TIMEOUT_DEFAULT, 1);
								#define min(a,b) (((a) < (b)) ? (a) : (b))
								Serial232Read       (   psHwReference->pBoardDriver,  &pBuffer[NumberOfBytesRead], (uint16_t)(min(nNumberOfBytesToRead, toRead)), &NumberOfBytesReadTmp, 150);
								
								
                                NumberOfBytesRead   +=  NumberOfBytesReadTmp;

                                if(NumberOfBytesRead    ==  toRead  +   PREFETCH_SZ)
                                {
                                    /*  the operation completed if we've read at the expected number of bytes */
                                    bResult =   1;
                                }
                            }

                        }
                    }
                }
            }
			*pNumberOfBytesRead = (uint16_t)NumberOfBytesRead;
			
	
	return result;
}

#define FALSE 0
#define TRUE 1

/* PN532 v1.5: send a Wakeup frame to deactivate the low VBAT mode of PN532 v1.5
 * This command has no effect on other boards
 * As the PN532 v1.5 is in Low VBat mode by default, we send a long preamble 0x55,0x55
 * following by a defined delay (0x00 0x00 0x00 0x00)
 * before changing the virtual mode into normal mode */
BOOL SendSerialCommand( uint8_t *FrameInOut, uint16_t uiBufferInSize, uint8_t *can_communicate)
{
    uint8_t     FrameAck[]      =   {0x00, 0x00,0xFF,0x00,0xFF,0x00};
    uint8_t     FrameError[]    =   {0x00,0x00,0xFF,0x01,0xFF,0x7F,0x81,0x00};
    DWORD       dwDone;
	extern sky_comDriver *uart1Dri;
	
    *can_communicate =   0;

    /* Write the Command to TAMA */
//    if(!Serial232Write( NULL, handle, &FrameInOut[0], uiBufferInSize, &dwDone, T_RS232_RETRY, T_RS232_TIMEOUT_DEFAULT))
//    {
//        return FALSE;
//    }
	
	phDalNfc_Write( 0,  &FrameInOut[0],uiBufferInSize, 0);

//	rt_thread_mdelay(150);
//    /* Read back the acknowledgement */
//    if(!Serial232Read(NULL, handle, &FrameInOut[0], sizeof(FrameAck), &dwDone, T_RS232_RETRY, T_RS232_TIMEOUT_DEFAULT, 0))
//    {
//        return FALSE;
//    }
	
	Serial232Read       (   uart1Dri,  &FrameInOut[0], sizeof(FrameAck), &dwDone, 150);

    /* verify if we have an ACK */
    if (memcmp(FrameInOut,FrameAck,6)!=0)
    {
        return FALSE;
    }

//    /*  verify we don't have an error frame */
//    if(!Serial232Read(NULL, handle, &FrameInOut[0], sizeof(FrameError), &dwDone, T_RS232_RETRY, T_RS232_TIMEOUT_DEFAULT, 0))
//    {
//        return FALSE;
//    }
	Serial232Read       (   uart1Dri,  &FrameInOut[0], sizeof(FrameError), &dwDone, 150);

    *can_communicate =   1;

    if (memcmp(FrameInOut,FrameError,sizeof(FrameError))==0)
    {
        return FALSE;
    }

    /*  Good. Now, let's read the response */
//    if(!Serial232Read(NULL, handle, FrameInOut+sizeof(FrameError), FrameInOut[3]-1, &dwDone, T_RS232_RETRY, T_RS232_TIMEOUT_DEFAULT, 0))
//    {
//        return FALSE;
//    }

	Serial232Read       (   uart1Dri,   FrameInOut+sizeof(FrameError), FrameInOut[3]-1, &dwDone, 1500);
	
    return TRUE;
}



/*! Changes baud rate of TAMA, according to baudrate.
 *  can_communicate: informs the caller whether the communication is possible.
 *  Returns TRUE if baudrate was successfully set
 */

int TAMABaudrateChange      (  uint8_t* can_communicate)
{

    uint8_t     FrameInOut          [0xFF];
    uint8_t     FrameAck[]      =   {0x00, 0x00,0xFF,0x00,0xFF,0x00};
    uint8_t     FrameError[]    =   {0x00,0x00,0xFF,0x01,0xFF,0x7F,0x81,0x00};
    uint8_t     FrameWakeup[]   =   {0x55,0x55,0x00,0x00,0x00,0x00,0x00,0xFF,0x03,0xFD,0xD4,0x14,0x01,0x17,0x00};/* 5 rising edge necessary to wake up PN532 1.5*/
    uint16_t    uiBufferInSize  =   0;
    uint8_t     pBufferIn           [20];
    DWORD       dwDone;
    uint8_t     uiCmd;
    uint16_t    sum =   0;

    int i;

    *can_communicate =   0;

    /*  We send a wakeup frame followed by a wake up delay
        so that the PN532 is waken up if connected onto this serial port
        This command has no effect onto PN511, PN512, PN531 4.2 & PN532 1.4
    */
//    if(!Serial232Write(NULL, handle, &FrameWakeup[0], sizeof(FrameWakeup), &dwDone, T_RS232_RETRY, T_RS232_TIMEOUT_DEFAULT))
//    {
//        return FALSE;
//    }
	
	phDalNfc_Write( 0,  &FrameWakeup[0], sizeof(FrameWakeup), 0);
	
	extern sky_comDriver *uart1Dri;
	Serial232Read       (   uart1Dri,FrameWakeup, sizeof(FrameWakeup), &dwDone, 50);
	Serial232Read       (   uart1Dri,FrameWakeup, sizeof(FrameWakeup), &dwDone, 50);
//    rt_thread_mdelay(300);

 
    //if(CBR_115200   ==  baudrate)
        pBufferIn[0]    =   0x04;
   

    uiCmd   =   0x10;
    uiBufferInSize  =   1;

    /* Preambule */
    FrameInOut[0] = 0x00;
    /* SYNC bytes */
    FrameInOut[1] = 0x00;
    FrameInOut[2] = 0xFF;
    /* LEN */
    FrameInOut[3] = (uint8_t)(uiBufferInSize + 2);
    /* LCS */
    FrameInOut[4] = (uint8_t)(0x00 - FrameInOut[3]);
    /* TFI */
    FrameInOut[5] = 0xD4;
    /* Command Tama */
    FrameInOut[6] = uiCmd;
    /* Data */
    memcpy( FrameInOut+7, pBufferIn, uiBufferInSize );
    /* DCS */
    for( i=5; i<uiBufferInSize+7; i++ )
        sum = (uint16_t)(sum + FrameInOut[i]);
    FrameInOut[uiBufferInSize+7] = (uint8_t)(0x00 - sum);
    /* Postambule */
    FrameInOut[uiBufferInSize+8] = 0x00;


    if(!SendSerialCommand( &FrameInOut[0], (uint16_t)(uiBufferInSize + 9), can_communicate))
    {
        return FALSE;
    }

    if(     0xD5    !=  FrameInOut[5]
        ||  0x11    !=  FrameInOut[6])
    {
        return FALSE;
    }

    /*  We have a SetbaudRate response.
        Now, send our ACK, so that TAMA really does the baudrate change.
    */
//    if(!Serial232Write(NULL, handle, &FrameAck[0], sizeof(FrameAck), &dwDone, T_RS232_RETRY, T_RS232_TIMEOUT_DEFAULT))
//    {
//        return FALSE;
//    }
	
	phDalNfc_Write( 0,  &FrameAck[0], sizeof(FrameAck), 0);

    return TRUE;
}





