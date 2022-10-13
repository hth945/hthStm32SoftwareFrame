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
 * \file phNfcIoctlCode.h
 * \brief IOCTL Code Definition.
 *
 *  This file contains control codes for the IOCTL function.
 *  
 *  
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:37:56 2007 $ 
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $
 *
 */


#ifndef PHNFCIOCTLCODE_H /* */
#define PHNFCIOCTLCODE_H /* */

#ifndef PH_FRINFC_EXCLUDE_FROM_TESTFW /* */

/**
 *  \name IOCTL Codes
 * 
 *  File: \ref phNfcIoctlCode.h
 * 
 */
/*@{*/
#define PHNFCIOCTLCODE_FILEREVISION "$Revision: 1.1 $" /**< \ingroup grp_file_attributes */
#define PHNFCIOCTLCODE_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008,NFC_FRI_SDK_1.0_071031 $"     /**< \ingroup grp_file_attributes */
/*@}*/

#endif /* PH_FRINFC_EXCLUDE_FROM_TESTFW */

/** 
 *  \name General IOCTL Codes
 *  The IOCTL code format uses the three lowest HEX digits for the code
 *  itself. The most significant digit, in this case 0, represents the
 *  general range.
 */
/*@{*/

/**
 * \ingroup grp_ioctl_codes
 *
 * Sends a <b>RAW Command</b> to a Target/card without protocol encapsulation. This command
 * is a raw Initiator/Reader "Transceive" command. As it uses no chaining, the maximum amount
 * of data to be sent or received is the number of bytes that can be handled by the
 * underlying device's or library's transceive functionality.
 *
 * Caution : No protocol-related verifications are done by this command. 
 *           Improper usage of this feature might likely disrupt
 *           communication or current HAL state: Caution!
 *
 * - Input buffer: Command to send to the device.
 * - Input buffer length: Number of bytes to send.
 * - Output buffer length: In: Max.number of bytes to receive. Out: Number of bytes actually returned.
 * - Output buffer: Response of the device.
 * .
 *
 * \note This function can be used to implement an additional or experimental contactless
 *       reader protocol on top of the HAL. In order to make this command usable in a particular
 *       RF mode,the caller might be required to apply custom register settings to the
 *       underlying device.
 */
#define PHHALNFC_IOCTL_RAW_CMD              (0x0001) /* */

/**
 * \ingroup grp_ioctl_codes
 *
 * SAM Configuration Command: This command code allows to configure the link to a directly
 * connected SAM chip.\n
 *
 * The different modes are defined in \ref grp_ioctl_samcfg_subcmd .
 *
 * - Input buffer:  The first byte (index 0) is used for the configuration mode selection.
 *                  The second Byte (index 1) is only used for the Virtual Mode, as the \b Time \b Out value.
 *                  Timeout is only used in PN53X so far, it is unused in PN51X.
 * - Output buffer: unused.
 *
 * \note If PN531 is used, be sure to have software Revision V3.3 or higher.\n
 */
#define PHHALNFC_IOCTL_PN5XX_SAM_CONFIG     (0x0002) /* */

/**
 * \defgroup grp_ioctl_samcfg_subcmd SAM Config Subcommands
 * \ingroup grp_ioctl_codes
 * 
 * Definitions of the subcommands for \ref PHHALNFC_IOCTL_PN5XX_SAM_CONFIG .\n
 * This commands are used to set the Hardware configuration of the Nfc chip ans the Secure Access Module chip (if present).\n
 * 4 differents mode are availlable on the HAL :
 * - \b PHHALNFC_IOCTL_PN5XX_SAM_CONFIG_RESET : \n 
 * \n
 * \image html SMX_Reset.png
 * The PN5XX can use the RF Field : 
 *  - to detect cards (\ref phHalNfc_Poll, \ref phHalNfc_Transceive),
 *  - to do peer to peer communication (\ref phHalNfc_StartTargetMode, , \ref phHalNfc_Transceive, \ref phHalNfc_Send, \ref phHalNfc_Receive). 
 * \n
 * - \b PHHALNFC_IOCTL_PN5XX_SAM_CONFIG_VIRTUAL :  \n 
 * \n
 * \image html SMX_Virtual.png
 * in this mode, the secure module can be seen by any external reader, every command send throught the HAL won't have any effect. The only command that will work is the changing mode command to be able to go back to other hardware mode.
 * \n
 * - \b PHHALNFC_IOCTL_PN5XX_SAM_CONFIG_WIRED :  \n 
 * \n
 * \image html SMX_WIRED.png
 * This mode is similar to the Reset mode, but the only target that can be discovered is the Secure Access Module. This way can manage its content.
 * \n
 * - \b PHHALNFC_IOCTL_PN5XX_SAM_CONFIG_BOTH :  \n 
 * \n
 * \image html SMX_BOTH.png
 *combination of the Virtual and Reset mode 
 */
/*@{*/
#define PHHALNFC_IOCTL_PN5XX_SAM_CONFIG_RESET   ((uint8_t)0x01) /**< Reset Mode: Use PN5XX in normal mode. */
#define PHHALNFC_IOCTL_PN5XX_SAM_CONFIG_VIRTUAL ((uint8_t)0x02) /**< Virtual Mode: PN5XX is invisible,
                                                                     only the SAM is seen by an external device. */
#define PHHALNFC_IOCTL_PN5XX_SAM_CONFIG_WIRED   ((uint8_t)0x03) /**< Wired Mode: The SAM only seen by the PN5XX. */
#define PHHALNFC_IOCTL_PN5XX_SAM_CONFIG_BOTH    ((uint8_t)0x04) /**< Both: Combination of Virtual and Reset modes. */
/*@}*/


/**
 * \ingroup grp_ioctl_codes
 *
 * Sets a new \b User \b Timeout in \b milliseconds for \ref phHalNfc_Receive and \ref phHalNfc_StartTargetMode operations (Target).
 * Returns the old value in the output buffer. In order to be able to restore a previously set value the caller
 * should save the content of the output buffer.
 *
 * - Input buffer: address of a uint32_t variable, casted to uint8_t pointer.
 * - Input Buffer length: Must be sizeof(uint_32_t).
 * - Output buffer: address of a uint32_t variable, casted to a uint8_t pointer.
 * - Output buffer length: Points to a variable, initialised with the value of at least sizeof(uint32_t).
 * .
 *
 * \note The default value is defined in \ref PHHALNFC_PN5XX_DEFAULT_USER_TIMEOUT .
 *
 */
#define PHHALNFC_IOCTL_PN5XX_SET_USER_TIMEOUT     (0x0003) /* */

/**
 * \ingroup grp_ioctl_codes
 * 
 * Definition of the standard timeout value (used as default value).
 *
 */
#define PHHALNFC_PN5XX_DEFAULT_USER_TIMEOUT       ((uint32_t)15000) /* */

/*@}*/ /* END General */



/**
 *  \name PN531 IOCTL Codes
 *  The IOCTL code format uses the three lowest HEX digits for the code
 *  itself. The most significant digit, 1, represents the
 *  PN53X range.
 */
/*@{*/

/**
 * \ingroup grp_ioctl_codes
 *
 * Sends a command directly to PN53x. Please refer to the PN53x user manuals for further information. 
 *
 * Caution : This function will disrupt the HAL context if a feature is used that changes the
 *           device state (e.g.: inListPassiveTarget).
 *           This function could be particularly useful to change device register values.
 * Caution : Make sure to refere to the right PN53x user manual for IOCTL message building.
			 As an example, although both PN531 and PN532 user manuals are very similar, ensure that the Input buffer sent is formatted
 *			 as required by the right user manual, since some commands, like RFConfiguration, SAMConfiguration or TgInitAsTarget, are different.
 * - Input buffer:  PN53x Command Buffer, exactly the same as in PN53x User Manual without
 *                  encapsulation (preamble and postamble are not required). 
 *                  The first byte is the command, the other bytes are the command parameters.
 * - Output buffer: PN53x response with device-specific encapsulation.
 */
#define PHHALNFC_IOCTL_PN53X_CMD            (0x1001) /* */


/**
 * \ingroup grp_ioctl_codes
 *
 * PN 53X Get Status Command: This performs a PN53X Get Status. Caution: This affects
 * existing connections and disrupts the HAL context.\n
 *
 * - Input buffer:  unused.
 * - Output buffer: the current status.
 *
 * \note This function is only useable with PN531 Software Revision V3.3 or higher; this is also useable with PN532\n
 */
#define PHHALNFC_IOCTL_PN53X_GET_STATUS      (0x1002) /* */

/*@}*/ /* END PN 53X */



/**
 *  \name PN511 IOCTL Codes
 *  The IOCTL code format uses the three lowest HEX digits for the code
 *  itself. The most significant digit, 2, represents the
 *  PN51X range.
 */
/*@{*/

/**
 * \ingroup grp_ioctl_codes
 *
 * PN 51X Soft Reset Command: This performs a PN51X Soft Reset. Caution: This affects
 * existing connections and disrupts the HAL context.
 *
 * - Input buffer:  unused.
 * - Output buffer: unused.
 */
#define PHHALNFC_IOCTL_PN511_SOFTRESET      (0x2001) /* */

/**
 * \ingroup grp_ioctl_codes
 *
 * PN 51X Set Register Command: This sets the content a PN51X register.
 * Caution: This affects existing connections and potentially disrupts the HAL context.
 *
 * - Input buffer[0]: Adress of the register.
 * - Input buffer[1]: Content to be set.
 * - Input buffer length: Must be set to at least \b 2.
 * - Output buffer: unused.
 */
#define PHHALNFC_IOCTL_PN511_SETREG         (0x2002) /* */

/**
 * \ingroup grp_ioctl_codes
 *
 * PN 51X Get Register Command: This gets the content a PN51X register.
 * Caution: This affects existing connections and potentially disrupts the HAL context.
 *
 * - Input buffer[0]: Adress of the register.
 * - Input buffer length: Must be set to at least \b 1.
 * - Output buffer[0]: Content of the register.
 * - Output buffer length: Must be set to at least \b 1.
 */
#define PHHALNFC_IOCTL_PN511_GETREG         (0x2003) /* */

/**
 * \ingroup grp_ioctl_codes
 *
 * PN 51X Event CALLBACK handling IOCTL. This IOCTL allows to switch between
 * - BFL internal POLLING, or
 * - External callback in polling mode, or
 * - External callabck in interrupt operation.
 *
 * - Input buffer[0]: If set to \b 0, external Callback operation is \b OFF. The BFL is polling.
 *                    The value \b 1 means that the external callback I/O is \b ON, in POLLING mode.
 *                    The value \b 2 (default) means that the external callback I/O is \b ON, in INTERRUPT mode.
 * - Input buffer length: Must be set to at least \b 1.
 * - Output buffer: Unused, shall be set to \b NULL.
 * - Output buffer length: Unused, shall be set to \b NULL.
 *
 * \note The call of this function is only safe between opening of the device
 *       and the start of communication.
 */
#define PHHALNFC_IOCTL_PN511_ENINTR         (0x2004) /* */

/**
 * \ingroup grp_ioctl_codes
 *
 * PN 51X WT value to be used for communication. The definition is according to
 * the ISO 18092 definition for the WT fields in the TO byte of ATR_RES.
 *
 * - Input buffer[0]: TO/WT according to the NFC specification.
 * - Input buffer length: Must be set to at least \b 1.
 * - Output buffer: Unused, shall be set to \b NULL.
 * - Output buffer length: Unused, shall be set to \b NULL.
 *
 * \note The call of this function is only safe between opening of the device
 *       and the start of communication.
 */
#define PHHALNFC_IOCTL_PN511_SETWT          (0x2005) /* */

/*@}*/  /* END PN 51X */

#endif /* PHNFCIOCTLCODE */

