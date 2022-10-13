/*
 *         Copyright (c), NXP Semiconductors
 *
 *                       (C)NXP Electronics N.V.2004
 *         All rights are reserved. Reproduction in whole or in part is
 *        prohibited without the written consent of the copyright owner.
 *    NXP reserves the right to make changes without notice at any time.
 *   NXP makes no warranty, expressed, implied or statutory, including but
 *   not limited to any implied warranty of merchantability or fitness for any
 *  particular purpose, or that the use will not infringe any third party patent,
 *   copyright or trademark. NXP must not be liable for any loss or damage
 *                            arising from its use.
 *
 */
#include "stdio.h"

/*!
 * \file phHalNfc.c
 * \brief HAL Function implementation
 *
 *  This API allows the user to have a common interface for existing NFC chips
 *  such as PN511 and PN531. The system is prepared to implement further NFC
 *  chips in the future.
 *
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:38:38 2007 $ 
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $
 *
 */
#ifdef PHHAL_VERSION_ACQUISITION
    #define PHHALNFC_SOURCE_FILEREVISION "$Revision: 1.1 $"
    #define PHHALNFC_SOURCE_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $"
#else /* PHHAL_VERSION_ACQUISITION */

#if ((! defined PHJALJOINER) && (! defined PHTALTAMA))
    #define PHJALJOINER 
    #define PHTALTAMA
#endif

#include <stdio.h>
#include <phHalNfc.h>
#include <phNfcStatus.h>
#include <phNfcIoctlCode.h>

/*
 * These include files shall by added by the maintenance programmer to support further
 * devices.
 */
#ifdef PHTALTAMA
    #include <phTalTama.h>
#endif

#ifdef PHJALJOINER 
    #include <phJalJoiner.h>
#endif


    #include <phDalNfc.h>



/* For documentation, see header file
 *  
 */
//extern NFCSTATUS phHalNfc_Enumerate(phHal_sHwReference_t  sHwReference[],
//                                    uint8_t               *pNbrOfDevDetected)
//{
//    NFCSTATUS   result = NFCSTATUS_SUCCESS;
//    uint8_t     nbOfDevDetected;

//    if (    NULL    ==  sHwReference 
//        ||  NULL    ==  pNbrOfDevDetected
//        ||  0       ==  *pNbrOfDevDetected)
//    {
//        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
//    }
//    else
//    {
//            result    =   phDalNfc_Enumerate(sHwReference, pNbrOfDevDetected);
//            /* Resetting some general values: */
//            if(result == NFCSTATUS_SUCCESS)
//            {
//                for (nbOfDevDetected = 0; nbOfDevDetected < *pNbrOfDevDetected; nbOfDevDetected++)
//                {
//                    sHwReference[nbOfDevDetected].UserTimeout  = PHHALNFC_PN5XX_DEFAULT_USER_TIMEOUT;
//                    sHwReference[nbOfDevDetected].Flags.AbortAllowed = 0;
//                }
//            }
//    }


//    return result;
//}


/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Open(phHal_sHwReference_t *psHwReference)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;

    if (psHwReference == NULL)
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if (psHwReference->Flags.LinkValid)
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_Open(psHwReference);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_Open(psHwReference);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                    break;
            }
        }
    }

    return result;
}

                                                                                             

/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_GetDeviceCapabilities(phHal_sHwReference_t          *psHwReference,
                                                phHal_sDeviceCapabilities_t   *psDevCapabilities)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;

    if ((psHwReference == NULL) || (psDevCapabilities == NULL))
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if ((psHwReference->Flags.LinkValid == 0))
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_GetDeviceCapabilities(psHwReference,
                                                                   psDevCapabilities);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_GetDeviceCapabilities(psHwReference,
                                                                 psDevCapabilities);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                    break;
            }
        }
    }
    return result;
}


/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Poll(phHal_sHwReference_t           *psHwReference,
                               phHal_eOpModes_t                OpModes[],
                               phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                               uint8_t                        *pNbrOfRemoteDev,
                               phHal_sDevInputParam_t         *psDevInputParam)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    phHal_sDeviceCapabilities_t devCapabilities;
    uint8_t i = 0;
    
    if((psHwReference == NULL))
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
    }

    if(psHwReference->Flags.LinkValid == 0)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);
    
    if( OpModes == NULL 
        || psRemoteDevInfoList == NULL 
        || pNbrOfRemoteDev == NULL 
        || psDevInputParam == NULL 
        || (*pNbrOfRemoteDev == 0)
        || OpModes[0] == phHal_eOpModesArrayTerminator)
    {
        return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } 

	if ((psHwReference->Flags.LinkValid == 0))
    {
        *pNbrOfRemoteDev = 0;
        return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
    } 

	if(phHalNfc_GetDeviceCapabilities (psHwReference, &devCapabilities) != NFCSTATUS_SUCCESS)
	{
		return  PHNFCSTVAL(CID_NFC_HAL,NFCSTATUS_INVALID_DEVICE);
	}

	/* Check if the OpModes are supported by the hardware device */
/* Mantis entry 713 - DBO */
/* 
	i = 0;
	while(OpModes[i] != phHal_eOpModesArrayTerminator)
	{
		switch (OpModes[i])
		{
			case phHal_eOpModesNfcActive106:
			case phHal_eOpModesNfcActive212:
			case phHal_eOpModesNfcActive424:
			case phHal_eOpModesNfcPassive106:
			case phHal_eOpModesNfcPassive212:
			case phHal_eOpModesNfcPassive424:
				if (devCapabilities.InitiatorSupProtocol.NFC != 1)
					return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
				break;

			case phHal_eOpModesFelica212:
			case phHal_eOpModesFelica424:
				if (devCapabilities.InitiatorSupProtocol.Felica != 1)
					return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
				break;

			case phHal_eOpModesMifare:
				if ((devCapabilities.InitiatorSupProtocol.MifareStd != 1) &&
					(devCapabilities.InitiatorSupProtocol.MifareUL != 1))
					return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
				break;

			case phHal_eOpModesISO14443_4A:
				if (devCapabilities.InitiatorSupProtocol.ISO14443_4A != 1)
					return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
				break;

			case phHal_eOpModesISO14443_4B:
				if (devCapabilities.InitiatorSupProtocol.ISO14443_4B != 1)
					return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
				break;

			case phHal_eOpModesJewel:
				if (devCapabilities.InitiatorSupProtocol.Jewel != 1)
					return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
				break;

			default:
				return  PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
				break;
		}
		i++;
	}

*/
    switch (psHwReference->CompType)
    {
	    case CID_NFC_JAL:
        #ifdef PHJALJOINER 
		    result = phJalJoiner_Poll(psHwReference,
									  OpModes,
                                      psRemoteDevInfoList,
                                      pNbrOfRemoteDev,
                                      psDevInputParam);
		#endif
			break;

		case CID_NFC_TAL:
        #ifdef PHTALTAMA
            result = phTalTama_Poll(psHwReference,
                                    OpModes,
                                    psRemoteDevInfoList,
                                    pNbrOfRemoteDev,
                                    psDevInputParam);
        #endif
            break;

		default:
            /* No component match: */
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
            break;
    }

	return result;
}


/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Connect(phHal_sHwReference_t          *psHwReference,
                                  phHal_eOpModes_t               OpMode,
                                  phHal_sRemoteDevInformation_t *psRemoteDevInfo,
                                  phHal_sDevInputParam_t        *psDevInputParam)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;

    if ((psHwReference == NULL) ||
         (psRemoteDevInfo == NULL) ||
         (psDevInputParam == NULL))
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else   if((psRemoteDevInfo->OpMode != OpMode) 
              || (OpMode == phHal_eOpModesInvalid)
              || (psRemoteDevInfo->OpMode  == phHal_eOpModesInvalid))
    {
        return  PHNFCSTVAL(CID_NFC_HAL,NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if ((psHwReference->Flags.LinkValid == 0))
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_Connect(psHwReference,
                                                    OpMode,
                                                    psRemoteDevInfo,
                                                    psDevInputParam);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_Connect(psHwReference,
                                                OpMode,
                                                psRemoteDevInfo,
                                                psDevInputParam);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);                          
                    break;
            }
        }
    }
    return result;
}


/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Transceive(phHal_sHwReference_t           *psHwReference,
                                     phHal_sRemoteDevInformation_t  *psRemoteDevInfo,
                                     phHal_uCmdList_t                Cmd,
                                     phHal_sDepAdditionalInfo_t     *psDepAdditionalInfo,
                                     uint8_t                        *pSendBuf,
                                     uint16_t                        SendLength,
                                     uint8_t                        *pRecvBuf,
                                     uint16_t                       *pRecvLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    if ((psRemoteDevInfo == NULL) ||
        (psDepAdditionalInfo == NULL) ||
        (pSendBuf == NULL) ||
        (pRecvBuf == NULL) ||
        (pRecvLength == NULL) ||
        (psHwReference == NULL))
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if (psHwReference->Flags.LinkValid == 0)
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_Transceive(psHwReference,
                                                        psRemoteDevInfo,
                                                        Cmd,
                                                        psDepAdditionalInfo,
                                                        pSendBuf,
                                                        SendLength,
                                                        pRecvBuf,
                                                        pRecvLength);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_Transceive(psHwReference,
                                                    psRemoteDevInfo,
                                                    Cmd,
                                                    psDepAdditionalInfo,
                                                    pSendBuf,
                                                    SendLength,
                                                    pRecvBuf,
                                                    pRecvLength);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                    break;
            }
        }
    }
    return result;
}




/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Disconnect(phHal_sHwReference_t          *psHwReference,
                                     phHal_sRemoteDevInformation_t *psRemoteDevInfo)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    
    if ((psHwReference == NULL) ||
         (psRemoteDevInfo == NULL))
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if ((psHwReference->Flags.LinkValid == 0))
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_Disconnect(psHwReference,
                                                        psRemoteDevInfo);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_Disconnect(psHwReference,
                                                    psRemoteDevInfo);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                    break;
            }
        }
    }
    return result;
}




/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Ioctl(phHal_sHwReference_t     *psHwReference,
                                uint16_t                  IoctlCode,
                                uint8_t                  *pInBuf,
                                uint16_t                  InLength,
                                uint8_t                  *pOutBuf,
                                uint16_t                 *pOutLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    if ((psHwReference == NULL))
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if ((psHwReference->Flags.LinkValid == 0))
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            switch(IoctlCode)
            {
           
                case PHHALNFC_IOCTL_PN5XX_SET_USER_TIMEOUT: 
                    
                    if ((InLength    <  sizeof (uint32_t))    ||
                        (*pOutLength <  sizeof (uint32_t))    ||
                        (pOutBuf     == NULL)                 ||
                        (pInBuf      == NULL))
                    {
                        result = PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_INVALID_PARAMETER);
                    } 
                    else
                    {
                        *(uint32_t*)pOutBuf  = psHwReference->UserTimeout;
                        *pOutLength = sizeof (uint32_t);
                        psHwReference->UserTimeout  =  *(uint32_t*)pInBuf; 
                    }
                    break;

                default:

                    switch (psHwReference->CompType)
                    {
                        case CID_NFC_JAL:
                            #ifdef PHJALJOINER 
                                result = phJalJoiner_Ioctl(psHwReference,
                                                IoctlCode,
                                                pInBuf,
                                                InLength,
                                                pOutBuf,
                                                pOutLength);
                            #endif
                            break;
                        case CID_NFC_TAL:
                            #ifdef PHTALTAMA
                                result = phTalTama_Ioctl(psHwReference,
                                                        IoctlCode,
                                                        pInBuf,
                                                        InLength,
                                                        pOutBuf,
                                                        pOutLength);
                            #endif
                            break;
                        default:
                            /* No component match: */
                            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                            break;
                    }   /*  switch(sHwReference->CompType)  */
            }   /*  switch(IoctlCode)  */
        }
    }

    return result;
}




/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Close(phHal_sHwReference_t *psHwReference)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    
    if (psHwReference == NULL)
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if (psHwReference->Flags.LinkValid == 0)
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_Close(psHwReference);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_Close(psHwReference);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                    break;
            }
        }
    }
    return result;
}




/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_StartTargetMode(phHal_sHwReference_t       *psHwReference,
                                          phHal_sTargetInfo_t        *pTgInfo,
                                          phHal_eOpModes_t            OpModes[],
                                          uint8_t                    *pConnectionReq,
                                          uint8_t                    *pConnectionReqBufLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;

    if ((psHwReference == NULL) ||
         (pTgInfo == NULL) ||
         (pConnectionReq == NULL) ||
         (pConnectionReqBufLength == NULL))
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if ((psHwReference->Flags.LinkValid == 0))
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            /* Allowing ABORT for this function: */
            psHwReference->Flags.AbortAllowed = 1;
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_StartTargetMode(psHwReference,
                                                            pTgInfo,
                                                            OpModes,
                                                            pConnectionReq,
                                                            pConnectionReqBufLength);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_StartTargetMode(psHwReference,
                                                        pTgInfo,
                                                        OpModes,
                                                        pConnectionReq,
                                                        pConnectionReqBufLength);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                    break;
            } /* SWITCH */
            /* Not allowing ABORT for subsequent function. */
            psHwReference->Flags.AbortAllowed = 0;
        }
    }

    return result;
}



/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Receive(phHal_sHwReference_t        *psHwReference,
                                  phHal_sDepAdditionalInfo_t  *psDepAdditionalInfo,
                                  uint8_t                     *pRecvBuf,
                                  uint16_t                    *pRecvLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;

    if ((psHwReference == NULL) ||
         (psDepAdditionalInfo == NULL) ||
         (pRecvBuf == NULL) ||
         (pRecvLength == NULL))
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if ((psHwReference->Flags.LinkValid == 0))
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            /* Allowing ABORT for this function: */
            psHwReference->Flags.AbortAllowed = 1;
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_Receive(psHwReference,
                                                    psDepAdditionalInfo,
                                                    pRecvBuf,
                                                    pRecvLength);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_Receive(psHwReference,
                                                psDepAdditionalInfo,
                                                pRecvBuf,
                                                pRecvLength);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                    break;
            }  /* SWITCH */
            /* Not allowing ABORT for subsequent function. */
            psHwReference->Flags.AbortAllowed = 0;
        }
    }
    return result;
}



/* For documentation, see header file
 *  
 */
extern NFCSTATUS phHalNfc_Send(phHal_sHwReference_t        *psHwReference,
                               phHal_sDepAdditionalInfo_t  *psDepAdditionalInfo,
                               uint8_t                     *pSendBuf,
                               uint16_t                     SendLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;

    if ((psHwReference == NULL) ||
         (psDepAdditionalInfo == NULL) ||
         (pSendBuf == NULL))
    {
        result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_PARAMETER);
    } else
    {
        if ((psHwReference->Flags.LinkValid == 0))
        {
            result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
        } else
        {
            switch (psHwReference->CompType)
            {
                case CID_NFC_JAL:
                    #ifdef PHJALJOINER 
                        result = phJalJoiner_Send(psHwReference,
                                                psDepAdditionalInfo,
                                                pSendBuf,
                                                SendLength);
                    #endif
                    break;
                case CID_NFC_TAL:
                    #ifdef PHTALTAMA
                        result = phTalTama_Send(psHwReference,
                                                psDepAdditionalInfo,
                                                pSendBuf,
                                                SendLength);
                    #endif
                    break;
                default:
                    /* No component match: */
                    result = PHNFCSTVAL(CID_NFC_HAL, NFCSTATUS_INVALID_DEVICE);
                    break;
            }
        }
    }
    return result;
}

#endif /* PHHAL_VERSION_ACQUISITION */
