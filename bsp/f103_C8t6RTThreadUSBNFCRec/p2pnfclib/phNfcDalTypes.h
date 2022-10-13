/*
*               Copyright (c), NXP Semiconductors
*
*                   (C)NXP Electronics N.V.2004 
*        All rights are reserved. Reproduction in whole or in part is
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
 * \file  phNfcDalTypes.h
 * \brief Structure declarations/type definitions belonging to the DAL subsystem.
 *
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:37:56 2007 $ 
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $
 *
 */

#ifndef PHNFCDALTYPES_H /* */
#define PHNFCDALTYPES_H /* */


#ifndef PH_FRINFC_EXCLUDE_FROM_TESTFW /* */

/** 
 * \name DAL Types
 * 
 * File: \ref phNfcDalTypes.h
 * 
 */
/*@{*/
#define PHNFCDALTYPES_FILEREVISION "$Revision: 1.1 $" /**< \ingroup grp_file_attributes */
#define PHNFCDALTYPES_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $"     /**< \ingroup grp_file_attributes */
/*@}*/

#endif /* PH_FRINFC_EXCLUDE_FROM_TESTFW */

#include "phNfcTypes.h"
#include "phNfcCompId.h"

/* + MantisID:7 - JP - 08-12-2005 */
#define PHNFCDALTYPES_LINK_NAME_SIZE 50
/* - MantisID:7 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 * \ingroup grp_hal_common
 *
 * \if hal
 *  \brief Flags Structure Embedded Into the Hardware Reference
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The auxiliary Flags substructure of \ref phHal_sHwReference_t is filled during opening of the currently
 *  connected NFC devices.
 *
 * \note None.
 *
 * \sa phHalNfc_Open .
 */
typedef struct phHal_sHwReferenceFlags  
{
    uint8_t LinkValid    : 1;      /**< Tells whether the link to the peripheral is usable. */
    uint8_t AbortAllowed : 2;      /**< Internal flag, to decide whether the HAL is allowed
                                        to abort a pending operation.
                                        The user must not change this. */
} phHal_sHwReferenceFlags_t;


/* This feature remains undocumented (officially) since it is not of interest for the user. */
typedef enum phHal_eChipName
{
    phHal_eChipNameUndefined		= 0x10000001,		/**<  Undefined				*/
    phHal_eChipName51x				= 0x10000010,		/**<  PN51x, x is unknwon	*/
    phHal_eChipName511				= 0x10000011,		/**<  PN511					*/
    phHal_eChipName512				= 0x10000012,		/**<  PN512					*/
    phHal_eChipName522				= 0x10000022,		/**<  PN522					*/
    phHal_eChipName523				= 0x10000023,		/**<  PN523					*/
    phHal_eChipName53x				= 0x10000030,		/**<  PN53x, x is unknwon	*/
    phHal_eChipName531				= 0x10000031,		/**<  PN531					*/
    phHal_eChipName532				= 0x10000032,		/**<  PN532					*/

} phHal_eChipName_t;

/* This feature remains undocumented (officially) since it is not of interest for the user. */
typedef struct phDal_Context
{
    uint32_t startThread;
    void* evTimeoutStarted;
    void* evCloseThread;
    void* evAbortTimeout;
    void* evReady;
    void* hThread;
    uint32_t dwThreadId;
/*+ MantisID:7 - JP - 08-12-2005 */
    int8_t   sLinkName[PHNFCDALTYPES_LINK_NAME_SIZE];
/*- MantisID:7  */
    uint32_t timeout;
    uint8_t  DeviceNbr;
    void*   AbortEvent;

    void *BflLowerFrame;
} phDal_Context_t;


/**
 * \ingroup grp_hal_common
 *
 * \if hal
 *  \brief Hardware Reference - The Peripheral Representation
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The Hardware Reference structure with its auxiliary Flags substructure is filled during enumeration
 *  listing the currently connected peripheral NFC devices. However, after enumeration the structure is not yet
 *  usable for device access - a device must use the open function first, based on the enumeration data entered
 *  into this compound.
 *
 * \note The caller can consider this structure atomic, no interpretation is required for HAL operation.
 * 
 * \sa phHalNfc_Enumerate phHalNfc_Open .
 * 
 */
typedef struct phHal_sHwReference
{
    void                        *pBoardDriver;      /**<  Will be usable/valid after the Open function. */
    phHal_sHwReferenceFlags_t    Flags;	            /**<  Flag indicating the validity of the peripheral link */
    uint8_t                      Sleeping;          /**<  Flag indicating if the chip is in power down mode */
    uint8_t                      CompType;          /**<  See \ref phNfcCompId.h */
    uint8_t                      LinkType;          /**<  Serial, USB, I2C, ... */
    uint8_t                      DeviceNbr;         /**<  Number of the device on the bus. */
    uint8_t						 ChipRevision;		/**<  The revision byte of the PN511. */
    phHal_eChipName_t			 ChipName;			/**<  The Chip name PN5xx. */
    uint8_t						 FirmwareVersion;	/**<  The version byte of the FIRMWARE, if any. */
    uint8_t						 FirmwareRevision;	/**<  The revision byte of the FIRMWARE, if any. */
    uint32_t                     UserTimeout;       /**<  Timeout value (in msec) used while in Target Mode, within
                                                          \ref phHalNfc_Send, \ref phHalNfc_Receive
                                                          and \ref phHalNfc_Transceive functions.*/
    phDal_Context_t              DalContext;        /**<  For additional information. */
    
} phHal_sHwReference_t;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#endif 
