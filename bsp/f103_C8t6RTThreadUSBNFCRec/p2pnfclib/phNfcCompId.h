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
 * \file phNfcCompId.h
 * \brief NFC Component ID Values - Used for Function Return Codes
 *
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:37:55 2007 $
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $
 *
 */

#ifndef PHNFCCOMPID_H  /* */
#define PHNFCCOMPID_H  /* */

#ifndef PH_FRINFC_EXCLUDE_FROM_TESTFW /* */

/**
 *  \name NFC Comp. ID
 *
 * File: \ref phNfcCompId.h
 *
 */
/*@{*/
#define PHNFCCOMPID_FILEREVISION "$Revision: 1.1 $" /**< \ingroup grp_file_attributes */
#define PHNFCCOMPID_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $"     /**< \ingroup grp_file_attributes */
/*@}*/

#endif /* PH_FRINFC_EXCLUDE_FROM_TESTFW */

/**
 *  \ingroup grp_comp_id
 *
 *  \name Component IDs
 *
 *  IDs for all NFC components. Combined with the Status Code they build the value (status)
 *  returned by each function.
 *
 *  ID Number Spaces:
 *  - 01..1F: HAL
 *  - 20..3F: NFC-MW (Local Device)
 *  - 40..5F: NFC-MW (Remote Device)
 *  .
 *
 *  \note The value \ref CID_NFC_NONE does not exist for Component IDs. Do not use this value except
 *         for \ref NFCSTATUS_SUCCESS. The enumeration function uses \ref CID_NFC_NONE
 *         to mark unassigned \ref phHal_sHwReference_t "Hardware References".
 *
 *  \if hal
 *   \sa \ref phHalNfc_Enumerate
 *  \endif
 */
/*@{*/
#define CID_NFC_NONE                    0x00    /**< \ingroup grp_comp_id
                                                     Unassigned or doesn't apply (see \ref NFCSTATUS_SUCCESS). */
#define CID_NFC_DAL                     0x01    /**< \ingroup grp_comp_id
                                                     Driver Adaption Layer \if hal (\ref grp_subcomponents) \endif .   */
#define CID_NFC_TAL                     0x02    /**< \ingroup grp_comp_id
                                                     PN531 Abstraction Layer \if hal (\ref grp_subcomponents) \endif . */
#define CID_NFC_JAL                     0x03    /**< \ingroup grp_comp_id
                                                     PN511 Abstraction Layer \if hal (\ref grp_subcomponents) \endif . */
#define CID_NFC_HAL                     0x10    /**< \ingroup grp_comp_id
                                                     Hardware Abstraction Layer \if hal (\ref grp_hal_common) \endif . */

#define CID_FRI_NFC_OVR_HAL             0x20    /**< \ingroup grp_comp_id
                                                     NFC-Device, HAL-based. */
#define CID_FRI_NFC_MAN_DEV_DIS         0x21    /**< \ingroup grp_comp_id
                                                     Manual Device Discovery. */
#define CID_FRI_NFC_NDEF_RECORD         0x22    /**< \ingroup grp_comp_id
                                                     NDEF Record Tools Library. */
#define CID_FRI_NFC_NDEF_MAP            0x23    /**< \ingroup grp_comp_id
                                                     NDEF Mapping. */
#define CID_FRI_NFC_NDEF_REGISTRY       0x24    /**< \ingroup grp_comp_id
                                                     NDEF_REGISTRY. */
#define CID_FRI_NFC_AUTO_DEV_DIS        0x25    /**< \ingroup grp_comp_id
                                                     Automatic Device Discovery. */
#define CID_FRI_NFC_NDEF_SMTCRDFMT      0x26    /**< \ingroup grp_comp_id
                                                     Smart Card Formatting */
#define CID_MAX_VALUE                   0xF0    /**< \ingroup grp_comp_id
                                                     The maximum CID value that is defined. */

/*@}*/

#endif /* PHNFCCOMPID_H */
