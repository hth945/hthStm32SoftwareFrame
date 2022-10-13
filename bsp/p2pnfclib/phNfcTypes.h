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

/*
 * \file  phNfcTypes.h
 * \brief Basic type definitions.
 *
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:37:56 2007 $
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $
 *
 */

#ifndef PHNFCTYPES /* */
#define PHNFCTYPES /* */

#ifndef PH_FRINFC_EXCLUDE_FROM_TESTFW /* */

/**
 *  \name NFC Types
 *
 * File: \ref phNfcTypes.h
 *
 */
/*@{*/
#define PHNFCTYPES_FILEREVISION "$Revision: 1.1 $" /**< \ingroup grp_file_attributes */
#define PHNFCTYPES_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $"     /**< \ingroup grp_file_attributes */
/*@}*/

#endif /* PH_FRINFC_EXCLUDE_FROM_TESTFW */

#ifndef _WIN32
#include <stdint.h>
#endif

#ifndef linux
/**
 *  \name Basic Type Definitions
 *
 *  Constant-length-type definition ('C99).
 *
 */
/*@{*/
#ifndef __int8_t_defined /* */
#define __int8_t_defined /* */
typedef signed   char   int8_t;         /**<  \ingroup grp_nfc_common
                                               8 bit signed integer */
#endif

#ifndef __int16_t_defined /* */
#define __int16_t_defined /* */
typedef signed   short  int16_t;        /**< \ingroup grp_nfc_common
                                             16 bit signed integer */
#endif

#ifndef __stdint_h
#ifndef __int32_t_defined /* */
#define __int32_t_defined /* */
typedef signed   long   int32_t;        /**< \ingroup grp_nfc_common
                                             32 bit signed integer */
#endif
#endif

#ifndef __uint8_t_defined /* */
#define __uint8_t_defined /* */
typedef unsigned char   uint8_t;        /**<  \ingroup grp_nfc_common
                                              8 bit unsigned integer */
#endif

#ifndef __uint16_t_defined /* */
#define __uint16_t_defined /* */
typedef unsigned short  uint16_t;       /**< \ingroup grp_nfc_common
                                             16 bit unsigned integer */
#endif

#ifndef __stdint_h
#ifndef __uint32_t_defined /* */
#define __uint32_t_defined /* */
typedef unsigned long   uint32_t;       /**< \ingroup grp_nfc_common
                                             32 bit unsigned integer */
#endif
#endif

#endif /* linux */

typedef uint16_t        NFCSTATUS;      /**< \ingroup grp_nfc_common
                                             NFC-FRI and HAL return values */

/*@}*/

#endif /* PHNFCTYPES */

