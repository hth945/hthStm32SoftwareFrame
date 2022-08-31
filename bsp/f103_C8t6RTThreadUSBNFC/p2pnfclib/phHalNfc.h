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
 * \file phHalNfc.h
 * \brief HAL Function Prototypes
 *
 *  This API allows the user to have a common interface for existing NFC chips
 *  such as PN511 and PN531. The system is prepared to implement further NFC
 *  chips in the future.
 *
 * \note This is the representative header file of the HAL. The release TAG or
 *       label is representing the release TAG (alias) of the entire library.
 *       A mechanism (see documentation \ref hal_release_label near the include guards
 *       of this file) is used to propagate the alias to the main documentation page.
 *
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:38:03 2007 $
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $
 *
 */

/** \page hal_release_label HAL Release Label
 *  $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $
 *  \note This is the TAG (label, alias) of the HAL. If the string is empty, the current documentation
 *        has not been generated from an official release.
 */
/*@{*/
#ifndef PHHALNFC_H
#define PHHALNFC_H
/*@}*/


/** 
 *  \name HAL
 *
 * File: \ref phHalNfc.h
 *
 */
/*@{*/
#define PHHALNFC_FILEREVISION "$Revision: 1.1 $" /**< \ingroup grp_file_attributes */
#define PHHALNFC_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $"     /**< \ingroup grp_file_attributes */
/*@}*/


#include "phNfcTypes.h"
#include "phNfcCompId.h"

#include "phNfcHalTypes.h"


/**
 *  \if hal
 *   \ingroup grp_hal_common
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  This function allows the user to receive a list of devices connected to the system.
 *  The function itself does not open handles to the devices. It only returns information
 *  about them and their availability.
 *
 *  \note
 *  - In fixed configurations (mostly embedded systems) the output of this function never changes.
 *  - It is up to the system integrator to provide a correct implementation within the
 *    "Driver Abstraction Layer (DAL)".
 *
 *  \param[in,out] sHwReference  Array of uninitialised Hardware Reference structures
 *                 which are allocated by the caller (user application, upper layer).
 *                 Each caller-provided structure (array element) represents one
 *                 peripheral device. The function fills the array until no further
 *                 device is detected or the array boundary is reached. \n
 *                 Those HW References which have not been initialised get the
 *                 \ref grp_comp_id "Component ID" \ref CID_NFC_NONE in order to
 *                 unambiguously mark it non-assigned.
 *
 *  \param[in,out] pNbrOfDevDetected The caller is responsible to specify the maximum number of
 *                 devices. When the function returns it yields the current
 *                 number of devices found with this parameter. \n
 *                 Caution: When the caller provides less array spaces than
 *                 devices found the function fills all array elements with
 *                 device information and returns the <em> actual </em> number
 *                 of devices found in this parameter (which in this
 *                 case exceeds the number provided by the caller).\n
 *                 Additionally the status is set to indicate that more devices
 *                 are available.
 *
 *  \retval  NFCSTATUS_SUCCESS          Enumeration successful.
 *  \retval  NFCSTATUS_INVALID_PARAMETER One or more of the supplied parameters could not be properly
 *                                      nterpreted. In particular, this error is generated when the
 *                                      number of devices parameter is initialised to zero by the
 *                                      caller or the HW-Ref. array parameter points to NULL.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 *  \if hal
 *   \sa \ref phDalNfc_Enumerate, \ref grp_dal_nfc "DAL"
 *  \endif
 */
extern NFCSTATUS phHalNfc_Enumerate( phHal_sHwReference_t  sHwReference[],
                                     uint8_t               *pNbrOfDevDetected);


/**
 *  \if hal
 *   \ingroup grp_hal_common
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  This function establishes a link to the peripheral. It uses a Hardware Reference,
 *  pre-initialised during enumeration. The reference to the peripheral device (with the device
 *  handle embedded) is made valid by this command, if it completes successfully. Otherwise the
 *  structure and its members are not modified.
 *
 * \note
 *  - The device is in initial (reset) state after this command has completed successfully.
 *  - It is up to the system integrator to provide a correct implementation within the
 *    "Driver Abstraction Layer" (DAL).
 *  - A chip on a usb link is not locked after been open, so it can be openned several time 
 *    but none of the openned session will work correctly. So application have to take care of
 *    been the only one that use this chip.  
 *
 * \param[in,out] psHwReference Hardware Reference, pre-initialised during enumeration. Made valid
 *                by this function.
 *
 * \retval NFCSTATUS_SUCCESS           Open has been successful.
 * \retval NFCSTATUS_INVALID_DEVICE    The device is not enumerated or the Hardware Reference
 *                                     points to a device which does not exist. Alternatively,
 *                                     also already open devices produce this error.
 * \retval NFCSTATUS_INVALID_PARAMETER The paramter could not be properly interpreted (structure
 *                                     uninitialised?).
 * \retval NFCSTATUS_CMD_ABORTED       The caller/driver has aborted the request.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 * \if hal
 *  \sa \ref phDalNfc_Open, \ref phHalNfc_Close, \ref grp_dal_nfc "DAL"
 * \endif
 */
extern NFCSTATUS phHalNfc_Open( phHal_sHwReference_t *psHwReference);



/**
 *  \if hal
 *   \ingroup grp_hal_common
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  Retrieves the capabilities of the device represented by the Hardware Reference parameter.
 *  The protocol supported, the MTU and other mandatory information are located inside the
 *  pDevCapabilities parameter.
 *
 *  \param[in]  psHwReference     Hardware Device Reference, pre-initialised during enumeration,
 *                                validated during opening.
 *  \param[out] psDevCapabilities Pointer to the device capabilities structure where all relevant
 *                                capabilities of the peripheral are stored.
 *
 *  \retval NFCSTATUS_SUCCESS            Success.
 *  \retval NFCSTATUS_INVALID_PARAMETER  One or more of the supplied parameters could not be properly
 *                                       interpreted.
 *  \retval NFCSTATUS_INVALID_DEVICE     The device has not been opened or has been disconnected
 *                                       meanwhile.
 *  \retval NFCSTATUS_CMD_ABORTED        The command was aborted by the device.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 */
extern NFCSTATUS phHalNfc_GetDeviceCapabilities(phHal_sHwReference_t          *psHwReference,
                                                phHal_sDeviceCapabilities_t   *psDevCapabilities);


/**
 *  \if hal
 *   \ingroup grp_hal_nfci
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  This function allows the Initiator to search for Targets/cards ("Remote Devices") in the RF
 *  neighbourhood, following a defined sequence and allowing a configurable subset of modes.
 *  The number of Remote Devices detected is returned to the caller (upper layer) as well as
 *  information about the startup phase in the corresponding \ref phHal_sRemoteDevInformation_t
 *  array element.\n
 *
 *  \note If timeout functionality is required the system integrator has to install a mechanism to
 *        abort the operation at driver level. Alternatively, the caller, if running in a
 *        de-coupled thread, can implement this feature.
 *  \note before any poll please check the operating mode supported by the NFC Hardware. 
 *        You can do so by using the \ref phHalNfc_GetDeviceCapabilities function and check the value of 
 *        psDevCapabilitiesInitiator->SupProtocol.<Wanted Protocol>
 *  \note With the \ref phHalNfc_GetDeviceCapabilities function you can also check the value of 
 *        psDevCapabilitiesInitiator->MultiPollSessionSup, 
 *        this will tell you if all informations returned by the poll will be useable or not after a second poll.  
 *        If not you have to use all /ref psRemoteDevInfoList detected before doing another poll.
 *
 *  \param[in]     psHwReference Hardware Reference.
 *
 *  \param[in]     OpModes Array (Enum type) controlling operation modes under which to look for Remote
 *                 Devices and the sequence order of the scan. Each member of the array represents
 *                 a single mode to search within.
 *                 When reading the array the function considers index 0 as most and index n as least
 *                 prioritary. The stop-condition is the the Array Terminator. \b Caution: This must always
 *                 be present as the function has no other way to determine the length of the array.
 *                 See the declaration of \ref phHal_eOpModes_t. Call to \ref phHalNfc_GetDeviceCapabilities
 *                 gives the operation modes supported by the Hardware device (in case the array is filled
 *                 with at least one not supported operation mode, the function will return a
 *                 NFCSTATUS_INVALID_PARAMETER error).
 *
 *  \param[in,out] psRemoteDevInfoList Points to the Remote Device Information structure list.
 *                 The caller has to provide a pre-allocated array of \b zero'ed structures.\n
 *                 During polling, the members of the list are filled with information about the
 *                 detected remote devices.	This information includes
 *                 - state,
 *                 - parameters of the start up phase, and
 *                 - information about the used protocol.\n
 *                 There are two ways for the list to be handled:
 *                 - If the device supports multi-polling (see \ref phHalNfc_GetDeviceCapabilities),
 *                   the function can be called as often as required, without loss of information
 *                   regarding previously acquired devices. To do this, the user needs to set
 *                   \ref phHal_sDevInputParamControlFlags_t::PollingAddsDevs. In this case,
 *                   the function does no RF field OFF/ON cycle.\n
 *                   Please note that, for this mode of operation, the user must provide a
 *                   "fresh" array to the function each time.
 *                 - If repeated polling is not supported the function always does a RF reset, 
 *                   causing all Remote Device information to be invalid. In addition,
 *                   \ref phHal_sDevInputParamControlFlags_t::PollingAddsDevs must \b not be set.\n
 *                 The array is filled from the beginning (index 0) on each time.
 *
 *  \param[in,out] pNbrOfRemoteDev The caller has to provide the maximum number of remote devices
 *                 (number of pre-allocated structures) to detect. The function returns the number
 *                 of remote devices actually found in this parameter. It is required that the
 *                 caller remembers the original number of array fields (maximum number of devices)
 *                 which is of importance when the function is called again.
 *
 *  \param[in,out] psDevInputParam Points to the input device parameter union/structure needed by the
 *                 start up phase. Don't forget to set the ISO14443_4B_AFI of the device to be polled (0 means
 *                 all devices).                   
 *
 *  \retval NFCSTATUS_SUCCESS                  Success.
 *  \retval NFCSTATUS_INVALID_PARAMETER        One or more of the supplied parameters could not be
 *                                             properly interpreted.
 *  \retval NFCSTATUS_INVALID_DEVICE           The device has not been opened or has been
 *                                             disconnected meanwhile.
 *  \retval NFCSTATUS_MULTI_POLL_NOT_SUPPORTED If the device does not support multi-polling, but the caller
 *                                             has set \ref phHal_sDevInputParamControlFlags_t::PollingAddsDevs
 *                                             this value is returned and the function does nothing.
 *  \retval NFCSTATUS_MORE_INFORMATION         If the device supports multi-polling and the number of
 *                                             devices in the RF field (remote devices) is larger than
 *                                             the number that the caller has specified, this value
 *                                             is returned. Thus the caller has the chance to repeat 
 *                                             polling using a larger Remote Device list. Remote devices
 *                                             that have been acquired already are not detected
 *                                             again during this repeated polling. So, the previously
 *                                             retrieved list of devices is still valid and must be preserved.
 *                                             In order to enable multi-polling, the caller needs to set
 *                                             \ref phHal_sDevInputParamControlFlags_t::PollingAddsDevs .
 *  \retval NFCSTATUS_NO_DEVICES_FOUND         No remote device found in the RF field.
 *  \retval NFCSTATUS_CMD_ABORTED              The caller/driver has aborted the request.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 * \if hal
 *  \sa \ref phHal_sDeviceCapabilities, \ref phHalNfc_GetDeviceCapabilities, \ref phHalNfc_Connect, \ref phHalNfc_Disconnect
 * \endif
 *
 */
extern NFCSTATUS phHalNfc_Poll(phHal_sHwReference_t           *psHwReference,
                               phHal_eOpModes_t                OpModes[],
                               phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                               uint8_t                        *pNbrOfRemoteDev,
                               phHal_sDevInputParam_t         *psDevInputParam);


/**
 *  \if hal
 *   \ingroup grp_hal_nfci
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  Allows to connect to a single, specific, already known Remote Device. The Remote Device
 *  Information structure is already pre-initialised with data (e.g. from polling or an earlier
 *  session, after \ref phHalNfc_Disconnect "disconnect").
 *  A new session is started after the connect function returns successfully. The session ends with a
 *  successful disconnect (see \ref phHalNfc_Disconnect function).
 *
 *  \note If timeout functionality is required the system integrator has to install a mechanism to
 *        abort the operation at driver level. Alternatively, the caller, if running in a
 *        de-coupled thread, can implement this feature.
 *
 *  \param[in]     psHwReference Hardware Device Reference returned during enumeration.
 *
 *  \param[in]     OpMode Enum value controlling the operation mode under which to activate the Remote
 *                 Device. See the declaration of \ref phHal_eOpModes_t.
 *
 *  \param[in,out] psRemoteDevInfo Points to the Remote Device Information structure. The members of
 *                 it can be re-used from a previous session.
 *
 *  \param[in]     psDevInputParam Points to the input device parameter struct/union needed by the
 *                 start up phase.
 *
 *  \retval NFCSTATUS_SUCCESS                  Success.
 *  \retval NFCSTATUS_INVALID_PARAMETER        One or more of the supplied parameters could not be
 *                                             properly interpreted.
 *  \retval NFCSTATUS_ALREADY_CONNECTED        More than one phHalNfc_Connect is not allowed during
 *                                             a session on the same remote device. The session has to be closed before (see
 *                                             \ref phHalNfc_Disconnect function) .
 *  \retval NFCSTATUS_RF_TIMEOUT               Command received no response from the remote device
 *                                             and ended with a time out.
 *  \retval NFCSTATUS_INVALID_DEVICE           The device has not been opened before. The Remote Device Identifier
 *                                             is not valid.
 *  \retval NFCSTATUS_INVALID_DEVICE_REQUEST   The Initiator has requested a mode, not enabled in the mode selection bitfield.
 *  \retval NFCSTATUS_CMD_ABORTED              The caller/driver has aborted the request.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 *  \if hal
 *   \sa \ref phHal_sDeviceCapabilities, \ref phHalNfc_Disconnect
 *  \endif
 */
extern NFCSTATUS phHalNfc_Connect(phHal_sHwReference_t          *psHwReference,
                                  phHal_eOpModes_t               OpMode,
                                  phHal_sRemoteDevInformation_t *psRemoteDevInfo,
                                  phHal_sDevInputParam_t        *psDevInputParam);


/**
 *  \if hal
 *   \ingroup grp_hal_nfci
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  The phHalNfc_Transceive function allows the Initiator to send and receive data to and from the
 *  Remote Device selected by the caller. The caller has to provide the Remote Device Information
 *  structure and the command in order to communicate with the selected remote device.
 *
 *  \note before any transceive please check the Maximum Transfert Unit supported by the NFC Hardware, 
 *        this is the maximum buffer size useable with the pSendBuf and pRecvBuf buffers.\n 
 *        You can do so by using the \ref phHalNfc_GetDeviceCapabilities function and check the value of 
 *        psDevCapabilitiesInitiator->MTU
 *
 *  \param[in]      psHwReference Device reference returned during enumeration.
 *
 *  \param[in,out]  psRemoteDevInfo Points to the Remote Device Information structure which
 *                  identifies the selected Remote Device.
 *
 *  \param[in]      Cmd Command to perform. See the command list.
 *
 *  \param[out]     psDepAdditionalInfo Set/get the protocol info about NAD or Meta Chaining usage.
 *                  The NAD value is specified by the Initiator.
 *
 *  \param[in]      pSendBuf Pointer to the data to be sent. See \ref phHal_uCmdList_t "Command List" for
 *                           additional information about the required format.
 *
 *  \param[in]      SendLength Length, in byte, of the pSendBuf parameter.
 *
 *  \param[out]     pRecvBuf Pointer to the buffer that receives the data returned by the media.
 *
 *  \param[in,out]  pRecvLength Supplies the Length, in byte, of the pRecvLength parameter and
 *                  receives the number of bytes actually received from the media.
 *
 *  \retval NFCSTATUS_SUCCESS               Success.
 *  \retval NFCSTATUS_INVALID_PARAMETER     One or more of the supplied parameters could not be
 *                                          properly interpreted or are invalid.
 *  \retval NFCSTATUS_INVALID_DEVICE        The device has not been opened or has been disconnected
 *                                          meanwhile.
 *  \retval NFCSTATUS_CMD_ABORTED           The caller/driver has aborted the request.
 *  \retval NFCSTATUS_BUFFER_TOO_SMALL      The buffer provided by the caller is too small.
 *  \retval NFCSTATUS_RF_TIMEOUT            No data has been received within the TIMEOUT period.
 *  \retval NFCSTATUS_COMMAND_NOT_SUPPORTED The command is not supported.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 *
 *  \section section_trx_sndbuf_fmt Send Buffer Format that Needs to be Provided
 *       \subsection subsection_trx_sndbuf_fmt_nfc NFC
 *            For the detailed command description please review \ref phHal_eNfcCmdList_t.
 *            \par Data Exchange
 *              \copydoc phHal_eNfcCmdListNfcDEP
 *            \par Parameter Selection
 *              \copydoc phHal_eNfcCmdListNfcPSL
 *            \par Attention
 *              \copydoc phHal_eNfcCmdListNfcATT
 *
 *       \subsection subsection_trx_sndbuf_fmt_mf Felica
 *            For the detailed command description please review \ref phHal_eFelicaCmdList_t.
 *            \par Data Exchange
 *              \copydoc phHal_eFelicaCmdListFelicaCmd
 *
 *       \subsection subsection_trx_sndbuf_fmt_tcl ISO 14443-4
 *            For the detailed command description please review \ref phHal_eIso14443_4_CmdList_t.
 *            \par Data Exchange
 *              \copydoc phHal_eIso14443_4_CmdListTClCmd
 *            \par Protocol Parameter Selection (PPS)
 *              For devices that support this function the definition of the function input data
 *              is as follows (\b PN51X and \b PN53X do \b not expose this command to the user):\n
 *              \copydoc phHal_eIso14443_4_CmdListPPSCmd
 *            \par Presence Check
 *              \copydoc phHal_eIso14443_4_CmdListPresenceCheckCmd
 *
 *       \subsection subsection_trx_sndbuf_fmt_mf Mifare
 *            For the detailed command description please review \ref phHal_eMifareCmdList_t.
 *            \par Authent A
 *              \copydoc phHal_eMifareCmdListMifareAuthentA
 *            \par Authent B
 *              \copydoc phHal_eMifareCmdListMifareAuthentB
 *            \par Read
 *              \copydoc phHal_eMifareCmdListMifareRead
 *            \par Write 16 Bytes
 *              \copydoc phHal_eMifareCmdListMifareWrite16
 *            \par Write 4 Bytes
 *              \copydoc phHal_eMifareCmdListMifareWrite4
 *
 *       \subsection subsection_trx_sndbuf_fmt_others Others
 *            Please see \ref phHal_uCmdList_t.
 *
 */
extern NFCSTATUS phHalNfc_Transceive(phHal_sHwReference_t           *psHwReference,
                                     phHal_sRemoteDevInformation_t  *psRemoteDevInfo,
                                     phHal_uCmdList_t                Cmd,
                                     phHal_sDepAdditionalInfo_t     *psDepAdditionalInfo,
                                     uint8_t                        *pSendBuf,
                                     uint16_t                        SendLength,
                                     uint8_t                        *pRecvBuf,
                                     uint16_t                       *pRecvLength);



/**
 *  \if hal
 *   \ingroup grp_hal_nfci
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  The function allows to disconnect from a specific Remote Device. This function closes the session
 *  opened with \ref phHalNfc_Connect "Connect".
 *
 *  \param[in]      psHwReference Device reference returned during enumeration.
 *
 *  \param[in,out]  psRemoteDevInfo Points to the valid (connected) Remote Device Information structure.
 *
 *  \retval NFCSTATUS_SUCCESS             Success.
 *  \retval NFCSTATUS_INVALID_PARAMETER   One or more of the supplied parameters could not be properly
 *                                        interpreted.
 *  \retval NFCSTATUS_INVALID_DEVICE      The device has not been opened before or has already been closed.
 *  \retval NFCSTATUS_NO_DEVICE_CONNECTED The Remote Device is not connected.
 *  \retval NFCSTATUS_RF_TIMEOUT          The command received no response from the Remote Device and ended with a timeout.
 *  \retval NFCSTATUS_CMD_ABORTED         The caller/driver has aborted the request.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 *  \if hal
 *   \sa \ref phHalNfc_Connect
 *  \endif
 */
extern NFCSTATUS phHalNfc_Disconnect(phHal_sHwReference_t          *psHwReference,
                                     phHal_sRemoteDevInformation_t *psRemoteDevInfo);


/**
 *  \if hal
 *   \ingroup grp_hal_nfct
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  Start Target mode. This is the counterpart of the polling or (actively) connecting
 *  functions (implicitly those functions select Initiator mode).
 *  The function waits until there is a connecting event such as an Attribute Request (ATR)
 *  or a Request Anwser To Select (RATS), if supported by the device. If no request is
 *  received within a certain period of time the function times out and returns.
 *
 *  \param[in]      psHwReference Hardware Reference prepared by the enumeration and open functions.
 *
 *  \param[in, out] pTgInfo Pointer to the Target Information structure.
 *
 *  \param[in,out]  OpModes The caller provides an array of enumerated values, initialised with the allowed
 *                  modes. As an input, the parameter processing is the same as in the function
 *                  \ref phHalNfc_Poll : Stop-condition, Array Terminator. See also \ref phHal_eOpModes_t.
 *                  The function returns, in this parameter at index 0, the mode of operation the Initiator
 *                  would like to select. See the declaration of \ref phHal_eOpModes_t.
 *
 *  \param[out]     pConnectionReq pointer to the buffer that receives the connection request coming
 *                  from the Inititor. This parameter is only used if the Automatic response
 *                  mechanism is activated. In this case, the request is only provided as
 *                  information.
 *
 *  \param[in,out]  pConnectionReqBufLength Supplies the Length, in bytes, of the pConnectionReq
 *                  parameter and receives the length, in bytes, of the request. If the Automatic
 *                  respose is deactivated, the pConnectionReqBufLength will be zero. Take care : if this 
 *                  lenght is not long enought to get the complete pConnectionReq, it will contain only the 
 *                  beginning of this frame .
 *
 *  \retval NFCSTATUS_SUCCESS                Successful.
 *  \retval NFCSTATUS_INVALID_DEVICE         The device has not been opened or has been disconnected
 *                                           meanwhile.
 *  \retval NFCSTATUS_INVALID_PARAMETER      One or more of the supplied parameters could not be
 *                                           properly interpreted.
 *  \retval NFCSTATUS_INVALID_DEVICE_REQUEST The Initiator has requested a mode, not enabled in the
 *                                           mode selection bitfield.
 *  \retval NFCSTATUS_CMD_ABORTED            The caller/driver has aborted the request.
 *  \retval NFCSTATUS_RF_TIMEOUT             The function has timed out, no request has been received.
 *
 *  \note The timeout period of the function is configurable via an the
 *        \ref PHHALNFC_IOCTL_PN5XX_SET_USER_TIMEOUT I/O Control command, please refer to
 *        its description. The function uses a
 *        \ref PHHALNFC_PN5XX_DEFAULT_USER_TIMEOUT "default value" if the
 *        \ref PHHALNFC_IOCTL_PN5XX_SET_USER_TIMEOUT "IOCTL" is not used.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 *  \if hal
 *   \sa \ref phHalNfc_Poll
 *  \endif
 */
extern NFCSTATUS phHalNfc_StartTargetMode(phHal_sHwReference_t       *psHwReference,
                                          phHal_sTargetInfo_t        *pTgInfo,
                                          phHal_eOpModes_t            OpModes[],
                                          uint8_t                    *pConnectionReq,
                                          uint8_t                    *pConnectionReqBufLength );


/**
 *  \if hal
 *   \ingroup grp_hal_nfct
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  Allows the Target to retrieve data/commands coming from the Initiator.
 *
 *  \note before any receive please check the Maximum Transfert Unit supported by the NFC Hardware, 
 *        this is the maximum buffer size useable with the pRecvBuf buffer.\n 
 *        You can do so by using the \ref phHalNfc_GetDeviceCapabilities function and check the value of 
 *        psDevCapabilitiesTarget->MTU
 *
 *  \param[in]      psHwReference Hardware Reference returned by phHalNfc_Enumerate function
 *
 *  \param[out]     psDepAdditionalInfo Retrieves the protocol info about NAD or Meta Chaining usage.
 *                  The NAD value is specified by the Initiator.
 *
 *  \param[out]     pRecvBuf pointer to the buffer that receives the data/command coming from the
 *                  Initiator.
 *
 *  \param[in,out]  pRecvLength Supplies the Length, in byte, of the pRecvBuf parameter and receives
 *                  the number of bytes received from the Initiator.
 *
 *  \retval NFCSTATUS_SUCCESS                Successful.
 *  \retval NFCSTATUS_INVALID_DEVICE         The device has not been opened or has been disconnected
 *                                           meanwhile.
 *  \retval NFCSTATUS_INVALID_PARAMETER      One or more of the supplied parameters could not be
 *                                           properly interpreted.
 *  \retval NFCSTATUS_CMD_ABORTED            The caller/driver has aborted the request.
 *  \retval NFCSTATUS_BUFFER_TOO_SMALL       The buffer provided by the caller is too small.
 *  \retval NFCSTATUS_RF_TIMEOUT             No data has been received within the TIMEOUT period.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 *  \note For this function has the same timeout and abort features as \ref phHalNfc_StartTargetMode.
 *        
 */
extern NFCSTATUS phHalNfc_Receive(phHal_sHwReference_t        *psHwReference,
                                  phHal_sDepAdditionalInfo_t  *psDepAdditionalInfo,
                                  uint8_t                     *pRecvBuf,
                                  uint16_t                    *pRecvLength);



/**
 *  \if hal
 *   \ingroup grp_hal_nfct
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  Allows the Target to send back data to the Initiator. It has to be the response to
 *  the previous command received via the phHalNfc_Receive function.
 *
 *  \note before any send please check the Maximum Transfert Unit supported by the NFC Hardware, 
 *        this is the maximum buffer size useable with the pSendBuf buffer.\n 
 *        You can do so by using the \ref phHalNfc_GetDeviceCapabilities function and check the value of 
 *        psDevCapabilitiesTarget->MTU
 *  \param[in]      psHwReference Hardware Reference returned by the enumeration procedure and
 *                  validated by opening the device.
 *  \param[in,out]  psDepAdditionalInfo Tells the protocol about NAD or Meta Chaining usage. NAD
 *                  must be the same as specified by the Initiator.
 *  \param[in] pSendBuf Pointer to the data to send back to the Initiator.
 *
 *  \param[in] SendLength Length, in byte, of the pSendBuf parameter.
 *
 *
 *  \retval NFCSTATUS_SUCCESS           Successful.
 *  \retval NFCSTATUS_INVALID_DEVICE    The device has not been opened or has been disconnected
 *                                      meanwhile.
 *  \retval NFCSTATUS_INVALID_PARAMETER One or more of the supplied parameters could not be properly
 *                                      interpreted.
 *  \retval NFCSTATUS_CMD_ABORTED       The caller/driver has aborted the request.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 */
extern NFCSTATUS phHalNfc_Send(phHal_sHwReference_t        *psHwReference,
                               phHal_sDepAdditionalInfo_t  *psDepAdditionalInfo,
                               uint8_t                     *pSendBuf,
                               uint16_t                     SendLength);



/**
 *  \if hal
 *   \ingroup grp_hal_common
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  The I/O Control function allows the caller to use (vendor-) specific functionality provided
 *  by the lower layer or by the hardware. Each feature is accessible via a specific IOCTL Code
 *  and has to be documented by the provider of the driver and the hardware.
 *  See "IOCTL Codes" for the definition of a standard command set.\n
 *
 *  \param[in]      psHwReference Device reference returned during enumeration.
 *
 *  \param[in]      IoctlCode Control code for the operation. This value identifies the specific
 *                  operation to be perform.
 *
 *  \param[in]      pInBuf Pointer to the data required to perform the operation. This parameter
 *                  can be NULL if the IoctlCode parameter specifies an operation that does not
 *                  require input data.
 *
 *  \param[in]      InLength Length, in byte, of the the buffer referenced by pInBuf.
 *
 *  \param[out]     pOutBuf Pointer to the buffer that receives the operation's output data. This
 *                  parameter can be NULL if the IoctlCode specifies an operation that does not
 *                  produce output data.
 *
 *  \param[in,out] pOutLength Supplies the Length, in byte, of the pRecvBuf parameter and receives
 *                 the size, in bytes,  of the stored data in the buffer referenced by OutBuf.
 *
 *  \retval NFCSTATUS_SUCCESS           Hardware link successfully opened.
 *  \retval NFCSTATUS_INVALID_PARAMETER One or more of the supplied parameters could not be properly
 *                                      interpreted.
 *  \retval NFCSTATUS_INVALID_DEVICE    The device has not been opened or has been disconnected
 *                                      meanwhile.
 *  \retval NFCSTATUS_BUFFER_TOO_SMALL  The buffer supplied by the caller is to small to receive all
 *                                      data.
 *  \retval NFCSTATUS_CMD_ABORTED       The caller/driver has aborted the request.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 *  \if hal
 *   \sa \ref phDalNfc_Ioctl, \ref grp_ioctl_codes
 *  \endif
 */
extern NFCSTATUS phHalNfc_Ioctl(phHal_sHwReference_t     *psHwReference,
                                uint16_t                  IoctlCode,
                                uint8_t                  *pInBuf,
                                uint16_t                  InLength,
                                uint8_t                  *pOutBuf,
                                uint16_t                 *pOutLength);



/**
 *  \if hal
 *   \ingroup grp_hal_common
 *  \else
 *   \ingroup grp_mw_external_hal_funcs
 *  \endif
 *
 *  Closes the link to the peripheral NFC device.
 *
 *  \note It is up to the system integrator to provide a correct implementation within the
 *        "Driver Abstraction Layer (DAL)".
 *
 *  \param[in,out] psHwReference Pointer to a Hardware Reference structure, pre-set during
 *                 enumeration and finally initialised by opening the link to the NFC peripheral.
 *
 *  \retval NFCSTATUS_SUCCESS           Closing successful.
 *  \retval NFCSTATUS_INVALID_DEVICE    The device has not been opened or has been disconnected
 *                                      meanwhile.
 *  \retval NFCSTATUS_INVALID_PARAMETER One or more of the supplied parameters could not be properly
 *                                      interpreted.
 *
 *  \par RF Behaviour:
 *      Described in the \ref subsection_architecture_diags_rf "Main Architecture Page".
 *
 *  \if hal
 *   \sa \ref phDalNfc_Close, \ref phHalNfc_Open, \ref grp_dal_nfc "DAL"
 *  \endif
 */
extern NFCSTATUS phHalNfc_Close(phHal_sHwReference_t *psHwReference);


#endif /* GUARD */

