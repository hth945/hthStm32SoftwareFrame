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


/**
 * \file phTalTama.h
 * \brief TAL Function Prototypes
 *
 *  This API allows the HAL layer to use the PN531 NFC chip with the same API
 *  than HAL itself. The implementation is dedicated to the PN531.
 *  
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:39:32 2007 $ 
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $
 *
 */


#ifndef PHTALTAMA_H
#define PHTALTAMA_H

/**
 *  \name TAL
 * 
 *  File: \ref phTalTama.h
 * 
 */
/*@{*/
#define PHTALTAMA_FILEREVISION "$Revision: 1.1 $"          /**< \ingroup grp_file_attributes */
#define PHTALTAMA_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $"   /**< \ingroup grp_file_attributes */
/*@}*/

#include "phNfcTypes.h"
#include "phNfcCompId.h"
#include "phNfcHalTypes.h"

        

/**
 * \ingroup grp_tal_common 
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Open
 *
 * \copydoc phHalNfc_Open
 *   
 */

extern NFCSTATUS phTalTama_Open(phHal_sHwReference_t *psHwReference);

                                                                                             

/** \ingroup grp_tal_nfci
 * 
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_GetDeviceCapabilities
 *
 * \copydoc phHalNfc_GetDeviceCapabilities
 *   
 */

extern NFCSTATUS phTalTama_GetDeviceCapabilities( phHal_sHwReference_t          *psHwReference,
                                                  phHal_sDeviceCapabilities_t   *psDevCapabilities);


/** \ingroup grp_tal_nfc
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Poll
 *
 * \copydoc phHalNfc_Poll
 *   
 */
extern NFCSTATUS phTalTama_Poll( phHal_sHwReference_t            *psHwReference,
                                 phHal_eOpModes_t                 OpModes[],
                                 phHal_sRemoteDevInformation_t    psRemoteDevInfoList[],
                                 uint8_t                         *pNbrOfRemoteDev,
                                 phHal_sDevInputParam_t          *psDevInputParam);


/** \ingroup grp_tal_nfci
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Connect
 *
 * \copydoc phHalNfc_Connect
 *   
 */
extern NFCSTATUS phTalTama_Connect(phHal_sHwReference_t           *psHwReference,
                                   phHal_eOpModes_t                OpMode,
                                   phHal_sRemoteDevInformation_t  *psRemoteDevInfo,
                                   phHal_sDevInputParam_t         *psDevInputParam);


/** \ingroup grp_tal_nfci
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Transceive
 *
 * \copydoc phHalNfc_Transceive
 *   
 */
extern NFCSTATUS phTalTama_Transceive(phHal_sHwReference_t           *psHwReference,
                                      phHal_sRemoteDevInformation_t  *psRemoteDevInfo,
                                      phHal_uCmdList_t                Cmd,
                                      phHal_sDepAdditionalInfo_t     *psDepAdditionalInfo,
                                      uint8_t                        *pSendBuf,
                                      uint16_t                        SendLength,
                                      uint8_t                        *pRecvBuf,
                                      uint16_t                       *pRecvLength);



/** \ingroup grp_tal_nfci
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Disconnect
 *
 * \copydoc phHalNfc_Disconnect
 *   
 */
extern NFCSTATUS phTalTama_Disconnect(phHal_sHwReference_t          *psHwReference,
                                      phHal_sRemoteDevInformation_t *psRemoteDevInfo);



/** \ingroup grp_tal_common
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Ioctl
 *
 * \copydoc phHalNfc_Ioctl
 *                
 */
extern NFCSTATUS phTalTama_Ioctl(phHal_sHwReference_t     *psHwReference,
                                 uint16_t                  IoctlCode,
                                 uint8_t                  *pInBuf,
                                 uint16_t                  InLength,
                                 uint8_t                  *pOutBuf,
                                 uint16_t                 *pOutLength);



/** 
 * \ingroup grp_tal_common
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Close
 *
 * \copydoc phHalNfc_Close
 *   
 */
extern NFCSTATUS phTalTama_Close(phHal_sHwReference_t *psHwReference);


/** 
 *  \ingroup grp_tal_nfct     
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_StartTargetMode
 *
 * \copydoc phHalNfc_StartTargetMode
 *   
 */
extern NFCSTATUS phTalTama_StartTargetMode(phHal_sHwReference_t       *psHwReference,
                                           phHal_sTargetInfo_t        *pTgInfo,
                                           phHal_eOpModes_t            OpModes[],
                                           uint8_t                    *pConnectionReq,
                                           uint8_t                    *pConnectionReqBufLength );


/** 
 *  \ingroup grp_tal_nfct
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Receive
 *
 * \copydoc phHalNfc_Receive
 *   
 */
extern NFCSTATUS phTalTama_Receive( phHal_sHwReference_t        *psHwReference,
                                    phHal_sDepAdditionalInfo_t  *psDepAdditionalInfo,
                                    uint8_t                     *pRecvBuf,
                                    uint16_t                    *pRecvLength);


/** 
 *  \ingroup grp_tal_nfct
 *
 * This function has to implemented by the TAL. It is wrapped by the corresponding HAL
 * function and behaves exactly the same way as described there.
 *
 * 
 * \sa phHalNfc_Send
 *
 * \copydoc phHalNfc_Send
 *   
 */
extern NFCSTATUS phTalTama_Send( phHal_sHwReference_t        *psHwReference,
                                 phHal_sDepAdditionalInfo_t  *psDepAdditionalInfo,
                                 uint8_t                     *pSendBuf,
                                 uint16_t                     SendLength);

#endif

