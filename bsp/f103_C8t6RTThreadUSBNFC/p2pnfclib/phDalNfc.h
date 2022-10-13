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
 * \file phDalNfc.h
 * \brief DAL Function Prototypes
 *
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:38:39 2007 $ 
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $
 *
 */

#ifndef PHDALNFC_H
#define PHDALNFC_H

/** \ingroup grp_file_attributes
 *  \name DAL
 * 
 *  File: \ref phDalNfc.h
 * 
 */
/*@{*/
#define PHDALNFC_FILEREVISION "$Revision: 1.1 $"               /**< \ingroup grp_file_attributes */
#define PHDALNFC_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $"        /**< \ingroup grp_file_attributes */
/*@}*/


#include "phNfcTypes.h"
#include "phNfcCompId.h"
#include "phNfcStatus.h"
#include "phNfcDalTypes.h"

#ifdef PHJALJOINER 

#include <phcsBflBal.h>
#include <phcsBflRegCtl.h>
#include <phcsBflStatus.h>
#include <phcsBflAux.h>

#endif

/**
 * \ingroup grp_dal_nfc
 *
 * \copydoc phHalNfc_Enumerate
 * 
 * \b MANDATORY: \n
 *  YES
 */
extern NFCSTATUS phDalNfc_Enumerate(phHal_sHwReference_t  sHwReference[],  
                                    uint8_t               *pNbrOfDevDetected );



int TAMABaudrateChange      (  uint8_t* can_communicate);

/**
 * \ingroup grp_dal_nfc
 *
 * \copydoc phHalNfc_Open
 * 
 * \b MANDATORY: \n
 *  YES
 */
extern NFCSTATUS phDalNfc_Open(phHal_sHwReference_t *psHwReference);


/**
 * \ingroup grp_dal_nfc
 *
 * \copydoc phHalNfc_Close
 * 
 * \b MANDATORY: \n
 *  YES
 */
extern NFCSTATUS phDalNfc_Close(phHal_sHwReference_t *psHwReference);



/**
 * \ingroup grp_dal_nfc
 *
 *  The phDalNfc_Write function allows to send data through the corresponding link to the peripheral
 *  device, described by its Hardware Reference.
 *
 *  \param[in]  psHwReference          Hardware Reference of the peripheral device.
 *  \param[in]  pBuffer                Pointer to the buffer containing the data to write.
 *  \param[in]  nNumberOfBytesToWrite  Specifies the number of bytes to write.
 *  \param[out] pNumberOfBytesWritten  Pointer to the variable that receives the number of bytes
 *                                     actually sent.
 *
 *  \retval NFCSTATUS_SUCCESS           Data successfully wrote.
 *  \retval NFCSTATUS_INVALID_PARAMETER One or more of the suplied parameters could not be (properly)
 *                                      interpreted.
 *
 * \b MANDATORY: \n
 *  YES
 */
extern NFCSTATUS phDalNfc_Write(phHal_sHwReference_t *psHwReference,
                                uint8_t              *pBuffer,  
							    uint16_t              nNumberOfBytesToWrite,
							    uint16_t             *pNumberOfBytesWritten);



/**
 *
 * \ingroup grp_dal_nfc
 *
 *  The phDalNfc_Read allows to read data through the corresponding link from the peripheral device,
 *  described by its Hardware Reference.
 *
 *  \param[in]  psHwReference        pHwRef Hardware Reference of the peripheral device.
 *  \param[out] pBuffer              Pointer to the buffer that receives the data read. .
 *  \param[in]  nNumberOfBytesToRead Specifies the number of bytes to read.
 *  \param[out] pNumberOfBytesRead   Pointer to the variable that receives the number of bytes read.
 *  \param[in]  nTimeout             Specifies the timeout value of the Read. The Read function is aborted
 *                                   after this delay. The value <em> zero </em> indicates an infinite timeout.
 *
 *  \retval NFCSTATUS_SUCCESS           Data successfully read.
 *  \retval NFCSTATUS_INVALID_PARAMETER One or more of the suplied paramters could not be properly
 *                                      interpreted.
 *  \retval NFCSTATUS_CMD_ABORTED       The operation has been aborted (timeout, link errors,...).
 *
 * \b MANDATORY: \n
 *  YES
 */
extern NFCSTATUS phDalNfc_Read(phHal_sHwReference_t *psHwReference,
                               uint8_t              *pBuffer,  
							   uint16_t              nNumberOfBytesToRead,
							   uint16_t             *pNumberOfBytesRead,
                               uint32_t              nTimeout);


/**
 * \ingroup grp_dal_nfc
 *
 * \copydoc phHalNfc_Ioctl
 * 
 * \b MANDATORY: \n
 *  NO, this function is used by another chip
 */ 
extern NFCSTATUS phDalNfc_Ioctl(phHal_sHwReference_t    *psHwReference,
                                uint16_t                 IoctlCode,
                                uint8_t                 *pInBuf,
                                uint16_t                 InLength,
                                uint8_t                 *pOutBuf,
                                uint16_t                *pOutLength);


/**
 * \ingroup grp_dal_nfc
 *
 * This function allows to abort a current underlying HAL/DAL I/O operation.
 *
 * \param psHwReference The HAL Hardware Reference.
 * 
 * \retval NFCSTATUS_SUCCESS                The command was successful.
 * \retval NFCSTATUS_INVALID_PARAMETER      Parameter is not valid for the operation.
 * \retval NFCSTATUS_INVALID_DEVICE         The device is not able to abort due to a system error.
 * \retval NFCSTATUS_INVALID_DEVICE_REQUEST The device is in a state where aborting is not allowed.
 *
 * \b MANDATORY: \n
 *  YES
 */ 
extern NFCSTATUS phDalNfc_Abort(phHal_sHwReference_t *psHwReference);

#ifdef PHJALJOINER

phcsBfl_Status_t phDal_WaitForReceptionCallback(phcsBflAux_WaitEventCbParam_t *psParams);

#endif /* PHJALJOINER */



#endif

