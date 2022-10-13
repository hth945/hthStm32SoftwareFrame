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
 * \file  phNfcHalTypes.h
 * \brief Structure declarations/type definitions belonging to the HAL subsystem.
 *
 * Project: NFC MW / HAL
 *
 * $Date: Thu Sep 27 09:37:56 2007 $
 * $Author: frq09147 $
 * $Revision: 1.1 $
 * $Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $
 *
 */



#ifndef PHNFCHALTYPES_H /* */
#define PHNFCHALTYPES_H /* */

#ifndef PH_FRINFC_EXCLUDE_FROM_TESTFW /* */

/**
 *  \name HAL Types
 *
 * File: \ref phNfcHalTypes.h
 *
 */
/*@{*/
#define PHNFCHALTYPES_FILEREVISION "$Revision: 1.1 $" /**< \ingroup grp_file_attributes */
#define PHNFCHALTYPES_FILEALIASES  "$Aliases: SDK_HAL_2.2,SDK_HAL_2.2_071008 $"     /**< \ingroup grp_file_attributes */
/*@}*/

#endif /* PH_FRINFC_EXCLUDE_FROM_TESTFW */

#include "phNfcTypes.h"
#include "phNfcCompId.h"
#include "phNfcDalTypes.h"


/**
 *
 *  \name HAL Overall Definitions
 *
 *  Definitions applicable to a variety of purposes and functions/features.
 *
 */
 /*@{*/
#define PHHAL_COMMON_MAX_STRING_LENGTH   64     /**< \ingroup grp_hal_common
                                                     Maximum vendor name length in bytes. */
#define PHHAL_UNKNOWN_DEVICE_TYPE        0x00   /**< \ingroup grp_hal_common
                                                     Unknown device type. */
#define PHHAL_SERIAL_DEVICE              0x01   /**< \ingroup grp_hal_common
                                                     Serial device type.  */
#define PHHAL_USB_DEVICE                 0x02   /**< \ingroup grp_hal_common
                                                     USB device type. */
#define PHHAL_I2C_DEVICE                 0x03   /**< \ingroup grp_hal_common
                                                     I2C device type. */
#define PHHAL_SPI_DEVICE                 0x04   /**< \ingroup grp_hal_common
                                                     SPI device type. */
#define PHHAL_PARALLEL_DEVICE            0x05   /**< \ingroup grp_hal_common
                                                     Parallel device type. */
#define PHHAL_NFCID_LENGTH               0x0A   /**< \ingroup grp_hal_common
                                                     Maximum length of NFCID 1..3. */
/*@}*/


/** \ingroup grp_hal_common
 *
 *
 * \if hal
 *  \brief Operation Mode Enumerator
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Operation Mode List Structure </em> describes the available operating modes of a NFC device.\n
 *
 *  \note \b Polling: Some modes, such as Active and Passive, are mutually exclusive and are processed following
 *        their priorities. For example, when the \b polling function processses the list of
 *        modes to poll and finds a Remote Device within one of the mutually exclusive modes the current
 *        mode determines the further behaviour of the function: E.g., once having activated a Target in
 *        active [passive] mode no attempts will be made to activate a further Target in passive [active]
 *        mode.
 *  \if hal
 *   \sa \ref phHalNfc_Poll
 *  \endif
 */
typedef enum phHal_eOpModes
{
    phHal_eOpModesArrayTerminator = 0x10000000, /**<  Must be inserted into the last position of the Operating
                                                 *    Modes array.
                                                 *    \if hal
                                                 *      Used in \ref phHalNfc_Poll.
                                                 *    \endif
                                                 */
    phHal_eOpModesNfcActive106 =  0x10000001,    /**<  Active operating mode at 106kbps.  */
    phHal_eOpModesNfcActive212 =  0x10000002,    /**<  Active operating mode at 212kbps. */
    phHal_eOpModesNfcActive424 =  0x10000003,    /**<  Active operating mode at 424kbps. */
    phHal_eOpModesNfcPassive106 = 0x10000004,    /**<  Passive operating mode using the ISO14443-3 Type A startup at 106 kbps.*/
    phHal_eOpModesNfcPassive212 = 0x10000005,    /**<  Passive operating mode using NFC polling startup at 212k. To perform a successful poll in this mode you will have to corretly set the NfcPollPayload in \ref phHal_sDevInputParam  */
    phHal_eOpModesNfcPassive424 = 0x10000006,    /**<  Passive operating mode using NFC polling startup at 424k. To perform a successful poll in this mode you will have to corretly set the NfcPollPayload in \ref phHal_sDevInputParam */
    phHal_eOpModesMifare =        0x10000007,    /**<  Mifare operating mode. */
    phHal_eOpModesISO14443_4A =   0x10000008,    /**<  ISO14443-4 Type A operating Mode. */
    phHal_eOpModesFelica212 =     0x10000009,    /**<  Felica operating mode at 212k. */
    phHal_eOpModesFelica424 =     0x1000000A,    /**<  Felica operating mode at 424k. */
    phHal_eOpModesISO14443_4B =   0x1000000B,    /**<  ISO14443-4 Type B operating Mode. */
    phHal_eOpModesJewel =         0x1000000C,    /**<  Innovision Jewel tag operating Mode. */
#ifdef PHFRINFC_OVRHAL_MOCKUP  /* */
    phHal_eOpModesMockup =        0x1000000D,
#endif /* PHFRINFC_OVRHAL_MOCKUP */
    phHal_eOpModesInvalid = 0x00                /**<  If a  \ref phHal_sRemoteDevInformation_t "Remote Device Information"
                                                 *    instance has no Remote Device assigned, its
                                                 *    \ref phHal_sRemoteDevInformation_t::OpMode "Operation Mode"
                                                 *    member is set to \ref phHal_eOpModesInvalid "this" value. This
                                                 *    member is <em>zero</em> because an initial zeroing of
                                                 *    the \ref phHal_sRemoteDevInformation_t structure will
                                                 *    automatically (p)reset the value correctly.
                                                 */
} phHal_eOpModes_t;


/** \ingroup grp_hal_common
 *
 * \if hal
 *  \brief HAL Device State Enumerator
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> State List Enumerator </em> lists all states a Remote Device can be in.
 *
 *  \note None.
 *
 */
typedef enum phHal_eStateList
{
    phHal_eStateListUndefinedState  = 0x10000001,       /**<  Undefined state. */
    phHal_eStateListHaltState       = 0x10000002,       /**<  Halt state. */
    phHal_eStateListSelectState     = 0x10000003,       /**<  Select state. */
    phHal_eStateListSleepState      = 0x10000004,       /**<  Sleep state. */
    phHal_eStateListSenseState      = 0x10000005,       /**<  Sense state. */
    phHal_eStateListResolutionState = 0x10000006,       /**<  Resolution state. */
    phHal_eStateListPolledState     =  0x10000007       /**<  Polled state. */

} phHal_eStateList_t;



/** \ingroup grp_hal_common
 *
 * \if hal
 *  \brief Enumerated MIFARE Commands
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Mifare Command List Enumerator </em> lists all available Mifare native commands.
 *
 * \note None.
 *
 */
typedef enum phHal_eMifareCmdList
{
    phHal_eMifareCmdListMifareAuthentA = 0x10000001, /**< Mifare Standard:\n
                                                          This command performs an authentication with KEY A for a sector.\n
                                                          Format of the buffer to send with this command :
                                                          - first byte : memory (block) adress to authenticate.
                                                          - next six bytes : key A.
                                                          .
                                                          Example: 0x \b 08 01 02 03 04 05 06 authenticates block 08 with the
                                                          key 0x01[..]06.
                                                          If this command fails, the user needs to disconnect, poll and connect 
                                                          again to the remote Mifare card.
                                                     */
    phHal_eMifareCmdListMifareAuthentB = 0x10000002, /**< Mifare Standard:\n
                                                          This command performs an authentication with KEY B for a sector.\n
                                                          Format of the buffer to send with this command :
                                                          - first byte : memory (block) adress to authenticate.
                                                          - next six bytes : key B.
                                                          .
                                                          Example: See \ref phHal_eMifareCmdListMifareAuthentA.
                                                          If this command fails, the user needs to disconnect, poll and connect 
                                                          again to the remote Mifare card.
                                                     */
    phHal_eMifareCmdListMifareRead = 0x10000003,     /**< Mifare Standard and Ultra Light:\n
                                                          Read 16 Bytes from a Mifare Standard block or 4 Mifare Ultra Light pages.\n
                                                          Format of the buffer to send with this command :
                                                          - 1st (and only) byte : memory adress to read.
                                                          .
                                                          Example: 0x \b 08 reads the data (16 bytes) from the specified address.
                                                          If this command fails, the user needs to disconnect, poll and connect 
                                                          again to the remote Mifare card.
                                                     */
    phHal_eMifareCmdListMifareWrite16 = 0x10000004,  /**< Mifare Standard:\n
                                                          Write 16 bytes.\n
                                                          Format of the buffer to send with this command :
                                                          - first byte : start address of memory to write.
                                                          - next 16 bytes : Data to write.
                                                          .
                                                          Example: 0x \b 08 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
                                                          If this command fails, the user needs to disconnect, poll and connect 
                                                          again to the remote Mifare card.
                                                     */
    phHal_eMifareCmdListMifareWrite4 = 0x10000005,   /**< Mifare Ultra Light:\n
                                                          Write 4 bytes.\n
                                                          Format of the buffer to send with this command :
                                                          - first byte : start address of memory (Mifare Ultra Light page) to write.
                                                          - next 4 bytes : Data to write.
                                                          .
                                                          Example: 0x \b 08 00 01 02 03
                                                          If this command fails, the user needs to disconnect, poll and connect 
                                                          again to the remote Mifare card.
                                                     */
    phHal_eMifareCmdListMifareInc = 0x10000006,      /**< Increment. */
    phHal_eMifareCmdListMifareDec = 0x10000007,      /**< Decrement. */
    phHal_eMifareCmdListMifareTransfer = 0x10000008, /**< Tranfer.   */
    phHal_eMifareCmdListMifareRestore = 0x10000009   /**< Restore.   */
} phHal_eMifareCmdList_t;


/** \ingroup grp_hal_common
 *
 * \if hal
 *  \brief Enumerated NFC Commands
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Nfc Command List Enumerator </em> for supported NFC Commands.
 *
 * \note None.
 *
 */
typedef enum phHal_eNfcCmdList
{
    phHal_eNfcCmdListNfcPSL = 0x10000001,   /**< PSL Command (Paramter Selection):\n
                                                   - Send buffer Byte \b 0: BRit is the Baud
                                                     Rate to be negotiated for communication from
                                                     the Initiator to the Target.
                                                     - 0x00 : 106 kbps
                                                     - 0x01 : 212 kbps
                                                     - 0x02 : 424 kbps
                                                     .
                                                   - Send Buffer Byte \b 1: BRti is the Baud
                                                     Rate to be negotiated for communication from
                                                     the Target to the Initiator.
                                                     - 0x00 : 106 kbps
                                                     - 0x01 : 212 kbps
                                                     - 0x02 : 424 kbps
                                                     .
                                             */
    phHal_eNfcCmdListNfcDEP = 0x10000002,   /**< DEP command (Data exchange protocol): \n
                                                 - This command sends the data buffer directly (without the need of any specific format).
                                                 - The maximum size of the send/receive buffer is \ref phHal_sDeviceCapabilities::MTU.
                                                   In order to send larger quantities than given by the MTU the caller must make
                                                   use of Meta-Chaining (\ref  phHal_sDepFlags::MetaChaining ).
                                                 .
                                            */
    phHal_eNfcCmdListNfcATT = 0x10000003    /**< Attention Command:\n
                                                 No input data is required.
                                             */
} phHal_eNfcCmdList_t;


#ifndef PH_SWIG_SHALL_SKIP_THIS /* Mantis #0000174: 09.01.06, by GK:
                                 *  Attention: This is used because the FRI test program wrapper
                                 *  generator has trouble with this typedef. We redefine it for
                                 *  the FRI test. This has no effect since it is not used by the
                                 *  FRI tests anyway.
                                 */
/** \ingroup grp_hal_common
 *
 * \if hal
 *  \brief Enumerated Raw Commands
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Raw Command List Enumerator </em> lists all available Raw Commands.
 *
 * \note None.
 *
 */
typedef enum phHal_eRawCmdList
{
    phHal_eRawCmdListRaw = 0xFFFFFFFF,     /**< Allow to send data directly without a protocol. */
} phHal_eRawCmdList_t;

#else /* PH_SWIG_SHALL_SKIP_THIS */

typedef enum phHal_eRawCmdList
{
    phHal_eRawCmdListRaw = 0x7FFFFFFF,     /* This value is never used, it just avoids wrapper trouble. */
} phHal_eRawCmdList_t;

#endif /* PH_SWIG_SHALL_SKIP_THIS */


/** \ingroup grp_hal_common
 *
 *
 * \if hal
 *  \brief Enumerated FeliCa Commands
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Felica Command List Enumerator </em> lists all available Felica Commands.
 *
 * \note None.
 *
 */
typedef enum phHal_eFelicaCmdList
{
    phHal_eFelicaCmdListFelicaCmd = 0x10000001, /**< Check/update (Read/Write) n blocks of data from/to the Felica Card.\n
                                                     Format of the buffer to send with this command:
                                                     - 1st byte:       Length of the data to be sent
                                                     - 2nd byte:       Command code depicting Check/update
                                                        - 0x06: Check
                                                        - 0x08: Update
                                                     - Next 8 bytes:   IDM (the NFCID2t)
                                                     - 11th byte:      Number of Services
                                                     - Next 2 bytes:   Service Code List
                                                     - 14th byte:      Number of Blocks(n) to Check/update
                                                     - Next 2n bytes:  Block Element or Block Address
                                                     - Next 16n bytes: Block Data (Only valid for Update)
                                                     Example: 
                                                     - Check:  0x 10 \b 06 (IDM) 01 0B 00 01 80 00.
                                                     - Update: 0x 20 \b 08 (IDM) 01 09 00 01 80 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10.
                                                 */
} phHal_eFelicaCmdList_t;

/** \ingroup grp_hal_common
 *
 *  The <em> T=Cl Command List Enumerator </em> lists all available T=Cl Commands.
 *
 * \note None.
 *
 */
typedef enum phHal_eIso14443_4_CmdList
{
/*+ MantisID:86 - JP - 15-12-2005 */
    phHal_eIso14443_4_CmdListTClCmd             = 0x10000001, /**< ISO 14443-4 Exchange command:\n
                                                                   - This command sends the data buffer directly (without the need of any specific format).
                                                                   - The maximum size of the send/receive buffer is \ref phHal_sDeviceCapabilities::MTU.
                                                                     In order to send larger quantities than given by the MTU the caller must make
                                                                     use of Meta-Chaining (\ref  phHal_sDepFlags::MetaChaining ).
                                                                   .
                                                               */

    /* Mantis  0000184, GK: */
    phHal_eIso14443_4_CmdListPPSCmd             = 0x10000002, /**< ISO 14443-4A (-only!) PPS Command.
                                                                    - Send buffer Byte \b 0:
                                                                      DRi, the Baud Rate from PCD to PICC
                                                                        - 0x00 : 106 kbps
                                                                        - 0x01 : 212 kbps
                                                                        - 0x02 : 424 kbps
                                                                        - 0x03 : 848 kbps
                                                                        .
                                                                    - Send Buffer Byte \b 1:
                                                                      DSi, the Baud rate from PICC to PCD.
                                                                        - 0x00 : 106 kbps
                                                                        - 0x01 : 212 kbps
                                                                        - 0x02 : 424 kbps
                                                                        - 0x03 : 848 kbps
                                                                        .

                                                                   \note this command is not supported by PN53x     .
                                                              */

    phHal_eIso14443_4_CmdListPresenceCheckCmd   = 0x10000003  /**< ISO 14443-4 Presence Check command:\n
                                                                   No input data is required.
                                                                   \note this command is not supported by PN53x     .
                                                               */
} phHal_eIso14443_4_CmdList_t;

/** \ingroup grp_hal_common
 *
  * \if hal
 *  \brief Enumerated Innovision Jewel tag Commands
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Jewel Command List Enumerator </em> lists all available Jewel Commands.
 *
 * \note None.
 *
 */
typedef enum phHal_eJewelCmdList
{
    phHal_eJewelCmdListJewelReadAll = 0x10000001,        /**< Read All Bytes from an Innovision Jewel tag.\n
                                                              No input data is required.
                                                          */

    phHal_eJewelCmdListJewelRead1 = 0x10000002,          /**< Read a single byte from an Innovision Jewel tag.\n
                                                              Format of the buffer to send with this command :
                                                              - 1st (and only) byte : memory address to read.
                                                              .
                                                              Example: 0x \b 08 reads the data (1 byte) from the specified address.
                                                          */

    phHal_eJewelCmdListJewelRead4 = 0x10000003,          /**< Read 4 bytes from an Innovision Jewel tag.\n
                                                              Format of the buffer to send with this command :
                                                              - 1st (and only) byte : memory address to read.
                                                              .
                                                              Example: 0x \b 08 reads the data (4 bytes) from the specified address.
                                                          */

    phHal_eJewelCmdListJewelRead8 = 0x10000004,          /**< Read 8 bytes from an Innovision Jewel tag.\n
                                                              Format of the buffer to send with this command :
                                                              - 1st (and only) byte : memory address to read.
                                                              .
                                                              Example: 0x \b 08 reads the data (8 bytes) from the specified address.
                                                          */

    phHal_eJewelCmdListJewelWriteNoErase1 = 0x10000005,  /**< Write 1 single byte without erasing (OR with old value).\n
                                                              Format of the buffer to send with this command :
                                                              - 1st byte : memory address to write.
                                                              - 2nd byte : Data to write.
                                                              .
                                                              Example: 0x \b 08 55.
                                                          */
    phHal_eJewelCmdListJewelWriteNoErase4 = 0x10000006,  /**< Write 4 bytes without erasing (OR with old value).\n
                                                              Format of the buffer to send with this command :
                                                              - 1st byte : memory address to write.
                                                              - next bytes : Data to write.
                                                              .
                                                              Example: 0x \b 08 55 66 77 88.
                                                          */
    phHal_eJewelCmdListJewelWriteNoErase8 = 0x10000007,  /**< Write 8 bytes without erasing (OR with old value).\n
                                                              Format of the buffer to send with this command :
                                                              - 1st byte : memory address to write.
                                                              - next bytes : Data to write.
                                                              .
                                                              Example: 0x \b 08 55 66 77 88 99 AA BB CC.
                                                          */
    phHal_eJewelCmdListJewelWriteErase1 = 0x10000008,    /**< Overwrite a single byte\n
                                                              Format of the buffer to send with this command :
                                                              - 1st byte : memory address to write.
                                                              - 2nd byte : Data to write.
                                                              .
                                                              Example: 0x \b 08 55.
                                                          */
    phHal_eJewelCmdListJewelWriteErase4 = 0x10000009,    /**< Overwrite 4 bytes.\n
                                                              Format of the buffer to send with this command :
                                                              - 1st byte : memory address to write.
                                                              - next bytes : Data to write.
                                                              .
                                                              Example: 0x \b 08 55 66 77 88.
                                                          */
    phHal_eJewelCmdListJewelWriteErase8 = 0x1000000A,    /**< Overwrite 8 bytes.\n
                                                              Format of the buffer to send with this command :
                                                              - 1st byte : memory address to rewritead.
                                                              - next bytes : Data to write.
                                                              .
                                                              Example: 0x \b 08 55 66 77 88 99 AA BB CC.
                                                          */
    phHal_eJewelCmdListJewelRid = 0x1000000B,            /**< Read ID, Use to read the metal-mask ROM and UID.\n
                                                              No input data is required.
                                                          */

} phHal_eJewelCmdList_t;

/** \ingroup grp_hal_common
 *
 *
 * \if hal
 *  \brief Common Command Attribute
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Hal Command Union </em> includes each available type of Commands.
 *
 * \note None.
 *
 */
typedef union phHal_uCommand
{
  phHal_eMifareCmdList_t         MfCmd;         /**< Mifare command structure.  */
  phHal_eFelicaCmdList_t         FelCmd;        /**< Felica command structure.  */
  phHal_eNfcCmdList_t            NfcInCmd;      /**< NFC Initiator command structure.  */
  phHal_eIso14443_4_CmdList_t    Iso144434Cmd;  /**< ISO 14443-4 command structure.  */
  phHal_eRawCmdList_t            RawCmd;        /**< Raw command structure.  */
  phHal_eJewelCmdList_t          JewelCmd;      /**< Jewel command structure.  */
}phHal_uCmdList_t;


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/** \ingroup grp_hal_common
 *
 *
 * \if hal
 *  \brief Flags Controlling the Data Exchange
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Data Exchange Flags Structure </em> holds information
 *  additional to the pure data to send/receive. It allows the caller to know whether the NAD and the Meta Chaining
 *  of the protocol are used or not.
 *
 */
typedef struct phHal_sDepFlags
{
    uint8_t MetaChaining : 1;      /**< [in, out] Information whether the protocol uses/shall use
                                     *  Meta Chaining. */
    uint8_t NADPresent   : 1;      /**< [in, out] Information whether the NFC/ISO 14443-4 protocols uses/shall use
                                     *  NAD. This member is read-only in Target mode. */
} phHal_sDepFlags_t;


/** \ingroup grp_hal_common
 *
 *
 * \if hal
 *  \brief Additional Information for Data Exchange
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Data Exchange protocol Additional Information Structure </em> holds information
 *  additional to the pure data to send/receive.
 *
 *  \note Via this compound the information is either specified or retrieved by the Initiator or
 *        Target.\n
 *        \par Example 1:
 *        The Initiator sends the request and specifies together with it the content of the NAD and
 *        its usage flag. The Target has to read both fields in order to use NAD properly.
 *        \par Example 2:
 *        The Target returns more data than the Initiator buffer can hold. In this case the function
 *        returns and indicates via the Meta Chaining flag that the Target has more data to send.
 *        Consequently the Initiator would have to save the data already received and call the
 *        exchange function again. Vice versa the Initiator can also <em>send</em> more data than
 *        its buffer can hold at a time, using Meta Chaining.
 *
 */
typedef struct phHal_sDepAdditionalInfo
{
    phHal_sDepFlags_t DepFlags;   /**< [in, out] Flags controlling Meta Chaining and Nad usage.*/
    uint8_t NAD;                  /**< [in, out] Nad value to use/used by the NFC/ISO 14443-4 protocols. */
} phHal_sDepAdditionalInfo_t;


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Start-Up in Passive Mode @ 106 kbps
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *
 *  The <em> Passive 106kbps Startup Structure </em> holds all information of the start up phase
 *  using the Passive mode at 106 kBaud.
 *
 *  \note All members are named according to ISO/IEC 18092.
 *
 */
typedef struct phHal_sPassive106Startup
{
    uint8_t             SensRes[2];         /**< [out] ATQ/Sens_Res. */
    uint8_t             SelRes;             /**< [out] The SAK/Sel_Res byte. */
    uint8_t             NFCID1tLength;      /**< [in, out] Length of the ID/NfcId (following the cascade
                                             *   level used). */
    uint8_t             NFCID1t[PHHAL_NFCID_LENGTH];     /**< [in, out] UID/NFCID1 target. */
} phHal_sPassive106Startup_t;

/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Start-Up in Jewel Passive Mode @ 106 kbps 
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *
 *  The <em> Jewel Startup Structure </em> holds all information of the start up phase
 *  using the Jewel Passive mode at 106 kBaud.
 *
 *  \note None.
 *
 */
typedef struct phHal_sJewelStartup
{
    uint8_t             SensRes[2];         /**< [out] ATQ/Sens_Res. */
    uint8_t             JewelId[4];         /**< [out] RID_res */
} phHal_sJewelStartup_t;

/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Start-Up in Passive Mode @ 212 & 424 kbps
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Passive 212/424kbps Startup Structure </em> holds all information of the start up
 *  phase using the Passive mode at 212 or 424 kBaud.
 *
 *  \note All members are named according to ISO/IEC 18092.
 *
 */
typedef struct phHal_sPassive212_424Startup
{
    uint8_t             RespCode;            /**< [out] Response code. */
    uint8_t             NFCID2t[8];          /**< [out] NFCID2t. */
    uint8_t             PMm[8];              /**< [out] Product Manufacturer code. */
    uint8_t             SystemCodeAvailable; /**< [out] Boolean (See \ref grp_special_conventions):
                                                  True if System Code available. */
    uint8_t             SystemCode[2];       /**< [out] Sysem Code if available. */
} phHal_sPassive212_424Startup_t;


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief NFC Protocol Attribute Structure
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Nfc Protocol Structure </em> holds all needed information of the NFC Atribute Response.
 *
 *  \note None.
 *
 */
typedef struct phHal_sNfcProtocol
{
    uint8_t NFCID3t[PHHAL_NFCID_LENGTH];        /**< [out] NFCID of the Target. */
    uint8_t TgGeneralByte[48];  /**< [out] General Bytes. */
    uint8_t TgGeneralByteLength;/**< [out] General Bytes Length. */
    uint8_t DIDtUsed;           /**< [out] Boolean (See \ref grp_special_conventions):
                                     True if DID used in the RF transaction.*/
    uint8_t NADtSup;            /**< [out] Boolean (See \ref grp_special_conventions.):
                                     True if NAD used in the DEP transaction.*/
} phHal_NfcProtocol_t;


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief ISO 14443-4 Type A Protocol Attribute Structure
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> ISO 14443-4 Type A protocol Structure </em> holds all needed information of the ISO 14443-4 activation.
 *
 *  \note None.
 *
 */
typedef struct phHal_sISO_14443_4A_Protocol
{
    uint8_t HistoBytes[256];  /**< [out] Historical Bytes. */
    uint8_t HistoBytesLength; /**< [out] Historical Bytes Length. */
    uint8_t CIDUsed;          /**< [out] Boolean (See \ref grp_special_conventions.):
                                   [out] CID used in the RF transaction.*/
    uint8_t NADUsed;          /**< [out] Boolean (See \ref grp_special_conventions):
                                   True if NAD used in the DEP transaction. */
} phHal_sISO_14443_4A_Protocol_t;

/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief ISO 14443-4 Type B Protocol Attribute Structure
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> ISO 14443-4 Type B protocol Structure </em> holds all needed information of the ISO 14443-4 activation.
 *
 *  \note None.
 *
 */
typedef struct phHal_sISO_14443_4B_Protocol
{
    uint8_t ATQB[12];      /**< [out] Response to REQB/WUPB. */
    uint8_t AttribLength;  /**< [out] Attrib answer length. */
    uint8_t Attrib[63];    /**< [out] Attrib answer. */
    uint8_t CIDUsed;       /**< [out] Boolean (See \ref grp_special_conventions.):
                                [out] CID used in the RF transaction.*/
    uint8_t NADUsed;       /**< [out] Boolean (See \ref grp_special_conventions):
                                True if NAD used in the DEP transaction. */
} phHal_sISO_14443_4B_Protocol_t;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief MIFARE Card Information
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Passive 106 kbps Mifare Card Information Structure </em> holds all parameters retrieved
 *  in the start up phase for the Mifare storage card in passive mode at 106 kBaud.
 *
 *  \note We can also find the current state of the remote device which is updated until the end of
 *        the session.
 *
 */
typedef struct phHal_sPassive106MfCardInformation
{
   phHal_eStateList_t         CurrentState;  /**< [out] Current state: Sense, Polled, ... */
   phHal_sPassive106Startup_t Startup106;    /**< Start up information for Mifare storage Cards. */
} phHal_sPassive106MfCardInformation_t;


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief ISO 14443-4 type A Card Information
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *
 *  The <em> Passive 106 kbps ISO14443-4-A Card Information Structure </em> holds all parameters
 *  retrieved in the start up phase for an ISO14443-4 Type A card in passive mode at 106 kBaud.
 *
 *  \note We can also find the current state of the Remote Device which is updated until the end of
 *        the session as well as the Anwser To Select (ATS) parameters.
 *
 */
typedef struct phHal_sPassive106_ISO14443_4A_CardInformation
{
    phHal_eStateList_t            CurrentState;         /**< [out] Current state: Sense, Polled, ... */
    phHal_sPassive106Startup_t    Startup106;           /**< Start up information for ISO14443-4
                                                         *   Type A Cards. */
    phHal_sISO_14443_4A_Protocol_t ISO14443_4A_Protocol; /**< ISO14443-4 Type A Protocol information:
                                                         *   Anwser To Select parameters. */
} phHal_sPassive106_ISO14443_4A_CardInformation_t;

/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief ISO 14443-4 type B Card Information
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *
 *  The <em> Passive 106 kbps ISO14443-4-B Card Information Structure </em> holds all parameters
 *  retrieved in the start up phase for an ISO14443-4 Type B card in passive mode at 106 kBaud.
 *
 *  \note None.
 *
 */
typedef struct phHal_sPassive106_ISO14443_4B_CardInformation
{
    phHal_eStateList_t             CurrentState;         /**< [out] Current state: Sense, Polled, ... */
    phHal_sISO_14443_4B_Protocol_t ISO14443_4B_Protocol; /**< ISO14443-4 Type B Protocol information:
                                                          *   Anwser To Attrib request */
} phHal_sPassive106_ISO14443_4B_CardInformation_t;

/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Innovision Jewel tag Information
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Passive 106 kbps Jewel tag Information Structure </em> holds all parameters retrieved
 *  in the start up phase for the Innovision Jewel tag in passive mode at 106 kBaud.
 *
 *  \note None.
 *
 */
typedef struct phHal_sPassive106_Jewel_CardInformation
{
   phHal_eStateList_t         CurrentState;  /**< [out] Current state: Sense, Polled, ... */
   phHal_sJewelStartup_t      StartupJewel;  /**< Start up information for Innovision Jewel tag. */
} phHal_sPassive106_Jewel_CardInformation_t;

/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief NFC Passive/106 kbps Information Structure
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Passive 106 kbps Nfc Information Structure </em> holds all parameters retrieved in the
 *  start up phase for the NFC Target in passive mode at 106 kBaud.
 *
 *  \note We can also find the current state of the Remote Device which is updated until the end of
 *        the session as well as all Atribute Response parameters.
 *
 */
typedef struct phHal_sPassive106NfcInformation
{
    phHal_eStateList_t         CurrentState;/**< [out] Current state: Sense, Polled, ... */
    phHal_sPassive106Startup_t Startup106;  /**< Start up information for NFC Target at 106 kBaud. */
    phHal_NfcProtocol_t        NfcProtocol; /**< NFC Protocol information : Atribute Response
                                             *   parameters. */
}phHal_sPassive106NfcInformation_t;


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Passive/212 & 424 kbps Card Information Structure
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Passive 212/424 kbps Card Information Structure </em> holds all parameters retrieved
 *  in the start up phase for the Felica card in passive mode at 212 or 424 kBaud.
 *
 *  \note We can also find the current state of the Remote Device which is updated until the end of
 *        the session.
 *
 */
typedef struct phHal_sPassive212_424CardInformation
{
    phHal_eStateList_t              CurrentState;    /**< [out] Current state: Sense, Polled, ... */
    phHal_sPassive212_424Startup_t  Startup212_424;  /**< Start up information for Felica card at
                                                      *   212 or 424 kBaud. */
} phHal_sPassive212_424CardInformation_t;


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief NFC Passive/212 & 424 kbps Information Structure
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Passive 212/424 kbps Nfc Information Structure </em> holds all parameters retrieved in
 *  the start up phase for the NFC Target in passive mode at 212 or 424 kBaud.
 *
 *  \note We can also find the current state of the Remote Device which is updated until the end of
 *        the session as well as all Atribute Response parameters.
 *
 */
typedef struct phHal_sPassive212_424NfcInformation
{
    phHal_eStateList_t              CurrentState;    /**< [out] Current state: Sense, Polled, ... */
    phHal_sPassive212_424Startup_t  Startup212_424;  /**< Start up information for NFC target at
                                                      *   212 or 424 kBaud. */
    phHal_NfcProtocol_t             NfcProtocol;     /**< NFC Protocol information : Atribute
                                                      *   Response parameters. */
} phHal_sPassive212_424NfcInformation_t;


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief NFC Active Information Structure
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Nfc Active Information Structure </em> holds all parameters retrieved in the start up
 *  phase for the NFC Target in active mode at 106, 212 or 424 kBaud.
 *
 *  \note We can also find the current state of the Remote Device which is updated until the end of
 *        the session as well as all Atribute Response parameters.
 */
typedef struct phHal_sNfcActiveInformation
{
    phHal_eStateList_t   CurrentState;  /**< [out] Current state: Sense, Polled, ... */
    phHal_NfcProtocol_t  NfcProtocol;   /**< NFC Protocol information : Atribute Response
                                         *   parameters. */
} phHal_sNfcActiveInformation_t;


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Remote Device Specific Information Container
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Remote Device Information Union </em> includes the available Remote Device Information
 *  structures. Following the device detected, the corresponding data structure is used.
 *
 *  \note None.
 *
 */
typedef union phHal_uRemoteDevInfo
{
  phHal_sPassive106MfCardInformation_t             CardInfo106;        /**< Card information structure in
                                                                        *   passive 106 kbps mode.  */
  phHal_sPassive106NfcInformation_t                NfcInfo106;         /**< NFC Target information structure
                                                                        *   in passive 106 kbps mode.  */
  phHal_sPassive212_424CardInformation_t           CardInfo212_424;    /**< Felica card information structure
                                                                        *   in Passive 212 or 424 kbps mode. */
  phHal_sPassive212_424NfcInformation_t            NfcInfo212_424;     /**< NFC Target information structure
                                                                        *   in Passive 212 or 424 kbps mode. */
  phHal_sNfcActiveInformation_t                    NfcActiveInfo;      /**< Information structure in NFC
                                                                        *   Active mode. */
  phHal_sPassive106_ISO14443_4A_CardInformation_t  ISO14443_4A_Info;   /**< Information structure in
                                                                        *   ISO14443-4 Type A mode. */
  phHal_sPassive106_ISO14443_4B_CardInformation_t  ISO14443_4B_Info;   /**< Information structure in
                                                                        *   ISO14443-4 Type B mode. */
  phHal_sPassive106_Jewel_CardInformation_t        JewelInfo;          /**< Information structure in
                                                                        *   Innovision Jewel tag mode. */
} phHal_uRemoteDevInfo_t;


/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Remote Device Information Structure
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Remote Device Information Structure </em> holds information about one single Remote
 *  Device detected by the polling function .\n
 *  It lists parameters common to all supported remote devices.
 *
 *  \note For more detailed information about a session, see the note inside the
 *        Device Capabilities structrure \ref phHal_sDeviceCapabilities_t documentation.
 *        Additionally the caller shall zero out the structure at the beginning, before usage.
 *
 *  \if hal
 *   \sa phal_sDeviceCapabilities \ref phHalNfc_Poll and \ref phHalNfc_Connect
 *  \else
 *   \sa phal_sDeviceCapabilities
 *  \endif
 *
 */
typedef struct phHal_sRemoteDevInformation
{
    uint8_t                    hLogHandle;          /**< [out] Handle of the Remote Device (logical
                                                     *   number). */
    uint8_t                    SessionOpened;       /**< [out] Boolean (See \ref grp_special_conventions):
                                                     *   Flag indicating the validity of
                                                     *   the handle of the remote device.
                                                     *   Whether <em>[in]</em> depends on the device
                                                     *   capabilities. See the
                                                     *   \ref phHal_sDeviceCapabilities_t structure
                                                     *   documentation for additional information. */
    phHal_eOpModes_t           OpMode;              /**< [out] Operation Mode. If no Remote Device is
                                                     *   behind the current structure instance, the
                                                     *   member is set to the value of \ref phHal_eOpModesInvalid.
                                                     */
    phHal_uRemoteDevInfo_t     RemoteDevInfo;       /**< Union of available Remote Device.
                                                     *   \ref phHal_uRemoteDevInfo_t Information.  */
    /* Mantis 181 */
    uint8_t UsedDID;                                /**< [in] Boolean (See \ref grp_special_conventions):
                                                     *   True if DID (NFC) was being used during polling. This field is used in order to save the DIDiUsed field of the \ref phHal_sDevInputParam_t structure.*/
    uint8_t UsedNAD;                                /**< [in] Boolean (See \ref grp_special_conventions):
                                                     *   True if NAD (NFC) was being used during polling. This field is used in order to save the NfcNADiUsed field of the \ref phHal_sDevInputParam_t structure.*/
} phHal_sRemoteDevInformation_t;


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/**
 * \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Additional Control Flags
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *
 *  The auxiliary ControlFlags substructure of \ref phHal_sDevInputParam_t has to be filled by the caller
 *  in order to hand over additional directives to the function.
 *
 * \note None.
 *
 * \if hal
 *  \sa \ref phHalNfc_Poll.
 * \endif
 */
typedef struct phHal_sDevInputParamControlFlags
{
    uint8_t PollingAddsDevs : 1;      /**< LSB set tells us whether the polling function shall add devices
                                       *   to existing ones instead of invalidating all links. This item is
                                       *   evaluated only if the \ref phHal_sDeviceCapabilities_t::MultiPollSessionSup
                                       *   member indicates that adding Remote Devices is supported. */
    uint8_t UseCustomPayload : 1;     /**< If set, depending on the OpMode, phHal_sDevInputParam.FelicaPollPayload or
                                       * phHal_sDevInputParam.NfcPollPayload will be used for Passive 212K/424K
                                       * activation. Otherwise a standard payload will be used.*/
} phHal_sDevInputParamControlFlags_t;



/** \ingroup grp_hal_nfci
 *
 * \if hal
 *  \brief Start-Up Information for the Initiator
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> \ref phHal_sDevInputParam Structure </em> holds all input information needed by the Initiator in
 *  order to perform the start up phase of the communication with Remote Device(s).
 *
 *  \note None.
 */
typedef struct phHal_sDevInputParam
{
    uint8_t FelicaPollPayload[5];          /**< [in]  Felica Polling request payload. This payload contains
                                                for instance the Time slot number from polling (in the
                                                last byte: PollPayload[4] = TSN) .*/
    uint8_t NfcPollPayload[5];             /**< [in] NFCIP-1 Polling request payload. This payload contains
                                                for instance the Time slot number from polling (in the
                                                last byte: PollPayload[4] = TSN) .*/
    uint8_t NFCIDAuto;                     /**< [in] Boolean (See \ref grp_special_conventions):
                                            *   If True the low level layer generates / has generated
                                            *   the NFCID automatically. */
    uint8_t NFCID3i[PHHAL_NFCID_LENGTH];        /**< [in, out] NFCID of the Initiator. If NFCIDAuto is 'False'
                                                the content of this parameter can be specified by the caller,
                                                otherwise it stores the automatically generated
                                                value and shall be read only. */
    uint8_t DIDiUsed;                      /**< [in] Boolean (See \ref grp_special_conventions):
                                            *   True if DID (NFC) or is used in the transaction.*/
    uint8_t CIDiUsed;                      /**< [in] Boolean (See \ref grp_special_conventions):
                                            *   True if CID (ISO14443-4A) is used in the transaction.*/
    uint8_t NfcNADiUsed;                   /**< [in] Boolean (See \ref grp_special_conventions):
                                                True if NAD (NFC) used in the transaction.*/
    /*+ MantisId : 31 - JP - 09-01-2006 */
        /*uint8_t TClNADiUsed; */                   /**< [in] Boolean (See \ref grp_special_conventions):
                                                True if NAD (NFC or ISO14443-4A) used in the transaction.*/
    /*- MantisId : 31 - JP - 09-01-2006 */
    uint8_t GeneralByte[48];               /**< [in] Initiator General Bytes
                                            *   \attention 48 bytes max. for Intiator and 47 bytes
                                            *   max. for Target. */
    uint8_t GeneralByteLength;             /**< [in] General Bytes Length: Remark: Set to zero if no
                                            *   General bytes shall be sent. */
    phHal_sDevInputParamControlFlags_t ControlFlags; /**< [in] Directives set by the caller. */
    
    uint8_t ISO14443_4B_AFI;               /**< [in] Application Familly Identifier of the device to be 
                                            *   polled */
} phHal_sDevInputParam_t;




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/** \ingroup  grp_hal_common
 *
 * \if hal
 *  \brief Vendor Attributes
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Vendor Name Structure </em> holds the Manufacturer Name. It is used inside the
 *  \ref phHal_sDeviceCapabilities_t structure.
 *
 *  \note All members of this structure are output parameters [out].
 *
 */
typedef struct phHal_sVendorName
{
    uint8_t Length;                                /**< [out] Length of the Vendor Name string. */
    int8_t   Name[PHHAL_COMMON_MAX_STRING_LENGTH]; /**< [out] Vendor Name string. */
} phHal_sVendorName_t;


/** \ingroup  grp_hal_common
 *
 * \if hal
 *  \brief Protocol Support Information
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Supported Protocols Structure </em> holds all protocol supported by the current NFC
 *  device.
 *
 *  \note All members of this structure are output parameters [out].
 *
 */
typedef struct phHal_sSupProtocol
{
    uint16_t MifareUL    : 1;  /**< Protocol Mifare Ultra Light. */
    uint16_t MifareStd   : 1;  /**< Protocol Mifare Standard. */
    uint16_t ISO14443_4A : 1;  /**< Protocol ISO14443-4 Type A.  */
    uint16_t ISO14443_4B : 1;  /**< Protocol ISO14443-4 Type B.  */
    uint16_t Felica      : 1;  /**< Protocol Felica. */
    uint16_t NFC         : 1;  /**< Protocol NFC. */
    uint16_t Jewel       : 1;  /**< Protocol Innovision Jewel Tag. */
} phHal_sSupProtocol_t;


/** \ingroup  grp_hal_common
 *
 * \if hal
 *  \brief Device Capabilities
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Device Capabilities Structure </em> holds all information about the NFC device
 *  capabilities such as the protocols supported, the Maximum Transfer Unit (MTU) or the Cascade
 *  Level Used in Target Mode.
 *
 *  \note All members of this structure are output parameters [out].
 *
 *  \if hal
 *   \sa \ref phHalNfc_Connect, \ref phHalNfc_Poll and \ref phHalNfc_Disconnect.
 *  \endif
 *
 */
typedef struct phHal_sDeviceCapabilities
{
    phHal_sHwReference_t     *psHwReference;        /**< Points to struct \ref phHal_sHwReference_t .*/
    phHal_sVendorName_t       VendorName;           /**< Name (designation) of the device. */
    uint32_t                  MTU;                  /**< Maximum Tranfer Unit : Max Size of the buffer
                                                     *   for the current device. */
    uint8_t                   MaxNumberOfRemoteDev; /**< Indicates the number of Remote Devices
                                                         simultaneously supported. */
    uint8_t                   MultiPollSessionSup;  /**< Boolean (See \ref grp_special_conventions):
                                                     *   Indicates whether the device supports polling
                                                     *   by leaving already acquired devices
                                                     *   unaffected. If FALSE, polling invalidates
                                                     *   all \ref phHal_uRemoteDevInfo_t structures. */
    uint8_t                   TargetCascadeLevel;   /**< 1, 2 or 3; Cascade level supported in Target Mode.*/
    phHal_sSupProtocol_t      InitiatorSupProtocol; /**< Supported protocols (Bitmapped) in Initiator mode. */
    phHal_sSupProtocol_t      TargetSupProtocol;    /**< Supported protocols (Bitmapped) in Target mode. */
} phHal_sDeviceCapabilities_t;
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*    TARGET STRUCTURES */

/** \ingroup grp_hal_nfct
 *
 * \if hal
 *  \brief Target Option Flags
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Target Options Bitmap </em> lists optional features valid for Target mode.
 *
 *  \note None.
 *
 */
typedef struct phHal_sTargetOptions
{
  uint8_t PassiveOnly             : 1;  /**< Passive mode will be used only. If this bit is set,
                                             we are out of the NFCIP-1 specification. */
  uint8_t AutoConnectionResponse  : 1;  /**< Send connection response automatically.
                                             The response must be provided in advance.
                                             e.g. the ATR_Res is sent automatically after
                                             receiving the ATR_Req from the intiator.*/
} phHal_sTargetOptions_t;


/** \ingroup grp_hal_nfct
 *
 * \if hal
 *  \brief Information for Target Mode Start-Up
 * \else
 *  \brief HAL-Specific
 * \endif
 *
 *  The <em> Target Information Structure </em> required to start Target mode. It contains all the
 *  information for the Target mode.
 *
 *  \note None.
 *
 */
typedef struct phHal_sTargetInfo
{
    phHal_sTargetOptions_t    Options;    /**< [in,out] Target options. */
    uint8_t    SensRes[2];                /**< [in] ATQ. */
    uint8_t    SelRes;                    /**< [in] The SAK byte.  */
    uint8_t    RandomNFCIDt;              /**< [in] Boolean (See \ref grp_special_conventions):
                                               If True NFCIDs are randomly generated.
                                               See \ref grp_special_conventions. */
    uint8_t    NFCID1t[PHHAL_NFCID_LENGTH]; /**< [in, out] NFCID1t from 1 to 10 : 0 Always assigned to 0x08.
                                               This member is caller-defined if RandomNFCIDt is 'FALSE'.*/
    uint8_t    NFCID2t[8];                /**< [in, out] Felica NfcId2t.
                                               This member is caller-defined if RandomNFCIDt is 'FALSE'.*/
    uint8_t    PAD[8];                    /**< [in] Felica PAD. */
    uint8_t    SystemCode[2];             /**< [in] Felica RemoteDevTypeReqSystem Code. */
    uint8_t    NFCID3t[PHHAL_NFCID_LENGTH];    /**< [in, out] NFCID3t of ATR_RES.
                                               This member is caller-defined if RandomNFCIDt is 'FALSE'.*/
    uint8_t    GeneralBytes[47];          /**< [in] General Bytes to reply to the Initiator in the ATR_RES. */
    uint8_t    GeneralBytesLength;        /**< [in] Number of General Bytes to reply in ATR_RES. */
} phHal_sTargetInfo_t;


#endif

/* EOF */
