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
 * \file  phNfcStatus.h
 * \brief NFC Status Values - Function Return Codes
 *
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:37:56 2007 $
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $
 *
 */

#ifndef PHNFCSTATUS /* */
#define PHNFCSTATUS /* */

#ifndef PH_FRINFC_EXCLUDE_FROM_TESTFW /* */

/**
 *  \name NFC Status
 *
 * File: \ref phNfcStatus.h
 *
 */
/*@{*/
#define PHNFCSTATUS_FILEREVISION "$Revision: 1.1 $" /**< \ingroup grp_file_attributes */
#define PHNFCSTATUS_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $"      /**< \ingroup grp_file_attributes */
/*@}*/

#endif /* PH_FRINFC_EXCLUDE_FROM_TESTFW */

#include <phNfcTypes.h>

/* Internally required by \ref PHNFCSTVAL. */
#define PHNFCSTSHL8                          (8)
/* Required by \ref PHNFCSTVAL. */
#define PHNFCSTBLOWER                        ((NFCSTATUS)(0x00FF))


/**
 *  \name NFC Status Composition Macro
 *
 *  This is the macro which must be used to compose status values.
 *
 *  \param[in] phNfcCompID Component ID, as defined in \ref phNfcCompId.h .
 *  \param[in] phNfcStatus Status values, as defined in \ref phNfcStatus.h .
 *
 *  \note The macro is not required for the \ref NFCSTATUS_SUCCESS value. This is the only
 *        return value to be used directly.
 *        For all other values it shall be used in assigment and conditional statements e.g.:
 *        - NFCSTATUS status = PHNFCSTVAL(phNfcCompID, phNfcStatus); ...
 *        - if (status == PHNFCSTVAL(phNfcCompID, phNfcStatus)) ...
 *
 */
#define PHNFCSTVAL(phNfcCompID, phNfcStatus)                                \
            ( ((phNfcStatus) == (NFCSTATUS_SUCCESS)) ? (NFCSTATUS_SUCCESS) :  \
                ( (((NFCSTATUS)(phNfcStatus)) & (PHNFCSTBLOWER)) |          \
                    (((uint16_t)(phNfcCompID)) << (PHNFCSTSHL8)) ) )     /**< \ingroup grp_retval
                                                                              Use this macro for return value composition. */

/**
 *  \name Status Codes
 *
 *  Status codes for all NFC components. Combined with the Component ID
 *  they build the value (status) returned by each
 *  function. E.g.:\n
 *  <UL>
 *      <LI> \ref grp_comp_id "Component ID" -  e.g. 0x10, plus                 </LI>
 *      <LI> status code as listed in this file - e.g. 0x03                     </LI>
 *  </UL>
 *  result in a status value of 0x0003.
 *
 *  \note The \ref grp_comp_id "Component ID" used for return codes
     must not be \ref CID_NFC_NONE
 *        except for the value \ref NFCSTATUS_SUCCESS.
 */
/*@{*/

/** \ingroup grp_retval
    The function indicates successful completion. */
#define NFCSTATUS_SUCCESS                                       (0x0000)

/** \ingroup grp_retval
    At least one paramter could not be properly interpreted. */
#define NFCSTATUS_INVALID_PARAMETER                             (0x0001)

/** \ingroup grp_retval
    The function/command has been aborted. */
#define NFCSTATUS_CMD_ABORTED                                   (0x0002)

/** \ingroup grp_retval
    The buffer provided by the caller is too small. */
#define NFCSTATUS_BUFFER_TOO_SMALL                              (0x0003)

/** \ingroup grp_retval
 *  Repeated call of the connecting function is not allowed.
 *   \if hal
 *     \sa \ref phHalNfc_Connect
 *   \endif */
#define NFCSTATUS_ALREADY_CONNECTED                             (0x0004)

/** \ingroup grp_retval
 *  Calling the polling function is not allowed when remote device
 *  are allready connected.
 *   \if hal
 *     \sa \ref phHalNfc_Poll
 *   \endif */
#define NFCSTATUS_MULTI_POLL_NOT_SUPPORTED                      (0x0005)

/** \ingroup grp_retval
    Device specifier/handle value is invalid for the operation. */
#define NFCSTATUS_INVALID_DEVICE                                (0x0006)

/** \ingroup grp_retval
    The device has received a request which is
    (currently) not valid. */
#define NFCSTATUS_INVALID_DEVICE_REQUEST                        (0x0007)

/** \ingroup grp_retval
    The function executed successfully but could have returned
    more information than space provided by the caller. */
#define NFCSTATUS_MORE_INFORMATION                              (0x0008)

/** \ingroup grp_retval
    No response from the remote device received: Time-out.*/
#define NFCSTATUS_RF_TIMEOUT                                    (0x0009)

/** \ingroup grp_retval
    No target found after poll.*/
#define NFCSTATUS_NO_DEVICE_FOUND                               (0x000A)

/** \ingroup grp_retval
    Attempt to disconnect a not connected remote device. */
#define NFCSTATUS_NO_DEVICE_CONNECTED                           (0x000B)

/** \ingroup grp_retval
    Not enough resources (e.g. allocation failed.). */
#define NFCSTATUS_INSUFFICIENT_RESOURCES                        (0x000C)

/** \ingroup grp_retval
    A non-blocking function returns this immediately to indicate
    that an internal operation is in progress. */
#define NFCSTATUS_PENDING                                       (0x000D)

/** \ingroup grp_retval
    External RF field detected. */
#define NFCSTATUS_EXTERNAL_RF_DETECTED                          (0x000E)

/** \ingroup grp_retval
    A board communication error occurred
    (e.g. configuration went wrong). */
#define NFCSTATUS_BOARD_COMMUNICATION_ERROR                     (0x000F)

/** \ingroup grp_retval
    Message is not allowed by the state machine
    (e.g. configuration went wrong). */
#define NFCSTATUS_MSG_NOT_ALLOWED_BY_FSM                        (0x0010)

/** \ingroup grp_retval
    A new semaphore could not be created due to
    a system error. */
#define NFCSTATUS_SEMAPHORE_CREATION_ERROR                      (0x1010) /* GK : Not sure whether to discontinue */

/** \ingroup grp_retval
    The given semaphore could not be released due to
    a system error or invalid handle. */
#define NFCSTATUS_SEMAPHORE_PRODUCE_ERROR                       (0x1011) /* GK : Not sure whether to discontinue */

/** \ingroup grp_retval
    The given semaphore could not be consumed due to a
    system error or invalid handle. */
#define NFCSTATUS_SEMAPHORE_CONSUME_ERROR                       (0x1012) /* GK : Not sure whether to discontinue */

/*  Harsha: Fixed the Mantis entry 0000359: Too many error codes
    Removed the un-used ones and replaced a few with commonly
    used errors */

/* NDEF Mapping error codes */
/** \ingroup grp_retval
    Smart tag functionality not supported */
#define NFCSTATUS_SMART_TAG_FUNC_NOT_SUPPORTED                  (0x1013)

/** \ingroup grp_retval
    Read operation failed */
#define NFCSTATUS_READ_FAILED                                   (0x1014)

/** \ingroup grp_retval
    Write operation failed */
#define NFCSTATUS_WRITE_FAILED                                  (0x1015)

/** \ingroup grp_retval
    Non Ndef Compliant */
#define NFCSTATUS_NO_NDEF_SUPPORT                               (0x1016)

/** \ingroup grp_retval
    No registry node matches the specified input data. */
#define NFCSTATUS_NODE_NOT_FOUND                                (0x1017)

/** \ingroup grp_retval
    A DESELECT event has occurred. */
#define NFCSTATUS_DESELECTED                                    (0x1018)

/** \ingroup grp_retval
    A RELEASE event has occurred. */
#define NFCSTATUS_RELEASED                                      (0x1019)

/** \ingroup grp_retval
    Could not proceed further with the write operation: reached card EOF*/
#define NFCSTATUS_EOF_NDEF_CONTAINER_REACHED                    (0x101A)

/** \ingroup grp_retval
    Incorrect number of bytes received from the card*/
#define NFCSTATUS_INVALID_RECEIVE_LENGTH                        (0x101B)

/** \ingroup grp_retval
    The data format/composition is not understood/correct. */
#define NFCSTATUS_INVALID_FORMAT                                (0x101C)

/** \ingroup grp_retval
    The remote device (type) is not valid for this request. */
#define NFCSTATUS_INVALID_REMOTE_DEVICE                         (0x101D)

/** \ingroup grp_retval
    No access has been granted. */
#define NFCSTATUS_ACCESS_DENIED                                 (0x101E)

/** \ingroup grp_retval
    There is not sufficient storage available. */
#define NFCSTATUS_INSUFFICIENT_STORAGE                          (0x101F)

/** \ingroup grp_retval
    Command not supported . */
#define NFCSTATUS_COMMAND_NOT_SUPPORTED                         (0x1020)

/** \ingroup grp_retval
    The power level set in the ADD component is too low to 
    execute any further phase. 
    One or more phases in the sequence might have been executed by now.*/
#define NFCSTATUS_PWR_LVL_LOW_TO_EXE_ANY_PHASE_FURTHER          (0x1021)

/** \ingroup grp_retval
    The power level set in the ADD component is too low to 
    execute any phase. 
    No phase is executed in the sequence. */
#define NFCSTATUS_PWR_LVL_LOW_TO_EXE_ANY_PHASE                  (0x1022)

/** \ingroup grp_retval
    The Ndef Format procedure has failed. */
#define NFCSTATUS_FORMAT_ERROR                                  (0x1023)


/** \ingroup grp_retval
    The current module is busy ; one might retry later */
#define NFCSTATUS_SMX_BAD_STATE                          (0x10F0)

/** \ingroup grp_retval
    The system is not in the good mode to support this command */
#define NFCSTATUS_INVALID_BUSY                           (0x10F1)


/** \ingroup grp_retval
    The Abort mechanism has failed for unexpected reason: user can try again*/
#define NFCSTATUS_ABORT_FAILED                           (0x10F2)


/** \ingroup grp_retval
    The Registration command has failed because the user wants to register on 
    an element for which he is already registered*/
#define NFCSTATUS_ALREADY_REGISTERED                     (0x10F3)


/** \ingroup grp_retval
    The Unregistration command has failed because the user wants to unregister on 
    an element for which he was not registered*/
#define NFCSTATUS_NOT_REGISTERED                         (0x10F4)


/** \ingroup grp_retval
    The Registration command has failed because the user wants to register as target
    on a operating mode not supported*/
#define NFCSTATUS_REG_OPMODE_NOT_SUPPORTED               (0x10F5)

/** \ingroup grp_retval
    Max number of Status Codes*/
#define NFCSTATUS_MAX_VALUE                              (0x10F6)



/*@}*/

#endif /* PHNFCSTATUS */
