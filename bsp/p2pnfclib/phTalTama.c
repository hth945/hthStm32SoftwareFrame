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
     
     
/*!
 * \file phTalTama.c
 * \brief TAL Function implementation
 *
 *  This API allows the HAL layer to use the PN531 NFC chip with the same API
 *  than HAL itself. The implementation is dedicated to the PN531.
 *
 * Project: NFC MW / HAL
 *
 * $Date: Mon Oct 29 11:09:56 2007 $ 
 * $Author: frq09147 $
 * $Revision: 1.6 $
 * $Aliases: SDK_HAL_2.2_071008,SDK_HAL_2.2 $
 *
 */
#undef	MODULE_NUMBER
#define	MODULE_NUMBER	MODULE_NFC
#undef	FILE_NUMBER
#define	FILE_NUMBER	2

#ifdef PHHAL_VERSION_ACQUISITION
    #define PHTALTAMA_SOURCE_FILEREVISION "$Revision: 1.6 $"
    #define PHTALTAMA_SOURCE_FILEALIASES  "$Aliases: SDK_HAL_2.2_071008,SDK_HAL_2.2 $"
#else /* PHHAL_VERSION_ACQUISITION */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <phTalTama.h>
#include <phNfcStatus.h>
#include <phDalNfc.h>
#include <phNfcIoctlCode.h>


#define GET_FIRMWARE_VERSION_CMD 0x02
#define GET_GENERAL_STATUS 0x04
#define READ_REGISTER 0x06
#define WRITE_REGISTER 0x08
#define SET_TAMA_PARAMETERS 0x12
#define SAM_CONFIGURATION 0x14
#define POWER_DOWN 0x16
#define RF_CONFIGURATION 0x32
#define IN_DATA_EXCHANGE_CMD 0x40
#define IN_COMMUNICATE_THRU_CMD 0x42
#define IN_DESELECT 0x44
#define IN_LIST_PASSIVE_TARGET_CMD 0x4A
#define IN_PSL_CMD 0x4E
#define IN_ATR 0x50
#define IN_RELEASE_CMD 0x52
#define IN_SELECT 0x54
#define IN_JUMP_FOR_DEP 0x56
#define TG_GET_DEP_CMD 0x86
#define TG_INIT_CMD 0x8C
#define TG_SET_DEP_CMD 0x8E
#define TG_GET_STATUS       0x8A
#define TG_SET_META_DEP_CMD 0x94
#define IN_AUTO_POLL 0x60

#define POLL_NFC_212  0x00
#define POLL_FELICA_212 0x01
#define POLL_NFC_424 0x02
#define POLL_FELICA_424 0x03
#define POLL_MIFARE 0x04
#define POLL_NFC_106 0x05
#define POLL_ISO_14443_A 0x06

#define NAD_MASK 0x80
#define MI_MASK 0x40
#define ERROR_MASK 0x3F

#define PHTAL_T_VD_NAME     "NXP SEMICONDUCTORS" /* PN 531 chip vendor name. */

/*
 *  Configuration Constant
 */

#define MAX_SIZE_TAMA_BUFFER		252
#define MAX_SIZE_TAMAMOBILE_BUFFER	262

/*
 *  General Values
 */
  
#define ACK_TIMEOUT         10

#define MAX_SIZE_DAL_BUFFER 300
#define DAL_TIME_OUT        500

#define ATR_SEND_MAX_RETRY 0x02
#define PSL_SEND_MAX_RETRY 0x02


/*
 *  RF configuration Analog settings Values
 *  for more details see TAMA Manual
 *  in target mode
 */

#define TM_RF_CFG_REG        0x59 
#define TM_GS_N_REG          0xF4
#define TM_CW_GS_P_REG       0x3F
#define TM_MOD_GS_P_REG      0x30 
#define TM_MOD_WIDTH_REG     0x26
#define TM_TX_BIT_PHASE_REG  0x87 
#define TM_RX_SEL_REG        0x84
#define TM_PASSIVE_DEMOD_REG 0x61
#define TM_RX_THRESHOLD_REG  0x85 
#define TM_ACTIVE_DEMOD_REG  0x61
#define TM_GSNLOAD_MOD_REG   0x6F


/*
 *  In initiator Mode
 */

#define IM_RF_CFG_REG        0x59 
#define IM_GS_N_REG          0xF4
#define IM_CW_GS_P_REG       0x3F
#define IM_MOD_GS_P_REG      0x30 
#define IM_MOD_WIDTH_REG     0x26
#define IM_TX_BIT_PHASE_REG  0x87 
#define IM_RX_SEL_REG        0x84
#define IM_PASSIVE_DEMOD_REG 0x4D
#define IM_RX_THRESHOLD_REG  0x85 
#define IM_ACTIVE_DEMOD_REG  0x61  
#define IM_GSNLOAD_MOD_REG   0x6F


/*
 *  RF configuration Analog settings Values
 *  for more details see TAMA Mobile Manual
 *  in target mode
 */

#ifdef RF_CONFIG_PN532_1_2
/* for PN532 1.2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#define TMM_0x0A_RF_CFG_REG				0x59 
#define TMM_0x0A_GS_N_ON_REG			0xF4
#define TMM_0x0A_CW_GS_P_REG			0x3F
#define TMM_0x0A_MOD_GS_P_REG			0x11
#define TMM_0x0A_DMOD_RF_ON_REG			0x4D
#define TMM_0x0A_RX_THRESHOLD_REG		0x85	/*	Attention: documented 0x55, but our usecase requires 0x85	*/
#define TMM_0x0A_DMOD_RF_OFF_REG		0x61
#define TMM_0x0A_GS_N_OFF_REG			0x6F
#define TMM_0x0A_MOD_WIDTH_REG			0x26
#define TMM_0x0A_MIFARE_REG				0x62
#define TMM_0x0A_TX_BITPHASE_REG		0x87

#define TMM_0x0B_RF_CFG_REG				0x69 
#define TMM_0x0B_GS_N_ON_REG			0xFF
#define TMM_0x0B_CW_GS_P_REG			0x3F
#define TMM_0x0B_MOD_GS_P_REG			0x11
#define TMM_0x0B_DMOD_RF_ON_REG			0x41
#define TMM_0x0B_RX_THRESHOLD_REG		0x85	/*	Attention: documented 0x55, but our usecase requires 0x85	*/
#define TMM_0x0B_DMOD_RF_OFF_REG		0x61
#define TMM_0x0B_GS_N_OFF_REG			0x6F

#define TMM_0x0C_GS_N_ON_REG			0xFF
#define TMM_0x0C_MOD_GS_P_REG			0x17
#define TMM_0x0C_RX_THRESHOLD_REG		0x85 

#define TMM_0x0D_RX_THRESHOLD_REG		0x85 
#define TMM_0x0D_MOD_WIDTH_REG_1		0x15
#define TMM_0x0D_MIFARE_REG_1			0x8A
#define TMM_0x0D_MOD_WIDTH_REG_2		0x08
#define TMM_0x0D_MIFARE_REG_2			0xB2
#define TMM_0x0D_MOD_WIDTH_REG_3		0x01
#define TMM_0x0D_MIFARE_REG_3			0xDA

#else

#define TMM_0x0A_RF_CFG_REG				0x59 
#define TMM_0x0A_GS_N_ON_REG			0xF4
#define TMM_0x0A_CW_GS_P_REG			0x3F
#define TMM_0x0A_MOD_GS_P_REG			0x11
#define TMM_0x0A_DMOD_RF_ON_REG			0x4D
#define TMM_0x0A_RX_THRESHOLD_REG		0x85	/*	Attention: documented 0x55, but our usecase requires 0x85	*/
#define TMM_0x0A_DMOD_RF_OFF_REG		0x61
#define TMM_0x0A_GS_N_OFF_REG			0x6F
#define TMM_0x0A_MOD_WIDTH_REG			0x26
#define TMM_0x0A_MIFARE_REG				0x62
#define TMM_0x0A_TX_BITPHASE_REG		0x8F

#define TMM_0x0B_RF_CFG_REG				0x59
#define TMM_0x0B_GS_N_ON_REG			0xFF
#define TMM_0x0B_CW_GS_P_REG			0x3F
#define TMM_0x0B_MOD_GS_P_REG			0x11
#define TMM_0x0B_DMOD_RF_ON_REG			0x4D
#define TMM_0x0B_RX_THRESHOLD_REG		0x85	/*	Attention: documented 0x55, but our usecase requires 0x85	*/
#define TMM_0x0B_DMOD_RF_OFF_REG		0x61
#define TMM_0x0B_GS_N_OFF_REG			0x6F

#define TMM_0x0C_GS_N_ON_REG			0xFF
#define TMM_0x0C_MOD_GS_P_REG			0x17
#define TMM_0x0C_RX_THRESHOLD_REG		0x75

#define TMM_0x0D_RX_THRESHOLD_REG		0x75
#define TMM_0x0D_MOD_WIDTH_REG_1		0x15
#define TMM_0x0D_MIFARE_REG_1			0x8A
#define TMM_0x0D_MOD_WIDTH_REG_2		0x08
#define TMM_0x0D_MIFARE_REG_2			0xB2
#define TMM_0x0D_MOD_WIDTH_REG_3		0x01
#define TMM_0x0D_MIFARE_REG_3			0xD1

#endif //RF_CONFIG_PN532_1_2

#define AUTO_POLL_TIME_OUT          2500

/*
 *  Target types used for as parameter of InAutoPoll command
 */

#define TARGET_TYPE_GENERIC_PASSIVE_106 0x00
#define TARGET_TYPE_GENERIC_PASSIVE_212 0x01
#define TARGET_TYPE_GENERIC_PASSIVE_424 0x02
#define TARGET_TYPE_JEWEL               0x04
#define TARGET_TYPE_MIFARE				0x10
#define TARGET_TYPE_FELICA_212			0x11
#define TARGET_TYPE_FELICA_424			0x12
#define TARGET_TYPE_ISO14443_4A			0x20
#define TARGET_TYPE_ISO14443_4B			0x23
#define TARGET_TYPE_DEP_PASSIVE_106		0x40
#define TARGET_TYPE_DEP_PASSIVE_212		0x41
#define TARGET_TYPE_DEP_PASSIVE_424		0x42
#define TARGET_TYPE_DEP_ACTIVE_106		0x80
#define TARGET_TYPE_DEP_ACTIVE_212		0x81
#define TARGET_TYPE_DEP_ACTIVE_424		0x82


NFCSTATUS phTalTama_Protocol(phHal_sHwReference_t   *psHwReference,
                             uint8_t    uiCmd,
                             uint8_t   *pBufferIn,
                             uint16_t  uiBufferInSize,
                             uint8_t   *pBufferOut,
                             uint16_t  *puiBufferOutSize,
                             uint16_t  *pLen,
                             uint32_t   nTimeout);

NFCSTATUS phTalTama_Protocol2(phHal_sHwReference_t  *psHwReference,
                              uint8_t   *pParam,
                              uint8_t   pParamSize,
                              uint8_t   *pDataIn,
                              uint16_t  pDataInSize,
                              uint8_t   *pDataOut,
                              uint16_t  *pDataOutSize,
                              uint16_t  *pLen,
                              uint32_t   nTimeout);

NFCSTATUS phTalTama_WakeUp(phHal_sHwReference_t   *psHwReference,
                             uint8_t    uiCmd,
                             uint8_t   *pBufferIn,
                             uint16_t   uiBufferInSize,
                             uint8_t   *pBufferOut,
                             uint16_t  *puiBufferOutSize,
                             uint32_t   nTimeout);

uint8_t   chipFirmwareVerCompare(phHal_sHwReference_t   *psHwReference, char const * const op, uint8_t ver, uint8_t rev);
void fillStartUp212_424(phHal_sPassive212_424Startup_t *startupToFill, uint8_t *dataBuffer);
void fillStartUp106(phHal_sHwReference_t *psHwReference, phHal_sPassive106Startup_t *startupToFill, uint8_t *dataBuffer);
void fillNfcProtocol(phHal_NfcProtocol_t *NfcProtocolToFill, uint8_t *dataBuffer, uint8_t dataBufferSize);
void fillIso14443A(phHal_sISO_14443_4A_Protocol_t* isoCardInfo, uint8_t *ATSBuffer);
void fillIso14443B(phHal_sISO_14443_4B_Protocol_t *protocolToFill, uint8_t *dataBuffer);
void fillStartUpJewel(phHal_sJewelStartup_t *startupToFill, uint8_t *dataBuffer);

NFCSTATUS pollActive(phHal_sHwReference_t           *psHwReference,
                     phHal_eOpModes_t                baudrate,
                     phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                     uint8_t                        *nbTargetFound,
                     phHal_sDevInputParam_t         *puDevInputParam );
     
NFCSTATUS pollPassive106A(phHal_sHwReference_t           *psHwReference,
                          phHal_eOpModes_t               *pollType,
                          phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                          uint8_t                        *pNbrOfRemoteDev,
                          uint8_t                        *nbTargetFound,
                          phHal_sDevInputParam_t         *puDevInputParam );

NFCSTATUS pollPassive106B(phHal_sHwReference_t           *psHwReference,
                         phHal_eOpModes_t                pollType,
                         phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                         uint8_t                        *pNbrOfRemoteDev,
                         uint8_t                        *nbTargetFound,
                         phHal_sDevInputParam_t         *puDevInputParam );

NFCSTATUS pollPassive106Jewel(phHal_sHwReference_t           *psHwReference,
                              phHal_eOpModes_t                pollType,
                              phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                              uint8_t                        *pNbrOfRemoteDev,
                              uint8_t                        *nbTargetFound,
                              phHal_sDevInputParam_t         *puDevInputParam );
     
NFCSTATUS pollPassive212_424(phHal_sHwReference_t           *psHwReference,
                             phHal_eOpModes_t                pollType,
                             phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                             uint8_t                        *pNbrOfRemoteDev,
                             uint8_t                        *nbTargetFound,
                             phHal_sDevInputParam_t         *puDevInputParam );
     
#ifdef AUTO_POLL
NFCSTATUS autoPoll(phHal_sHwReference_t           *psHwReference,
                   phHal_eOpModes_t                OpModes[],
                   phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                   uint8_t                        *pNbrOfRemoteDev,
                   phHal_sDevInputParam_t         *puDevInputParam );
#endif /* AUTO_POLL */
     
/*	
 *  END: Internally Used Function	
 */



/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Open( phHal_sHwReference_t *psHwReference)
{       
    NFCSTATUS	result = NFCSTATUS_SUCCESS;
    uint16_t	OutputBufferSize = MAX_SIZE_TAMA_BUFFER;
    uint8_t		outputBuffer[MAX_SIZE_TAMA_BUFFER];
	uint8_t		RfConf_buffer[12];
	uint16_t	RfConf_buffer_sz	=	0;
	uint8_t		tamaMobile			=	0;
    uint16_t    LEN                 =   0;

    /* Configuration of TAMA */
    result = phDalNfc_Open(psHwReference);

    if(result != NFCSTATUS_SUCCESS)
        return result;

	/*	First, get the version number */
	result = phTalTama_Protocol(psHwReference,
								GET_FIRMWARE_VERSION_CMD,
								NULL,
								0,
								outputBuffer,
								&OutputBufferSize,
                                &LEN,
								300);
 
	if (result != NFCSTATUS_SUCCESS) 
    {
		phDalNfc_Close(psHwReference);
        return result;
    }
	
	if(0x0b	==	OutputBufferSize)
	{
		/*	TAMA PN531	*/
		tamaMobile	=	0;
		psHwReference->ChipName		        =	phHal_eChipName531;
	    psHwReference->FirmwareVersion		=	outputBuffer[7];
	    psHwReference->FirmwareRevision		=	outputBuffer[8];

    	/*	Get the embeded JOINER ChipRevision number */
	{
		uint8_t buffer[] = {0x63, 0x27};

		result =  phTalTama_Protocol(psHwReference,
									0x06,
									buffer,
									2,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);
		if (result != NFCSTATUS_SUCCESS)
        {
            phDalNfc_Close(psHwReference);		
            return result;
        }
		psHwReference->ChipRevision	=	outputBuffer[7];

        }

	}
	else
	if(0x0d	==	OutputBufferSize)
	{
		/*	TAMA Mobile PN532	*/
		tamaMobile	=	1;
        psHwReference->FirmwareVersion		=	outputBuffer[8];
	    psHwReference->FirmwareRevision		=	outputBuffer[9];
		psHwReference->ChipName		        =	phHal_eChipName532;
    	/*	Get the embeded JOINER ChipRevision number */
        psHwReference->ChipRevision         =	outputBuffer[7];

	}
	else
		return NFCSTATUS_INVALID_DEVICE;

	
	if(tamaMobile	||	chipFirmwareVerCompare(psHwReference, ">=", 4, 1))
		RfConf_buffer_sz	=	12;
    else
        RfConf_buffer_sz	=	11;

	if(!tamaMobile	&&	chipFirmwareVerCompare(psHwReference, "<", 3, 3))
	{
		/* Set TimeOut Value to non infinite */
		RfConf_buffer[0] = 0x02;
		RfConf_buffer[1] = 0x00;
		RfConf_buffer[2] = 0x0B;
		RfConf_buffer[3] = 0x0A;
		OutputBufferSize = sizeof(outputBuffer);
		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									4,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);

		if(result != NFCSTATUS_SUCCESS) 
        {
            phDalNfc_Close(psHwReference);		
            return result;
        }

	}
	
	/* Set Max_Retry */

    RfConf_buffer[0] = 0x05;
    RfConf_buffer[1] = ATR_SEND_MAX_RETRY;
    RfConf_buffer[2] = PSL_SEND_MAX_RETRY;

	if(tamaMobile	||	chipFirmwareVerCompare(psHwReference, ">", 3, 3))
		RfConf_buffer[3] = 0x05; /* Max retry target passive activation */

    OutputBufferSize = sizeof(outputBuffer);
	if(tamaMobile	||	chipFirmwareVerCompare(psHwReference, ">", 3, 3))
	{
		
		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									4,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);
	}
	else
	{
		
		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									3,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);
	}
    
    if(result != NFCSTATUS_SUCCESS) 
    {
        phDalNfc_Close(psHwReference);     
        return result;
    }

    /* Analog Settings */

	if(!tamaMobile)
	{
		/* set for targe side */
		RfConf_buffer[0] = 0x06;
		RfConf_buffer[1] = TM_RF_CFG_REG;
		RfConf_buffer[2] = TM_GS_N_REG;
		RfConf_buffer[3] = TM_CW_GS_P_REG;
		RfConf_buffer[4] = TM_MOD_GS_P_REG;
		RfConf_buffer[5] = TM_MOD_WIDTH_REG;
		RfConf_buffer[6] = TM_TX_BIT_PHASE_REG;
		RfConf_buffer[7] = TM_RX_SEL_REG;
		RfConf_buffer[8] = TM_PASSIVE_DEMOD_REG;
		RfConf_buffer[9] = TM_RX_THRESHOLD_REG;
		RfConf_buffer[10]= TM_ACTIVE_DEMOD_REG;

		if(chipFirmwareVerCompare(psHwReference, ">=", 4, 1))
			RfConf_buffer[11]= TM_GSNLOAD_MOD_REG ;

		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									RfConf_buffer_sz,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);

		if(result != NFCSTATUS_SUCCESS) 
        {
            phDalNfc_Close(psHwReference);		
            return result;
        }

		/* set for initiator side */
		RfConf_buffer[0] = 0x07;
		RfConf_buffer[1] = IM_RF_CFG_REG;
		RfConf_buffer[2] = IM_GS_N_REG;
		RfConf_buffer[3] = IM_CW_GS_P_REG;
		RfConf_buffer[4] = IM_MOD_GS_P_REG;
		RfConf_buffer[5] = IM_MOD_WIDTH_REG;
		RfConf_buffer[6] = IM_TX_BIT_PHASE_REG;
		RfConf_buffer[7] = IM_RX_SEL_REG;
		RfConf_buffer[8] = IM_PASSIVE_DEMOD_REG;
		RfConf_buffer[9] = IM_RX_THRESHOLD_REG;
		RfConf_buffer[10]= IM_ACTIVE_DEMOD_REG;
	    
 		if(chipFirmwareVerCompare(psHwReference, ">=", 4, 1))
			RfConf_buffer[11]= IM_GSNLOAD_MOD_REG ;
	 
		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									RfConf_buffer_sz,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);
    
	}
	else
	{


		/*	for the 106 kbps baudrate, type A	*/
		RfConf_buffer[0]	= 0x0A;
		RfConf_buffer[1]	= TMM_0x0A_RF_CFG_REG;
		RfConf_buffer[2]	= TMM_0x0A_GS_N_ON_REG;
		RfConf_buffer[3]	= TMM_0x0A_CW_GS_P_REG;
		RfConf_buffer[4]	= TMM_0x0A_MOD_GS_P_REG;
		RfConf_buffer[5]	= TMM_0x0A_DMOD_RF_ON_REG;
		RfConf_buffer[6]	= TMM_0x0A_RX_THRESHOLD_REG;
		RfConf_buffer[7]	= TMM_0x0A_DMOD_RF_OFF_REG;
		RfConf_buffer[8]	= TMM_0x0A_GS_N_OFF_REG;
		RfConf_buffer[9]	= TMM_0x0A_MOD_WIDTH_REG;
		RfConf_buffer[10]	= TMM_0x0A_MIFARE_REG;
		RfConf_buffer[11]	= TMM_0x0A_TX_BITPHASE_REG;

		RfConf_buffer_sz	=	12;

		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									RfConf_buffer_sz,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);

		if(result != NFCSTATUS_SUCCESS)
        {
            phDalNfc_Close(psHwReference);		
            return result;
        }

		/*	for the 212/424 kbps baudrate */

		RfConf_buffer[0]	= 0x0B;
		RfConf_buffer[1]	= TMM_0x0B_RF_CFG_REG;
		RfConf_buffer[2]	= TMM_0x0B_GS_N_ON_REG;
		RfConf_buffer[3]	= TMM_0x0B_CW_GS_P_REG;
		RfConf_buffer[4]	= TMM_0x0B_MOD_GS_P_REG;
		RfConf_buffer[5]	= TMM_0x0B_DMOD_RF_ON_REG;
		RfConf_buffer[6]	= TMM_0x0B_RX_THRESHOLD_REG;
		RfConf_buffer[7]	= TMM_0x0B_DMOD_RF_OFF_REG;
		RfConf_buffer[8]	= TMM_0x0B_GS_N_OFF_REG;

		RfConf_buffer_sz	=	9;

		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									RfConf_buffer_sz,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);

		if(result != NFCSTATUS_SUCCESS)
        {
            phDalNfc_Close(psHwReference);		
            return result;
        }

		/*	for the type B */

		RfConf_buffer[0]	= 0x0C;
		RfConf_buffer[1]	= TMM_0x0C_GS_N_ON_REG;
		RfConf_buffer[2]	= TMM_0x0C_MOD_GS_P_REG;
		RfConf_buffer[3]	= TMM_0x0C_RX_THRESHOLD_REG;

		RfConf_buffer_sz	=	4;

		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									RfConf_buffer_sz,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);

		/*	If not supported, this might be because this is a PN5321, and not a PN5320
		if(result != NFCSTATUS_SUCCESS)
			return result;	*/

		/*	for 212/424 (/848) with ISO14443-4 (T=CL) cards*/


		RfConf_buffer[0]	= 0x0D;
		RfConf_buffer[1]	= TMM_0x0D_RX_THRESHOLD_REG;
		RfConf_buffer[2]	= TMM_0x0D_MOD_WIDTH_REG_1;
		RfConf_buffer[3]	= TMM_0x0D_MIFARE_REG_1;
		RfConf_buffer[4]	= TMM_0x0D_RX_THRESHOLD_REG;
		RfConf_buffer[5]	= TMM_0x0D_MOD_WIDTH_REG_2;
		RfConf_buffer[6]	= TMM_0x0D_MIFARE_REG_2;
		RfConf_buffer[7]	= TMM_0x0D_RX_THRESHOLD_REG;
		RfConf_buffer[8]	= TMM_0x0D_MOD_WIDTH_REG_3;
		RfConf_buffer[9]	= TMM_0x0D_MIFARE_REG_3;
	
		RfConf_buffer_sz	=	10;

		result = phTalTama_Protocol(psHwReference,
									RF_CONFIGURATION,
									RfConf_buffer,
									RfConf_buffer_sz,
									outputBuffer,
									&OutputBufferSize,
                                    &LEN,
									0);

	}
    
    if(result != NFCSTATUS_SUCCESS)
    {
        phDalNfc_Close(psHwReference);		
    }
     
    return result; 
}
          
/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_GetDeviceCapabilities( phHal_sHwReference_t          *psHwReference,
                                                  phHal_sDeviceCapabilities_t   *psDevCapabilities)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t pRecvBuf[20];
    uint16_t ui16RecvSize = 20;
	uint8_t	tamaMobile	  =	0;
    uint16_t    LEN                 =   0;

    if((psHwReference == NULL) || (psDevCapabilities == NULL))
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER); 
    
    /* check validity of the link */
    if(psHwReference->Flags.LinkValid != 1)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE );

	if(phHal_eChipName532	==	psHwReference->ChipName)
		tamaMobile	=	1;

    psDevCapabilities->InitiatorSupProtocol.Felica      = 1;
    psDevCapabilities->InitiatorSupProtocol.ISO14443_4A = 1;
	psDevCapabilities->InitiatorSupProtocol.ISO14443_4B = 0;
    psDevCapabilities->InitiatorSupProtocol.MifareStd   = 1;
    psDevCapabilities->InitiatorSupProtocol.MifareUL    = 1;
    psDevCapabilities->InitiatorSupProtocol.NFC         = 1;
    psDevCapabilities->InitiatorSupProtocol.Jewel       = 0;

    psDevCapabilities->TargetSupProtocol.Felica         = 0;
    psDevCapabilities->TargetSupProtocol.ISO14443_4A    = 0;
	psDevCapabilities->TargetSupProtocol.ISO14443_4B    = 0;
    psDevCapabilities->TargetSupProtocol.MifareStd      = 0;
    psDevCapabilities->TargetSupProtocol.MifareUL       = 0;
    psDevCapabilities->TargetSupProtocol.NFC            = 1;
    psDevCapabilities->TargetSupProtocol.Jewel          = 0;

    psDevCapabilities->psHwReference = psHwReference;

    psDevCapabilities->MaxNumberOfRemoteDev = 2;
	if(!tamaMobile)
	    psDevCapabilities->MTU = MAX_SIZE_TAMA_BUFFER;
	else
		psDevCapabilities->MTU = MAX_SIZE_TAMAMOBILE_BUFFER;

    psDevCapabilities->MultiPollSessionSup = 0; 
    psDevCapabilities->TargetCascadeLevel = 1;

    /* send GetFirmwareVersion to TAMA */
    result = phTalTama_Protocol(psHwReference,
                                GET_FIRMWARE_VERSION_CMD,
                                NULL,
                                0,
                                pRecvBuf,
                                &ui16RecvSize,
                                &LEN,
                                300);

    if (result != NFCSTATUS_SUCCESS)
        return result;

	/*	For portability reasons, strlen( x ) and strcpy(x, y) might only accept char* as the type of x and y
		Indeed, signed char and unsigned char may yield to compilation error ; thus, let's use char* instead of int8_t*
	*/
    strcpy((char*)psDevCapabilities->VendorName.Name,PHTAL_T_VD_NAME);
	psDevCapabilities->VendorName.Length	=	(uint8_t)strlen((char*)psDevCapabilities->VendorName.Name);

	if(tamaMobile)
	{
		psDevCapabilities->InitiatorSupProtocol.ISO14443_4A	= (pRecvBuf[10]	& 0x01);
		psDevCapabilities->InitiatorSupProtocol.ISO14443_4B	= (pRecvBuf[10]	& 0x02)	>> 1;
		psDevCapabilities->InitiatorSupProtocol.NFC			= (pRecvBuf[10]	& 0x04)	>> 2;
		psDevCapabilities->InitiatorSupProtocol.Jewel		= 1;
	}

	return result;
}


/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Poll(phHal_sHwReference_t           *psHwReference,
                                phHal_eOpModes_t                OpModes[],
                                phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                                uint8_t                        *pNbrOfRemoteDev,
                                phHal_sDevInputParam_t         *puDevInputParam)
     
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t OutputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t OutputBufferSize = sizeof(OutputBuffer);
    uint16_t    LEN                 =   0;
    
    uint8_t nbTargetFound = 0;
    /* Set  time out (to default before optimisation) */
    uint8_t Tama_RfConf[2];
     
    uint8_t i = 0;

    /*  *****************************
        parameter verification
        *****************************
     */

    if((psHwReference == NULL))
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
    }

    if(psHwReference->Flags.LinkValid == 0)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);
    
    if( OpModes == NULL 
        || psRemoteDevInfoList == NULL 
        || pNbrOfRemoteDev == NULL 
        || puDevInputParam == NULL 
        || (*pNbrOfRemoteDev == 0)
        || OpModes[0] == phHal_eOpModesArrayTerminator)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
    }
     
    for (i=0;i< (*pNbrOfRemoteDev);i++)
    {
        if (psRemoteDevInfoList[i].SessionOpened == 1)
        {
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_MULTI_POLL_NOT_SUPPORTED);
        }
    }
    if(*pNbrOfRemoteDev>2)
        *pNbrOfRemoteDev    =   2;

    /*  *****************************
        detection
        *****************************
     */

#ifdef AUTO_POLL
	if (psHwReference->ChipName == phHal_eChipName532)
    {
        autoPoll (psHwReference,
                  OpModes,
                  psRemoteDevInfoList,
                  pNbrOfRemoteDev,
                  puDevInputParam);
    }
    else
#endif
    {
        i=0;
        while((OpModes[i] != phHal_eOpModesArrayTerminator) && (nbTargetFound == 0) )
        {
            /*  
             * before each poll, we have to stop and restart the RF Field
             * because after a inListPassiveTarget, TAMA go into a state where no more 
             * command can be passed. Stopping and restarting the RF field re-init the communication, 
             * and we can re-poll the target.
             */
    
            /* halt RF */
            Tama_RfConf[0] = 0x01;
            Tama_RfConf[1] = 0x02;
        
    
            OutputBufferSize = sizeof(OutputBuffer);
            result = phTalTama_Protocol(psHwReference,
                                        RF_CONFIGURATION,
                                        Tama_RfConf,
                                        2,
                                        OutputBuffer,
                                        &OutputBufferSize,
                                        &LEN,
                                        0);
    
            if(result != NFCSTATUS_SUCCESS)
            {
                return result;
            }
       
            /* RF will be restarted with the next TAMA command */
    
            switch(OpModes[i])
            {
                /* ------------------------------------------------------------ */
                case phHal_eOpModesNfcPassive106 :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesNfcPassive106 selected\n");
#endif
                    {
                        phHal_eOpModes_t tempOpMode[] = {phHal_eOpModesNfcPassive106,phHal_eOpModesArrayTerminator};
                        result = pollPassive106A(psHwReference,
                                                 tempOpMode,
                                                 psRemoteDevInfoList,
                                                 pNbrOfRemoteDev,
                                                 & nbTargetFound,
                                                 puDevInputParam);
                        
                        if (result!=NFCSTATUS_SUCCESS 
                            && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                            return result;
                    }
                    break;
                    /* ------------------------------------------------------------ */
                case phHal_eOpModesMifare :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesMifare selected\n");
#endif
                    {
                        phHal_eOpModes_t tempOpMode[] = {phHal_eOpModesMifare,phHal_eOpModesArrayTerminator};
                        result = pollPassive106A(psHwReference,
                                                 tempOpMode ,
                                                 psRemoteDevInfoList,
                                                 pNbrOfRemoteDev,
                                                 & nbTargetFound,
                                                 puDevInputParam);
                        
                        if (result!=NFCSTATUS_SUCCESS 
                            && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                            return result;
                    }
                    break;
                    /* ------------------------------------------------------------ */
                case phHal_eOpModesISO14443_4A :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesISO14443_4A selected\n");
#endif
                    {
                        phHal_eOpModes_t tempOpMode[] = {phHal_eOpModesISO14443_4A,phHal_eOpModesArrayTerminator,phHal_eOpModesArrayTerminator};
                        if (OpModes[i+1] == phHal_eOpModesMifare)
                        {
                            tempOpMode[1] = phHal_eOpModesMifare;
                            i++;
                        }
                        result = pollPassive106A(psHwReference,
                                                 tempOpMode ,
                                                 psRemoteDevInfoList,
                                                 pNbrOfRemoteDev,
                                                 & nbTargetFound,
                                                 puDevInputParam);
              
                        if (result!=NFCSTATUS_SUCCESS 
                            && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                            return result;
                    }
                    break;
              
                    //------------------------------------------------------------
                case phHal_eOpModesISO14443_4B :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesISO14443_4B selected\n");
#endif
                    result = pollPassive106B(psHwReference,
                                            phHal_eOpModesISO14443_4B,
                                            psRemoteDevInfoList,
                                            pNbrOfRemoteDev,
                                            & nbTargetFound,
                                            puDevInputParam);
              
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
         
                    break;
/*- MantisID:320 - JG - 09-10-2006 */
              
                    //------------------------------------------------------------
                case phHal_eOpModesJewel :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesJewel selected\n");
#endif
                    result = pollPassive106Jewel(psHwReference,
                                                 phHal_eOpModesJewel,
                                                 psRemoteDevInfoList,
                                                 pNbrOfRemoteDev,
                                                 & nbTargetFound,
                                                 puDevInputParam);
              
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
         
                    break;
              
                    //------------------------------------------------------------
                case phHal_eOpModesNfcPassive212 :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesNfcPassive212 selected\n");
#endif
                    result = pollPassive212_424(psHwReference,
                                                phHal_eOpModesNfcPassive212,
                                                psRemoteDevInfoList,
                                                pNbrOfRemoteDev,
                                                & nbTargetFound,
                                                puDevInputParam);
              
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
              
                    break;
                    /* ------------------------------------------------------------ */
                case phHal_eOpModesFelica212 :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesFelica212 selected\n");
#endif
                    result = pollPassive212_424(psHwReference,
                                                phHal_eOpModesFelica212,
                                                psRemoteDevInfoList,
                                                pNbrOfRemoteDev,
                                                & nbTargetFound,
                                                puDevInputParam);
                  
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
              
                    break;
                    /* ------------------------------------------------------------ */
                case phHal_eOpModesNfcPassive424 :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesNfcPassive424 selected\n");
#endif
                    
                    result = pollPassive212_424(psHwReference,
                                                phHal_eOpModesNfcPassive424,
                                                psRemoteDevInfoList,
                                                pNbrOfRemoteDev,
                                                & nbTargetFound,
                                                puDevInputParam);
              
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
              
                    break;
                    /* ------------------------------------------------------------ */
                case phHal_eOpModesFelica424 :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesFelica424 selected\n");
#endif
                    result = pollPassive212_424(psHwReference,
                                                phHal_eOpModesFelica424,
                                                psRemoteDevInfoList,
                                                pNbrOfRemoteDev,
                                                & nbTargetFound,
                                                puDevInputParam);
                  
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
                    break;
                    /* ------------------------------------------------------------ */
                case phHal_eOpModesNfcActive106 :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesNfcActive106 selected\n");
#endif
                    
                    result = pollActive(psHwReference,
                                        phHal_eOpModesNfcActive106,
                                        psRemoteDevInfoList,
                                        & nbTargetFound,
                                        puDevInputParam);
              
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
              
                    break;
                    /* ------------------------------------------------------------ */
                case phHal_eOpModesNfcActive212 :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesNfcActive212 selected\n");
#endif
                                    
                    result = pollActive(psHwReference,
                                        phHal_eOpModesNfcActive212,
                                        psRemoteDevInfoList,
                                        & nbTargetFound,
                                        puDevInputParam);
              
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
         
                    break;
                    /* ------------------------------------------------------------ */
                case phHal_eOpModesNfcActive424 :
#ifdef PRINTF_DEBUG
                    rt_kprintf("---------------------------------------------------\n");
                    rt_kprintf("phHal_eOpModesNfcActive424 selected\n");
#endif
                    
                    result = pollActive(psHwReference,
                                        phHal_eOpModesNfcActive424,
                                        psRemoteDevInfoList,
                                        & nbTargetFound,
                                        puDevInputParam);
              
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                        return result;
         
                    break;
                default :
                    return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
                    break;
            } /* END : switch(OpModes[i]) */
            i++;
        } /* END :   while((OpModes[i] != phHal_eOpModesArrayTerminator) || (!targetFound) )  */
         
        *pNbrOfRemoteDev = nbTargetFound;
    }
     
    if(*pNbrOfRemoteDev == 0)
    {
        result = PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_NO_DEVICE_FOUND);
    }

    return result;
}
          
void SetTamaParameter_Initialize(       phHal_eOpModes_t               OpMode,
                                        uint8_t*                       param,
                                        uint8_t                        useDID,
                                        uint8_t                        useNAD)
{
//    assert( NULL !=  param );

    switch(OpMode)
    {
        /* passive 106/212/424 */
    case phHal_eOpModesMifare:
    case phHal_eOpModesNfcPassive106:
    case phHal_eOpModesNfcPassive212 :
    case phHal_eOpModesNfcPassive424 :
    case phHal_eOpModesFelica212:
    case phHal_eOpModesFelica424:
        *param = 0x04;
        if(useDID != 0)
        {
                *param |= 0x02;
        }
        
        if((useNAD != 0))
        {
                *param |= 0x01;
        }
         break;
        /* Active */
    case phHal_eOpModesNfcActive106:
    case phHal_eOpModesNfcActive212:
    case phHal_eOpModesNfcActive424:
        *param = 0x14;
        if(useDID!= 0)
        {
           *param |= 0x02;
        }
    
        if(useNAD != 0)
        {
            *param |= 0x01;
        }



        break;
        /* ISO14443-4 */
    case phHal_eOpModesISO14443_4A:
    case phHal_eOpModesISO14443_4B:
        *param = 0x14;
		break;
    case phHal_eOpModesJewel:
        *param = 0x04;
        break;

        /* Default */
    default:
//        assert(0);
        break;
    }   //  switch

}


     
/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Connect(phHal_sHwReference_t          *psHwReference,
                                   phHal_eOpModes_t               OpMode,
                                   phHal_sRemoteDevInformation_t *psRemoteDevInfo,
                                   phHal_sDevInputParam_t        *puDevInputParam)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t outBuff[20];
    uint16_t outBuffSize = 20;
    uint16_t    LEN                 =   0;

    /* check parameter validity */
    if (    NULL    ==  psRemoteDevInfo 
        ||  NULL    ==  psHwReference
        ||  NULL    ==  puDevInputParam )
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
    }

    if (psHwReference->Flags.LinkValid == 0x00)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);
    }

    if((psRemoteDevInfo->OpMode != OpMode) 
        || (OpMode == phHal_eOpModesInvalid)
        || (psRemoteDevInfo->OpMode  == phHal_eOpModesInvalid))
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
    }

    if (psRemoteDevInfo->SessionOpened != 0x00)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_ALREADY_CONNECTED);
    }
    else 	
	if(		(phHal_eChipName531	==	psHwReference->ChipName)
		&&	(chipFirmwareVerCompare(psHwReference, "<=", 3, 4)))
	{
		psRemoteDevInfo->SessionOpened = 0x01;
	}
	else
    {
        result = phTalTama_Protocol(psHwReference,
                                    IN_SELECT,
                                    & psRemoteDevInfo->hLogHandle,
                                    1,
                                    outBuff,
                                    &outBuffSize,
                                    &LEN,
                                    0);
        if(result == NFCSTATUS_SUCCESS)
        {

            switch (outBuff[7])
            {
                case 0x00 :
                    psRemoteDevInfo->SessionOpened = 0x01;
                    break;
                case 0x01 :
                    result =   PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_RF_TIMEOUT);
                    break;
                case 0x27:
                    psRemoteDevInfo->SessionOpened = 0x01;
                    break;
                default:
                    result =   PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_CMD_ABORTED);
                    break;
            }

        }
    }
    return result;
}
     
     
/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Transceive(phHal_sHwReference_t           *psHwReference,
                                      phHal_sRemoteDevInformation_t  *psRemoteDevInfo,
                                      phHal_uCmdList_t                Cmd,
                                      phHal_sDepAdditionalInfo_t     *psDepAdditionalInfo,
                                      uint8_t                        *pSendBuf,
                                      uint16_t                        SendLength,
                                      uint8_t                        *pRecvBuf,
                                      uint16_t                       *pRecvLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t  pTempBuf[MAX_SIZE_DAL_BUFFER];
    uint8_t  tab[3];
    uint8_t  TgId;
    uint16_t    LEN                 =   0;
    uint16_t uiTempLength = MAX_SIZE_DAL_BUFFER;
	uint16_t MAXSizeBuffer=	MAX_SIZE_TAMA_BUFFER;
    uint8_t	 tamaMobile	=	(phHal_eChipName532	==	psHwReference->ChipName ? 1:0);
    uint8_t  outputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t OutputBufferSize = sizeof(outputBuffer);


    /* check parameters validity */
    if((psHwReference == NULL) || (psRemoteDevInfo == NULL) || (psDepAdditionalInfo == NULL)) 
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);

    /* check validity of the link to the device */
    if(psHwReference->Flags.LinkValid != 1)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE) ;

    /* check validity of the remote device */
    if (psRemoteDevInfo->SessionOpened == 0)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE) ;

	if(	tamaMobile)
	{
		MAXSizeBuffer	=	MAX_SIZE_TAMAMOBILE_BUFFER;
	}

    TgId = (uint8_t)psRemoteDevInfo->hLogHandle;


    /* Raw Command: the caller is responsible of its validity/protocol encapsulation */
    if (Cmd.RawCmd == phHal_eRawCmdListRaw)
    {
        /* check parameters validity */
        if(     (pSendBuf       == NULL) 
            ||  (pRecvLength    == NULL) 
            ||  ((pRecvBuf      == NULL)&&(*pRecvLength != 0))
            ||  (SendLength     >  MAXSizeBuffer)
           )
        {
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
        }

        result = phTalTama_Protocol(psHwReference,
                                    IN_COMMUNICATE_THRU_CMD,
                                    pSendBuf,
                                    SendLength,
                                    pTempBuf,
                                    &uiTempLength,
                                    &LEN,
                                    0);
    }
    else if((Cmd.Iso144434Cmd == phHal_eIso14443_4_CmdListPPSCmd) && 
			 ((psRemoteDevInfo->OpMode == phHal_eOpModesISO14443_4A) || (psRemoteDevInfo->OpMode == phHal_eOpModesISO14443_4B)))
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_COMMAND_NOT_SUPPORTED);
    }
    else if((Cmd.Iso144434Cmd == phHal_eIso14443_4_CmdListPresenceCheckCmd) && 
			 ((psRemoteDevInfo->OpMode == phHal_eOpModesISO14443_4A) || (psRemoteDevInfo->OpMode == phHal_eOpModesISO14443_4B)))
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_COMMAND_NOT_SUPPORTED);
    }
    /* NfcPSL command for an NFC (active or passive) OpMode */
    else if ((Cmd.NfcInCmd == phHal_eNfcCmdListNfcPSL) 
           && (psRemoteDevInfo->OpMode != phHal_eOpModesMifare)
           && (psRemoteDevInfo->OpMode != phHal_eOpModesISO14443_4A)
           && (psRemoteDevInfo->OpMode != phHal_eOpModesISO14443_4B)
           && (psRemoteDevInfo->OpMode != phHal_eOpModesJewel)
           && (psRemoteDevInfo->OpMode != phHal_eOpModesFelica212)
           && (psRemoteDevInfo->OpMode != phHal_eOpModesFelica424))

    {
        /* data contained in pSendBuf don't include target ID */
        tab[0] = TgId;

        if(     (pSendBuf       == NULL) 
            ||  (pRecvLength    == NULL)
            ||  ((pRecvBuf      == NULL)    &&  (   *pRecvLength != 0))
           )
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);

        if (pSendBuf[0] >2 || pSendBuf[1] >2 )
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
        tab[1] = pSendBuf[0];
        tab[2] = pSendBuf[1];

        result = phTalTama_Protocol(psHwReference,
                                    IN_PSL_CMD,  
                                    tab,
                                    3,
                                    pTempBuf,
                                    &uiTempLength,
                                    &LEN,
                                    0);
    }
    else if (    ( (Cmd.NfcInCmd == phHal_eNfcCmdListNfcDEP)      && (   (psRemoteDevInfo->OpMode == phHal_eOpModesNfcActive106)
                                                                       ||(psRemoteDevInfo->OpMode == phHal_eOpModesNfcActive212)
                                                                       ||(psRemoteDevInfo->OpMode == phHal_eOpModesNfcActive424)
                                                                       ||(psRemoteDevInfo->OpMode == phHal_eOpModesNfcPassive106)
                                                                       ||(psRemoteDevInfo->OpMode == phHal_eOpModesNfcPassive212)
                                                                       ||(psRemoteDevInfo->OpMode == phHal_eOpModesNfcPassive424)
                                                                      )
                  )                                                               
              ||  ( (Cmd.FelCmd == phHal_eFelicaCmdListFelicaCmd) && (   (psRemoteDevInfo->OpMode == phHal_eOpModesFelica212)
                                                                       ||(psRemoteDevInfo->OpMode == phHal_eOpModesFelica424)
                                                                      ) 
                  )
              ||  ( (Cmd.Iso144434Cmd == phHal_eIso14443_4_CmdListTClCmd) && (psRemoteDevInfo->OpMode == phHal_eOpModesISO14443_4A)
                  )
              ||  ( (Cmd.Iso144434Cmd == phHal_eIso14443_4_CmdListTClCmd) && (psRemoteDevInfo->OpMode == phHal_eOpModesISO14443_4B)
				  )
			)
    {
        uint8_t NADSup_rem =   0;  /* NAD supported in the remote device  */
        uint8_t NADSup_loc =   0;  /* NAD supported in the Transceive function */
        uint8_t Tama_SetTamaParam[] = {0x14};
        
        NADSup_loc  =   psDepAdditionalInfo->DepFlags.NADPresent;

        /* check parameters validity */
        if(     (pSendBuf   == NULL)
            || (pRecvLength == NULL)
            || ((pRecvBuf   == NULL)    &&  (*pRecvLength != 0) )
            || (SendLength  > MAXSizeBuffer)
          )
        {
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
        }

        /* Check the usage of NAD at remote size */

        switch(psRemoteDevInfo->OpMode)
        {
        case phHal_eOpModesNfcPassive106:
            NADSup_rem =   psRemoteDevInfo->RemoteDevInfo.NfcInfo106.NfcProtocol.NADtSup;
            break;
        case phHal_eOpModesNfcPassive212:
        case phHal_eOpModesNfcPassive424:
            NADSup_rem =   psRemoteDevInfo->RemoteDevInfo.NfcInfo212_424.NfcProtocol.NADtSup;
            break;
        case phHal_eOpModesNfcActive106:
        case phHal_eOpModesNfcActive212:
        case phHal_eOpModesNfcActive424:
            NADSup_rem =   psRemoteDevInfo->RemoteDevInfo.NfcActiveInfo.NfcProtocol.NADtSup;
            break;
        case phHal_eOpModesISO14443_4A:
            NADSup_rem =   psRemoteDevInfo->RemoteDevInfo.ISO14443_4A_Info.ISO14443_4A_Protocol.NADUsed;
            break;
        case phHal_eOpModesISO14443_4B:
            NADSup_rem =   psRemoteDevInfo->RemoteDevInfo.ISO14443_4B_Info.ISO14443_4B_Protocol.NADUsed;
            break;
        default:
            NADSup_rem   =   0;
            NADSup_loc   =   0;
            break;
        }   /* switch */

        /* check consistency of the usage of the NAD */
        if(      (0 != NADSup_loc)
            &&  (0  == NADSup_rem)  
            &&  (psRemoteDevInfo->OpMode != phHal_eOpModesJewel)
            &&  (psRemoteDevInfo->OpMode != phHal_eOpModesFelica212) 
            &&  (psRemoteDevInfo->OpMode != phHal_eOpModesFelica424)
            )
        {
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
        }

        /* Add MI information for DEP for TAMA 3.4 and above */

	    if(	    (tamaMobile	||	chipFirmwareVerCompare(psHwReference, ">", 3, 3)) 
            &&  (psDepAdditionalInfo->DepFlags.MetaChaining) 
            &&  (psRemoteDevInfo->OpMode != phHal_eOpModesISO14443_4A) 
            &&  (psRemoteDevInfo->OpMode != phHal_eOpModesISO14443_4B) 
            &&  (psRemoteDevInfo->OpMode != phHal_eOpModesJewel) 
            &&  (psRemoteDevInfo->OpMode != phHal_eOpModesFelica212)  
            &&  (psRemoteDevInfo->OpMode != phHal_eOpModesFelica424)
              )
        {
            TgId = TgId | MI_MASK;
        }

        /* Check whether the caller requests the usage of the NAD for this local NFC chip */
        if( NADSup_loc )
        {
            /* We Initialize the Tama_SetTamaParam *without* DID, and *with* the NAD */
            SetTamaParameter_Initialize(psRemoteDevInfo->OpMode, &Tama_SetTamaParam[0], 0, 1);

           OutputBufferSize = sizeof(outputBuffer);
            result = phTalTama_Protocol(psHwReference,
                        SET_TAMA_PARAMETERS,
                        Tama_SetTamaParam,
                        1,
                        outputBuffer,
                        &OutputBufferSize,
                        &LEN,
                        0);
    
            if(result != NFCSTATUS_SUCCESS)
            {
                return result;
            }

            tab[0] = IN_DATA_EXCHANGE_CMD;
            tab[1] = TgId;
            tab[2] = psDepAdditionalInfo->NAD;

            if((SendLength + 1) > MAXSizeBuffer)
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);

            result = phTalTama_Protocol2(psHwReference,
                                         tab,
                                         3,
                                         pSendBuf,
                                         (uint16_t)(SendLength),
                                         pTempBuf,
                                         &uiTempLength,
                                         &LEN,
                                         psHwReference->UserTimeout);
            /* We Initialize the Tama_SetTamaParam back to its original value */
            SetTamaParameter_Initialize(psRemoteDevInfo->OpMode, &Tama_SetTamaParam[0], psRemoteDevInfo->UsedDID,psRemoteDevInfo->UsedNAD);
            
            OutputBufferSize = sizeof(outputBuffer);
            phTalTama_Protocol( psHwReference,
                                SET_TAMA_PARAMETERS,
                                Tama_SetTamaParam,
                                1,
                                outputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                0);
        }
        else
        {
            tab[0] = IN_DATA_EXCHANGE_CMD;
            tab[1] = TgId;

            result = phTalTama_Protocol2(psHwReference,
                                         tab,
                                         2,
                                         pSendBuf,
                                         SendLength,
                                         pTempBuf,
                                         &uiTempLength,
                                         &LEN,
                                         psHwReference->UserTimeout);
        }

    }
    else if ( psRemoteDevInfo->OpMode == phHal_eOpModesMifare )
    {
        tab[0] = IN_DATA_EXCHANGE_CMD;
        tab[1] = TgId;

        /* check parameters validity */
        if(     (pSendBuf   == NULL)
            || (pRecvLength == NULL) 
            || ((pRecvBuf   == NULL)&&(*pRecvLength != 0))
            || (SendLength > (MAXSizeBuffer-1))
           )
        {
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
        }
     
        if((Cmd.MfCmd == phHal_eMifareCmdListMifareAuthentA)
           ||   (Cmd.MfCmd == phHal_eMifareCmdListMifareAuthentB)
           )
        {
            uint8_t tempBuffer[15];
            /*  The SendLength should be checked and not be different than 7
                Adr+key = 1 + 6 = 7 bytes */
            if( SendLength != 7)
            {
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
            }

            if(Cmd.MfCmd == phHal_eMifareCmdListMifareAuthentA)
                tab[2] = 0x60;
            else if(Cmd.MfCmd == phHal_eMifareCmdListMifareAuthentB)
                tab[2] = 0x61;
            
            memcpy(tempBuffer,pSendBuf,SendLength);
            memcpy(& (tempBuffer[SendLength]),
                   psRemoteDevInfo->RemoteDevInfo.CardInfo106.Startup106.NFCID1t,
                   psRemoteDevInfo->RemoteDevInfo.CardInfo106.Startup106.NFCID1tLength);
            
            result =  phTalTama_Protocol2(psHwReference,
                                          tab,
                                          3,
                                          tempBuffer,
                                          (uint16_t)(SendLength + psRemoteDevInfo->RemoteDevInfo.CardInfo106.Startup106.NFCID1tLength),
                                          pTempBuf,
                                          &uiTempLength,
                                          &LEN,
                                          0);
        
            /*DBO Mantis 749 750*/
            if( result == NFCSTATUS_SUCCESS && (uiTempLength != 10))
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
        }
        else
        {
            uint16_t waited_lenght = 0;
            if(Cmd.MfCmd == phHal_eMifareCmdListMifareRead)
            {
                tab[2] = 0x30;
                waited_lenght = 16;
            }
            else if(Cmd.MfCmd == phHal_eMifareCmdListMifareWrite16)
            {
                tab[2] = 0xA0;
                waited_lenght = 0;
            }
            else if(Cmd.MfCmd == phHal_eMifareCmdListMifareWrite4)
            {
                tab[2] = 0xA2;
                waited_lenght = 0;
            }
            else if(Cmd.MfCmd == phHal_eMifareCmdListMifareInc)
            {
                tab[2] = 0xC1;
                waited_lenght = 0;
            }
            else if(Cmd.MfCmd == phHal_eMifareCmdListMifareDec)
            {
                tab[2] = 0xC0;
                waited_lenght = 0;
            }
            else if(Cmd.MfCmd == phHal_eMifareCmdListMifareTransfer)
            {
                tab[2] = 0xB0;
                waited_lenght = 0;
            }
            else if(Cmd.MfCmd == phHal_eMifareCmdListMifareRestore)
            {
                tab[2] = 0xC2;
                waited_lenght = 0;
            }
            else /* this command is invalid */
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
            
            result =  phTalTama_Protocol2(psHwReference,
                                          tab,
                                          3,
                                          pSendBuf,
                                          SendLength,
                                          pTempBuf,
                                          &uiTempLength,
                                          &LEN,
                                          0);
           /*DBO Mantis 749 750*/
            if((result == NFCSTATUS_SUCCESS)
                && (pTempBuf[7] == 0x00)
                && (uiTempLength != (10+waited_lenght)))
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
        }
    }
    /* else Jewel command */
    else if (psRemoteDevInfo->OpMode == phHal_eOpModesJewel)
    {
        tab[0] = IN_DATA_EXCHANGE_CMD;
        tab[1] = TgId;

        //check parameters validity
        if(     (pSendBuf   == NULL)
            || (pRecvLength == NULL) 
            || ((pRecvBuf   == NULL)&&(*pRecvLength != 0))
            || (SendLength > (MAXSizeBuffer-1))
           )
        {
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
        }
    
        switch (Cmd.JewelCmd)
        {
            case phHal_eJewelCmdListJewelRid:
                tab[2] = 0x78;
                break;
            case phHal_eJewelCmdListJewelReadAll:
                tab[2] = 0x00;
                break;
            case phHal_eJewelCmdListJewelRead1:
                tab[2] = 0x01;
                break;
            case phHal_eJewelCmdListJewelRead4:
                tab[2] = 0x02;
                break;
            case phHal_eJewelCmdListJewelRead8:
                tab[2] = 0x03;
                break;
            case phHal_eJewelCmdListJewelWriteNoErase1:
                tab[2] = 0x1A;
                break;
            case phHal_eJewelCmdListJewelWriteNoErase4:
                tab[2] = 0x1B;
                break;
            case phHal_eJewelCmdListJewelWriteNoErase8:
                tab[2] = 0x1C;
                break;
            case phHal_eJewelCmdListJewelWriteErase1:
                tab[2] = 0x53;
                break;
            case phHal_eJewelCmdListJewelWriteErase4:
                tab[2] = 0x54;
                break;
            case phHal_eJewelCmdListJewelWriteErase8:
                tab[2] = 0x55;
                break;
            default:
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
                break;
        }     

        result =  phTalTama_Protocol2(psHwReference,
                                      tab,
                                      3,
                                      pSendBuf,
                                      SendLength,
                                      pTempBuf,
                                      &uiTempLength,
                                      &LEN,
                                      0);
     } else
     {
        result = PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
     }

    if (result!=NFCSTATUS_SUCCESS)
        return result;

    /* test of status byte (00:OK) */
    if ((uiTempLength > 7)&&((pTempBuf[7] & ERROR_MASK) != 0x00))
    {
        if ((pTempBuf[7] & ERROR_MASK) == 0x01)
        {
            /*  This is an RF_TIMEOUT. After an RF_TIMEOUT, switch the field to OFF, in order to have a consistency between
             *  PN53x and PN51x.
             */

            /* halt RF */
            uint8_t Tama_RfConf[2];
        
            Tama_RfConf[0] = 0x01;
            Tama_RfConf[1] = 0x02;
        

            OutputBufferSize = sizeof(outputBuffer);
            phTalTama_Protocol( psHwReference,
                                RF_CONFIGURATION,
                                Tama_RfConf,
                                2,
                                outputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                0);

            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_RF_TIMEOUT);
        } else if ((pTempBuf[7] & ERROR_MASK) == 0x13)
        {
            // mifare error
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_RF_TIMEOUT);
        } else if ((pTempBuf[7] & ERROR_MASK) == 0x14)
        {
            // mifare error
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_RF_TIMEOUT);
        } else
        {
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
        }

        

    }

    /* Mantis 736 DBO */
    if (psDepAdditionalInfo->DepFlags.MetaChaining ==0)
    {
        uint8_t setRegisters[] = {0x01, 0xE4, 0x00};
        uint8_t outB[20] ;
        uint16_t outBSize = 20;   
        /* Reset a register value to avoid double data */
        if (psHwReference->ChipName == phHal_eChipName531)
        {
            if ((psHwReference->FirmwareVersion ==4)
                && (psHwReference->FirmwareRevision ==2))
            {
                    /*  For 531 4.2*/
                setRegisters[0] = 0x01;
                setRegisters[1] = 0xD2;
            } else if((psHwReference->FirmwareVersion ==4)
                && (psHwReference->FirmwareRevision ==2))
            {
                setRegisters[0] = 0x01;
                setRegisters[1] = 0xD3;
            }

            phTalTama_Protocol( psHwReference,
                    WRITE_REGISTER,
                    setRegisters,
                    3,
                    outB,
                    &outBSize,
                    &LEN,
                    0);

        } else
        {
            /* TAMA 532 1.4 or 1.5 other are not managed*/
            setRegisters[0] = 0x01;
            setRegisters[1] = 0xE4;
            phTalTama_Protocol( psHwReference,
                                WRITE_REGISTER,
                                setRegisters,
                                3,
                                outB,
                                &outBSize,
                                &LEN,
                                0);
        }
        /* on other chip the PR is not managed */        
    }
    /* Mantis 736 DBO */

    /* check if MI is used in the response */
    if((pTempBuf[7] & MI_MASK) == 0)
        psDepAdditionalInfo->DepFlags.MetaChaining = 0;
    else
        psDepAdditionalInfo->DepFlags.MetaChaining = 1;


    /* check if NAD is used in the response */
    if((pTempBuf[7] & NAD_MASK) == 0)
    {
        if(*pRecvLength < (uiTempLength - 10))
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_BUFFER_TOO_SMALL);
        if (uiTempLength < 10) /* this may happen if NFC device is disconnected */
            return PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_INVALID_DEVICE);

        psDepAdditionalInfo->DepFlags.NADPresent = 0;
        *pRecvLength = uiTempLength - 10;
        memcpy(pRecvBuf, pTempBuf+8, *pRecvLength);
    }
    else
    {
        if(*pRecvLength < (uiTempLength - 11))
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_BUFFER_TOO_SMALL);
        if (uiTempLength < 11) /* this may happen if NFC device is disconnected */
            return PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_INVALID_DEVICE);

        psDepAdditionalInfo->DepFlags.NADPresent = 1;
        psDepAdditionalInfo->NAD = *(pTempBuf+8);
        *pRecvLength = uiTempLength - 11;
        memcpy(pRecvBuf, pTempBuf+9, *pRecvLength);
    }

    return result;
}
     
     
     
     
/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Disconnect(phHal_sHwReference_t          *psHwReference,
                                      phHal_sRemoteDevInformation_t *psRemoteDevInfo)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t outBuff[20];
    uint16_t outBuffSize = 20;
    uint16_t    LEN                 =   0;

    if (psRemoteDevInfo ==NULL || psHwReference ==NULL)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
    }

    if (psHwReference->Flags.LinkValid == 0x00)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);
    }

    if (psRemoteDevInfo->SessionOpened == 0x00)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_NO_DEVICE_CONNECTED);
    }
    else
    {
        if(   psRemoteDevInfo->OpMode == phHal_eOpModesNfcPassive424
              || psRemoteDevInfo->OpMode == phHal_eOpModesNfcPassive212
              || psRemoteDevInfo->OpMode == phHal_eOpModesNfcPassive106
            ) 
        {
            /* In case of a nfc pasive target we do a release to completly deselect the target */
            result = phTalTama_Protocol(psHwReference,
                                        IN_RELEASE_CMD,
                                        & psRemoteDevInfo->hLogHandle,
                                        1,
                                        outBuff,
                                        &outBuffSize,
                                        &LEN,
                                        0);
                                        
        }else if(phHal_eChipName531	!=	psHwReference->ChipName)
        {
            /*  the deselect command may fail for a PN531 ; don't do it for PN531.  */
            result = phTalTama_Protocol(psHwReference,
                                        IN_DESELECT,
                                        & psRemoteDevInfo->hLogHandle,
                                        1,
                                        outBuff,
                                        &outBuffSize,
                                        &LEN,
                                        0);
            if(result == NFCSTATUS_SUCCESS)
            {
                /*  if the deselect_command works or return a time out (no more target in the RF)
                 *  close the session
                 */

                switch (outBuff[7] & ERROR_MASK)
                {
                    case 0x01 :
                    case 0x0A :
                        result =   PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_RF_TIMEOUT);
                    case 0x27 :
                    case 0x00 :
                        psRemoteDevInfo->SessionOpened = 0x00;
                        break;
                    default:
                        result =   PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_CMD_ABORTED);
                        break;
                }
            }
        }
        
        psRemoteDevInfo->SessionOpened = 0x00;

    }

    return result;
}
     
          
/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Ioctl( phHal_sHwReference_t     *psHwReference,
                                  uint16_t                  IoctlCode,
                                  uint8_t                  *pInBuf,
                                  uint16_t                  InLength,
                                  uint8_t                  *pOutBuf,
                                  uint16_t                 *pOutLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint16_t    LEN                 =   0;

    if (psHwReference == NULL)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);

    if(NULL    ==   pOutLength   &&  NULL    !=  pOutBuf)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);

    /* check if the device is opened */
    if(psHwReference->Flags.LinkValid != 1)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);

    switch(IoctlCode)
    {
        case PHHALNFC_IOCTL_PN5XX_SAM_CONFIG :
        {
            uint8_t resetCommand[] = {0x01};
            uint8_t outBuff[MAX_SIZE_DAL_BUFFER];
            uint16_t outBuffSize = MAX_SIZE_DAL_BUFFER;
            uint8_t setRegisters[] = {0x63, 0x03, 0x04, 0x63, 0x31, 0x08};
			
			// Workaround for Distance of detection - Disable the Frame Delay
			uint8_t registerAddr[] = {0x63, 0x0D}; // FD Address
			uint8_t setRegistersNFD[] = {0x63, 0x0D, 0xDF}; // FD Address and value mask
            
            /* parameters verification */
            if((pInBuf == NULL)
               ||(pInBuf[0]>0x04)
               ||(InLength < 1)
               ||(InLength > 3))
            {
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
            }

            if (pInBuf[0] == 0x04)
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_COMMAND_NOT_SUPPORTED);

            if(pInBuf[0] != 0x01)
            {
                /* switch to normal mode */
                result =  phTalTama_Protocol(psHwReference,
                                             SAM_CONFIGURATION,
                                             resetCommand,
                                             1,
                                             outBuff,
                                             &outBuffSize,
                                             &LEN,
                                             0);
            }

            if (result != NFCSTATUS_SUCCESS)
            {
                return result;
            }
            
            if ((pInBuf[0] == 0x02) 
                && (pInBuf[1] > 0x10) 
                && (phHal_eChipName531 == psHwReference->ChipName)
                )
            {
                pInBuf[1] = 0x10;
            }          

            outBuffSize = MAX_SIZE_DAL_BUFFER;
            result =  phTalTama_Protocol(psHwReference,
                                         SAM_CONFIGURATION,
                                         pInBuf,
                                         InLength,
                                         outBuff,
                                         &outBuffSize,
                                         &LEN,
                                         0);

            if (result != NFCSTATUS_SUCCESS)
                return result;

            /* PN532 v1.5 low VBat management */
            if( (pInBuf[0] == 0x02) && (phHal_eChipName532	==	psHwReference->ChipName)
                && (chipFirmwareVerCompare(psHwReference, ">=", 1, 5)))
            {
                psHwReference->Sleeping = 1;
            }
            outBuffSize = MAX_SIZE_DAL_BUFFER;

			if(		(phHal_eChipName531	==	psHwReference->ChipName)
				&&	(chipFirmwareVerCompare(psHwReference, "<", 3, 4)))
			{
				/* change some registers in order to improve synchro between TAMA & SMX */
				result =  phTalTama_Protocol(psHwReference,
											WRITE_REGISTER,
											setRegisters,
											6,
											outBuff,
											&outBuffSize,
                                            &LEN,
											0);
			}

			if(		(phHal_eChipName532 == psHwReference->ChipName)
				&&	(chipFirmwareVerCompare(psHwReference, "==", 1, 4)))
			{
				/* Workaround for the detection distance - Disable Frame delay */
				result =  phTalTama_Protocol(psHwReference,
											READ_REGISTER,
											registerAddr,
											2,
											outBuff,
											&outBuffSize,
                                            &LEN,
											0);

				setRegistersNFD[2] = setRegistersNFD[2] & outBuff[7];

				result =  phTalTama_Protocol(psHwReference,
											WRITE_REGISTER,
											setRegistersNFD,
											3,
											outBuff,
											&outBuffSize,
                                            &LEN,
											0);
			}

            /*There is no output param. So we should return zero in the length*/
            if(NULL !=  pOutLength)
                *pOutLength = 0;
        }
        break;

        case PHHALNFC_IOCTL_PN53X_GET_STATUS:
            /* verify Parameters */
            if(     (NULL       ==  pOutBuf )
                ||  (NULL       ==  pOutLength) 
                ||  (0          ==  *pOutLength)
                )
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);                

            /* send the command */
            result =  phTalTama_Protocol(psHwReference,
                                         GET_GENERAL_STATUS,
                                         NULL,
                                         0,
                                         pOutBuf,
                                         pOutLength,
                                         &LEN,
                                         0);
            break;

        case PHHALNFC_IOCTL_PN53X_CMD :
            /* verify Parameters */
            if((pInBuf == NULL)
               ||(pOutBuf == NULL)
               ||   (pOutLength ==  NULL) 
               ||(InLength == 0)
               ||((*pOutLength) == 0))
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);                

            /* send the command */
            result =  phTalTama_Protocol(psHwReference,
                                         pInBuf[0],
                                         & pInBuf[1],
                                         (uint16_t)(InLength - 1),
                                         pOutBuf,
                                         pOutLength,
                                         &LEN,
                                         0);
            /* check if the command was a POWER_DOWN request */
            if(!psHwReference->Sleeping 
                && ( phHal_eChipName532	==	psHwReference->ChipName )
                && ( chipFirmwareVerCompare(psHwReference, ">=", 1, 5) ) 
                && ( POWER_DOWN == pInBuf[0] )
                && ( NFCSTATUS_SUCCESS == result ) )
            {
                psHwReference->Sleeping = 1;
            }
            break;
    
        case PHHALNFC_IOCTL_RAW_CMD :
        {
            
            /* verify Parameters */
            if((pInBuf == NULL)
               ||(pOutBuf == NULL)
               ||   (pOutLength ==  NULL) 
               ||(InLength == 0)
               ||((*pOutLength) == 0))
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);                
            {
                uint8_t outBuff[MAX_SIZE_TAMA_BUFFER];
                uint16_t outBuffSize = *pOutLength;
                
                /* send the command */
                result =  phTalTama_Protocol(psHwReference,
                                             IN_COMMUNICATE_THRU_CMD,
                                             pInBuf,
                                             (uint16_t)(InLength),
                                             outBuff,
                                             &outBuffSize,
                                             &LEN,
                                             0);
                if(outBuff[7] ==0)
                {
                    *pOutLength = outBuffSize - 10;
                    memcpy(pOutBuf,&outBuff[8],*pOutLength);
                }
                else
                {
                    *pOutLength =0;
                    result =  PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_CMD_ABORTED);
                }
            }
            break;
        }
        default:
            result =  PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_INVALID_PARAMETER);
            break;
    }
    
    return result;
}
     
     
     
     
/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Close( phHal_sHwReference_t *psHwReference)
{
    uint8_t     Tama_RfConf[2];
    uint8_t     Tama_LowVBat[] = {0x02, 0x00, 0x00};
    uint8_t     outputBuffer[MAX_SIZE_TAMA_BUFFER];
    uint16_t    outputBufferSz = sizeof(outputBuffer);
    uint16_t    LEN                 =   0;
    NFCSTATUS   status;
    
    /* halt RF */
    
    Tama_RfConf[0] = 0x01;
    Tama_RfConf[1] = 0x02;

    outputBufferSz = sizeof(outputBuffer);
    status  =   phTalTama_Protocol( psHwReference,
                        RF_CONFIGURATION,
                        Tama_RfConf,
                        2,
                        outputBuffer,
                        &outputBufferSz,
                        &LEN,
                        0);
    if(NFCSTATUS_SUCCESS == status)
    {
        if(!psHwReference->Sleeping 
           && (phHal_eChipName532	==	psHwReference->ChipName)
           && (chipFirmwareVerCompare(psHwReference, ">=", 1, 5)) )
        {
            status  =   phTalTama_Protocol( psHwReference,
                                            SAM_CONFIGURATION,
                                            Tama_LowVBat,
                                            3,
                                            outputBuffer,
                                            &outputBufferSz,
                                            &LEN,
                                            0);
            if(NFCSTATUS_SUCCESS == status)
            {
                psHwReference->Sleeping = 1;
            }
        }

    }
    
    status =   phDalNfc_Close(psHwReference);

    return status;
}
     
     
     
     
/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_StartTargetMode( phHal_sHwReference_t    *psHwReference,
                                            phHal_sTargetInfo_t     *pTgInfo,
                                            phHal_eOpModes_t        OpModes[],
                                            uint8_t                 *pConnectionReq,
                                            uint8_t                 *pConnectionReqBufLength )
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t  pCommand[MAX_SIZE_DAL_BUFFER];
    uint8_t  Buffer[100];  
    uint8_t  mode, mask	=	0x73;
    uint16_t nNumberOfBytesToRead = 100;
    uint16_t    LEN                 =   0;
    uint8_t  GeneralBytesSize;
	uint8_t bWTRegValue[3]; // bWT Address and value
    uint8_t  tempBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t    tempBufferLen       =   MAX_SIZE_DAL_BUFFER;

    if((psHwReference == NULL) || (pTgInfo == NULL) || (pConnectionReqBufLength == NULL) ||
       ((pConnectionReq == NULL)&&(*pConnectionReqBufLength !=0)))
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);

    /* check if the device is opened */
    if(psHwReference->Flags.LinkValid != 1)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);

    GeneralBytesSize	=	pTgInfo->GeneralBytesLength;
 
	/* Fill mode byte: DEP only mode */
    pCommand[0] = 0x02;

    /* Fill Mifare params (6 bytes) */
    memcpy(pCommand+1, pTgInfo->SensRes, 2);
    memcpy(pCommand+3, pTgInfo->NFCID1t, 3);
    pCommand[6] = pTgInfo->SelRes;

    /* Fill Felica params (18 bytes) */
    memcpy(pCommand+7, pTgInfo->NFCID2t, 8);
    memcpy(pCommand+15, pTgInfo->PAD, 8);
    pCommand[23] = pTgInfo->SystemCode[0];
    pCommand[24] = pTgInfo->SystemCode[1];
    pCommand[7] = 0x01;
    pCommand[8] = 0xFE;

    /* Fill NFCID3t (10 bytes) */
    memcpy(pCommand+25, pTgInfo->NFCID3t, 10);

    /* Peer to Peer BugFix: The value of the Waiting Time by default is 9 
     * which is not enough for transactions that have longer bytes. So, 
     * the value is changed to account for the more time.
     */

    if(psHwReference->ChipName == phHal_eChipName531)
    {
        bWTRegValue[0] = 0x01;
        bWTRegValue[1] = 0x06;
        bWTRegValue[2] = 0x0B;
    }
    else
        if(psHwReference->ChipName == phHal_eChipName532)
        {
            bWTRegValue[0] = 0x02;
            bWTRegValue[1] = 0xF9;
            bWTRegValue[2] = 0x0B;
        }
        else
        {
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);
        }

	result =  phTalTama_Protocol(psHwReference,
								WRITE_REGISTER,
								bWTRegValue,
								3,
								tempBuffer,
								&tempBufferLen,
                                &LEN,
								0);

    if(	phHal_eChipName532	==	psHwReference->ChipName)
	{

		/* Fill General bytes (47 bytes) */
		pCommand[35]	=	GeneralBytesSize;
	    if (GeneralBytesSize != 0)
		    memcpy(pCommand+36, pTgInfo->GeneralBytes, GeneralBytesSize);

		/* Fill Historical bytes (set to 0 right now) */
		pCommand[36+GeneralBytesSize]	=	0;

		/* Send TgInitTarget to PN532 with encapsulation, read ACK and response */
		result =  phTalTama_Protocol(psHwReference,
									TG_INIT_CMD,
									pCommand,
									(uint16_t)(37 + GeneralBytesSize),
									Buffer ,
									&nNumberOfBytesToRead,
                                    &LEN,
									psHwReference->UserTimeout);
	}
	else
	{
		/* Fill General bytes (47 bytes) */
	    if (GeneralBytesSize != 0)
		    memcpy(pCommand+35, pTgInfo->GeneralBytes, GeneralBytesSize);

		/* Send TgInitTAMATarget to PN531 with encapsulation, read ACK and response */
		result =  phTalTama_Protocol(psHwReference,
									TG_INIT_CMD,
									pCommand,
									(uint16_t)(35 + GeneralBytesSize),
									Buffer ,
									&nNumberOfBytesToRead,
                                    &LEN,
									psHwReference->UserTimeout);
	}

    if (result!=NFCSTATUS_SUCCESS)
    {
        return result;
    }
     
    /* Mode activated by initiator */
	mode = (uint8_t)(Buffer[7] & mask);
    switch (mode)
    {
        case 0x00:
            OpModes[0] = phHal_eOpModesNfcPassive106;
            break;
     
        case 0x01:
            OpModes[0] = phHal_eOpModesNfcActive106;
            break;
     
        case 0x21:
            OpModes[0] = phHal_eOpModesNfcActive424;
            break;
     
        case 0x22:
            OpModes[0] = phHal_eOpModesNfcPassive424;
            break;
     
        case 0x11:
            OpModes[0] = phHal_eOpModesNfcActive212;
            break;
     
        case 0x12:
            OpModes[0] = phHal_eOpModesNfcPassive212;
            break;
     
        default: 
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE_REQUEST) ;
            break;
    }

    /* Fill ConnectionReqBuf if automatic response set */
    if (pTgInfo->Options.AutoConnectionResponse == 0)
    {
        *pConnectionReqBufLength = 0;
    }
    else if (*pConnectionReqBufLength > 0)
    {
        if (*pConnectionReqBufLength >= ((uint8_t)(nNumberOfBytesToRead - 10)))
            *pConnectionReqBufLength = (uint8_t)(nNumberOfBytesToRead - 10);
        memcpy(pConnectionReq, &(Buffer[8]), *pConnectionReqBufLength);
    }

return result;
}



/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Receive( phHal_sHwReference_t        *psHwReference,
                                    phHal_sDepAdditionalInfo_t  *psDepAdditionalInfo,
                                    uint8_t                     *pRecvBuf,
                                    uint16_t                    *pRecvLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint16_t    LEN                 =   0;
    uint8_t  pTempBuf[MAX_SIZE_DAL_BUFFER];
    uint16_t uiTempLength = sizeof(pTempBuf);

    if((psHwReference == NULL) || (psDepAdditionalInfo == NULL) || (pRecvLength == NULL) 
       || ((pRecvBuf == NULL)&&(*pRecvLength != 0)))
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);            

    if(psHwReference->Flags.LinkValid != 1)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);

        /* Data Exchange Protocol is used */
        result = phTalTama_Protocol(psHwReference,
                                    TG_GET_DEP_CMD, 
                                    NULL,
                                    0,
                                    pTempBuf,
                                    &uiTempLength,
                                    &LEN,
                                    psHwReference->UserTimeout);

        if (result!=NFCSTATUS_SUCCESS)
            return result;

        /* test of status byte (00:OK) */
        if ((uiTempLength > 7)&&((pTempBuf[7] & ERROR_MASK) != 0x00))
        {
            if ((pTempBuf[7] & ERROR_MASK) == 0x01)
            {
                result  =   NFCSTATUS_RF_TIMEOUT;
            }
            else
            {
                /*  Retrieve TgGetTargetStatus
                    Pay attention to the fact that we use
                    the same pTempBuf buffer to store another
                    output of the phTalTama_Protocol function
                */
                uiTempLength = sizeof(pTempBuf);
            
                result = phTalTama_Protocol(psHwReference,
                                            TG_GET_STATUS, 
                                            NULL,
                                            0,
                                            pTempBuf,
                                            &uiTempLength,
                                            &LEN,
                                            psHwReference->UserTimeout);

                if (NFCSTATUS_SUCCESS != result)
                    return result;

                /*  TODO: Manage bug from PN531, not notifying that
                    the target has been released */

                if(0X00  ==  pTempBuf[7])
                    result  =   NFCSTATUS_RELEASED;
                else
                    if(0X02  ==  pTempBuf[7])
                        result  =   NFCSTATUS_DESELECTED;
                    else
                        result  =   NFCSTATUS_CMD_ABORTED;
            }

            return PHNFCSTVAL(CID_NFC_TAL,result);
        }



        /* check if MI is used in the response */
        
        if((pTempBuf[7] & MI_MASK) == 0)
            psDepAdditionalInfo->DepFlags.MetaChaining = 0;
        else
            psDepAdditionalInfo->DepFlags.MetaChaining = 1;


    /* check if NAD is used in the response */
        if((pTempBuf[7] & NAD_MASK) == 0)
        {
            if(*pRecvLength < (uiTempLength - 10))
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_BUFFER_TOO_SMALL);
            psDepAdditionalInfo->DepFlags.NADPresent = 0;
            *pRecvLength = uiTempLength - 10;
            memcpy(pRecvBuf, pTempBuf+8, *pRecvLength);
        }
        else
        {
            if(*pRecvLength < (uiTempLength - 11))
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_BUFFER_TOO_SMALL);
            psDepAdditionalInfo->DepFlags.NADPresent = 1;
            psDepAdditionalInfo->NAD = *(pTempBuf+8);
            *pRecvLength = uiTempLength - 11;
            memcpy(pRecvBuf, pTempBuf+9, *pRecvLength);
        }
     return result;
}



/*! For documentation, see header file
 *  
 */
extern NFCSTATUS phTalTama_Send( phHal_sHwReference_t        *psHwReference,
                                 phHal_sDepAdditionalInfo_t  *psDepAdditionalInfo,
                                 uint8_t                     *pSendBuf,
                                 uint16_t                     SendLength)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t     aBufferOut[20];
    uint16_t    uiBufferOutSize = 20;
	uint16_t	MAXSizeBuffer=	MAX_SIZE_TAMA_BUFFER;
    uint16_t    LEN                 =   0;
	uint8_t		tamaMobile	=	(phHal_eChipName532	==	psHwReference->ChipName? 1:0);


    if((psHwReference == NULL) || (psDepAdditionalInfo == NULL) || (pSendBuf == NULL))
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);            

    /* check if the device is opened */
    if(psHwReference->Flags.LinkValid != 1)
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_DEVICE);

	if(	tamaMobile )
	{
		MAXSizeBuffer	=	MAX_SIZE_TAMAMOBILE_BUFFER;
	}

    /* Data Exchange Protocol is used */
    if(psDepAdditionalInfo->DepFlags.NADPresent)
    {
        uint8_t tab[2];
        /* check if the buffer length and NAD byte < MTU */
        if (SendLength > MAXSizeBuffer-1)
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
        
        if(psDepAdditionalInfo->DepFlags.MetaChaining)
            tab[0] = TG_SET_META_DEP_CMD;
        else
            tab[0] = TG_SET_DEP_CMD;
        tab[1] = psDepAdditionalInfo->NAD;

        result = phTalTama_Protocol2(psHwReference,
                                     tab,
                                     2,
                                     pSendBuf,
                                     (uint16_t)(SendLength+1),
                                     aBufferOut,
                                     &uiBufferOutSize,
                                     &LEN,
                                     0);
    }
    else
    {
        uint8_t cmd;
        /* check if the buffer length < MTU */
        if (SendLength > MAXSizeBuffer)
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);

        if(psDepAdditionalInfo->DepFlags.MetaChaining)
            cmd = TG_SET_META_DEP_CMD;
        else
            cmd = TG_SET_DEP_CMD;

        result = phTalTama_Protocol(psHwReference,
                                    cmd, 
                                    pSendBuf,
                                    SendLength,
                                    aBufferOut,
                                    &uiBufferOutSize,
                                    &LEN,
                                    psHwReference->UserTimeout);
    }

    if (result!=NFCSTATUS_SUCCESS)
        return result;

    if((uiBufferOutSize > 7)&&((aBufferOut[7] & ERROR_MASK) != 0x00))
        result =  PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_CMD_ABORTED); 
     
    return result;
}
     
/*
 * Send a command to a TAMA target, wait for the result and return the response of the TAMA Target
 * \param [in] psHwReference : reference of the device
 * \param[in] pBufferIn : [in] buffer containing the command to send
 * \param[in] uiBufferInSize : Size of the input buffer
 * \param[out] pBufferOut : result buffer send by the TAMA. Warning: this buffer have to be sized almost to the complete tama frame 
 * even if the result contains only informations data of this frame.
 * \param[out] puiBufferOutSize : size of the outputBuffer
 * 
 *
 */

NFCSTATUS phTalTama_Protocol(phHal_sHwReference_t   *psHwReference,
                             uint8_t    uiCmd,
                             uint8_t   *pBufferIn,
                             uint16_t   uiBufferInSize,
                             uint8_t   *pBufferOut,
                             uint16_t  *puiBufferOutSize,
                             uint16_t  *pLen,
                             uint32_t   nTimeout)
{
    /****************************************
     TAMA encapsulation
     ****************************************/
    uint8_t  pCmdBuffer[MAX_SIZE_DAL_BUFFER];
    uint8_t  pAckBuffer[6]; 
    uint8_t  Ack[]={0x00, 0x00,0xFF,0x00,0xFF,0x00};
    uint8_t  ErrorFrame[] = {0x00,0x00,0xFF,0x01,0xFF,0x7F,0x81,0x00};
    uint8_t  ExtInfFrame[] = {0x00,0x00,0xFF,0xFF,0xFF};
    uint16_t ui16ReadBufferSize;
    uint16_t ui16WriteBufferSize;
    uint16_t i;
    uint8_t  sum = 0;
    NFCSTATUS result = NFCSTATUS_SUCCESS;

    *pLen = 0;

    result = phTalTama_WakeUp(psHwReference,uiCmd,pBufferIn,uiBufferInSize,pBufferOut,puiBufferOutSize,nTimeout);
    if (result != NFCSTATUS_SUCCESS)
    {
		return result;
	}

	if(     phHal_eChipName53x   ==  psHwReference->ChipName
        ||  phHal_eChipName531   ==  psHwReference->ChipName)
    {
		/*	
         *  Normal Information Frame	(PN531)	
         */
		
        /* Preambule */
		pCmdBuffer[0] = 0x00;
		/* SYNC bytes */
		pCmdBuffer[1] = 0x00;
		pCmdBuffer[2] = 0xFF;
		/* LEN */
		pCmdBuffer[3] = (uint8_t)(uiBufferInSize + 2);
		/* LCS */
		pCmdBuffer[4] = (uint8_t)(0x00 - pCmdBuffer[3]);
		/* TFI */
		pCmdBuffer[5] = 0xD4;
		/* Command Tama */
		pCmdBuffer[6] = uiCmd;
		/* Data */
		memcpy( pCmdBuffer+7, pBufferIn, uiBufferInSize );
		/* DCS */
		for( i=5; i<uiBufferInSize+7; i++ )
			sum = sum + pCmdBuffer[i];
		pCmdBuffer[uiBufferInSize+7] = (uint8_t)(0x00 - sum);
	  
		/* Postambule */
		pCmdBuffer[uiBufferInSize+8] = 0x00;

        /****************************************
         Command send
        ****************************************/

	#ifdef PRINTF_DEBUG
		rt_kprintf("PC -> TAMA : size %d \n             ",(uiBufferInSize + 9));
		for(i=0; i < (uiBufferInSize + 9);i++)
		{
			rt_kprintf("%02X ",pCmdBuffer[i]);
		}

		rt_kprintf("\n");
	#endif
	     
		/* send command */
		result = phDalNfc_Write( psHwReference,  
								 pCmdBuffer,  
								 (uint16_t)(uiBufferInSize + 9),  
								 &ui16WriteBufferSize );
		     
		if (result != NFCSTATUS_SUCCESS)
		{
	#ifdef PRINTF_DEBUG  
			rt_kprintf("<Status ERROR %d>\n",result);
	#endif
			return result;
		}
	}
    else
    {
		/*	
         *  Extended Information Frame	(PN532 only)	
         */

		/* Preambule */
		pCmdBuffer[0] = 0x00;
		/* SYNC bytes */
		pCmdBuffer[1] = 0x00;
		pCmdBuffer[2] = 0xFF;
		/* Extended Info */
		pCmdBuffer[3] = 0xFF;
		pCmdBuffer[4] = 0xFF;
		/* LEN */
		pCmdBuffer[5] = (uint8_t)((uiBufferInSize + 2)	>> 8);
		pCmdBuffer[6] = (uint8_t)((uiBufferInSize + 2)	% 256);
		/* LCS */
		pCmdBuffer[7] = (uint8_t)(0x00 - pCmdBuffer[5] - pCmdBuffer[6]);
		/* TFI */
		pCmdBuffer[8] = 0xD4;
		/* Command Tama */
		pCmdBuffer[9] = uiCmd;
		/* Data */
		memcpy( pCmdBuffer+10, pBufferIn, uiBufferInSize );
		/* DCS */
		for( i=8; i<uiBufferInSize+10; i++ )
			sum = sum + pCmdBuffer[i];
		pCmdBuffer[uiBufferInSize+10] = (uint8_t)(0x00 - sum);
	  
		/* Postambule */
		pCmdBuffer[uiBufferInSize+11] = 0x00;

        /****************************************
         Command send
        ****************************************/

	#ifdef PRINTF_DEBUG
		rt_kprintf("PC -> TAMA : size %d \n             ",(uiBufferInSize + 12));
		for(i=0; i < (uiBufferInSize + 12);i++)
		{
			rt_kprintf("%02X ",pCmdBuffer[i]);
		}

		rt_kprintf("\n");
	#endif
	     
		/* send command */
		result = phDalNfc_Write( psHwReference,  
								 pCmdBuffer,  
								 (uint16_t)(uiBufferInSize + 12),  
								 &ui16WriteBufferSize);
	     
		if (result != NFCSTATUS_SUCCESS)
		{
	#ifdef PRINTF_DEBUG  
			rt_kprintf("<Status ERROR %d>\n",result);
	#endif
			return result;
		}
	}
	
	
    /* read ACK  */
    result = phDalNfc_Read(psHwReference,  
                           pAckBuffer, 
                           6,  
                           &ui16ReadBufferSize,
                           ACK_TIMEOUT);
#ifdef PRINTF_DEBUG
    rt_kprintf("TAMA -> PC : ACK \n             ");
    for(i=0; i < 6 ; i++)
    {
        rt_kprintf("%02X ",pAckBuffer[i]);
    }
    rt_kprintf("\n");
#endif
     
    if (result != NFCSTATUS_SUCCESS)
    {
#ifdef PRINTF_DEBUG
        rt_kprintf("<Status ERROR %d>\n",result);
#endif
        return result;
    }

    /* verify if we have an ACK */
    if (memcmp(pAckBuffer,Ack,6)!=0)
    {
#ifdef PRINTF_DEBUG
        rt_kprintf("return error code\n");
#endif
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
    }
    /* read response  */
    result = phDalNfc_Read(psHwReference,  
                           pBufferOut,
                           *puiBufferOutSize,  
                           &ui16ReadBufferSize,
                           nTimeout);

#ifdef PRINTF_DEBUG
    rt_kprintf("TAMA -> PC : size %d \n             ",ui16ReadBufferSize);
    for(i=0; i < ui16ReadBufferSize;i++)
    {
        rt_kprintf("%02X ",pBufferOut[i]);
    }
    rt_kprintf("\n");
#endif

    if (result != NFCSTATUS_SUCCESS)
    {
        NFCSTATUS result2 = NFCSTATUS_SUCCESS;
#ifdef PRINTF_DEBUG
        rt_kprintf("PC -> TAMA : ABORT \n             ");
        for(i=0; i < 6;i++)
        {
            rt_kprintf("%02X ",Ack[i]);
        }
        rt_kprintf("\n");
#endif
        /* send command */
        result2 = phDalNfc_Write(psHwReference,  
                                Ack,
                                6,  
                                &ui16ReadBufferSize);
        return result;
    }

    /* verify if we have an error frame */
    if (memcmp(pBufferOut,ErrorFrame,8)==0)
    {
#ifdef PRINTF_DEBUG
        rt_kprintf("return error code\n");
#endif
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
    }
    
 
     /* Verify we have a normal or extended information Frame */
    if (memcmp(pBufferOut,ExtInfFrame,sizeof(ExtInfFrame))!=0)
    {
        /* Normal Information Frame */


        if ((uint8_t)(pBufferOut[3]+pBufferOut[4]) != 0)
        {   
    #ifdef PRINTF_DEBUG
            rt_kprintf("checksum error\n");
    #endif
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);    
        }
        sum = 0;
        for( i=5; i<ui16ReadBufferSize; i++ )
        {
            sum += pBufferOut[i];
        }
        if ((uint8_t)sum != 0)
        {
    #ifdef PRINTF_DEBUG
            rt_kprintf("checksum error\n");
    #endif
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);    
        }

        *pLen   =    pBufferOut[3];

    }
    else
    {
        /* Extended Information Frame */


        if ((uint8_t)(pBufferOut[5]+pBufferOut[6]+pBufferOut[7]) != 0)
        {   
    #ifdef PRINTF_DEBUG
            rt_kprintf("checksum error\n");
    #endif
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);    
        }
        sum = 0;
        for( i=8; i<ui16ReadBufferSize; i++ )
        {
            sum += pBufferOut[i];
        }
        if ((uint8_t)sum != 0)
        {
    #ifdef PRINTF_DEBUG
            rt_kprintf("checksum error\n");
    #endif
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);    
        }
        *pLen   =    (((uint16_t)pBufferOut[5])<<8) +((uint16_t)pBufferOut[6]);

        /* Fake we have a normal information frame, for the caller */
        for(i=3; i<3+*pLen; i++)
        {
            pBufferOut[i] = pBufferOut[i+3];
        }
        ui16ReadBufferSize  -=3;
    }

   /* set the size of the last output buffer */
    *puiBufferOutSize = ui16ReadBufferSize;
      

     if (*puiBufferOutSize < 7)
    {
        /* From the protocol, this is impossible that TAMA sends less than 7 bytes... */
//        assert(0);
        return PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_BOARD_COMMUNICATION_ERROR);
    }
    return result;
}



NFCSTATUS phTalTama_Protocol2(phHal_sHwReference_t  *psHwReference,
                              uint8_t   *pParam,
                              uint8_t   pParamSize,
                              uint8_t   *pDataIn,
                              uint16_t  pDataInSize,
                              uint8_t   *pDataOut,
                              uint16_t  *pDataOutSize,
                              uint16_t  *pLen,
                              uint32_t   nTimeout)
{
    /****************************************
        TAMA encapsulation
    ****************************************/

    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t  pCmdBuffer[MAX_SIZE_DAL_BUFFER];
    uint8_t  pAckBuffer[6]; 
    uint8_t  Ack[]={0x00, 0x00,0xFF,0x00,0xFF,0x00};
    uint8_t ErrorFrame[] = {0x00,0x00,0xFF,0x01,0xFF,0x7F,0x81,0x00};
    uint8_t ExtInfFrame[] = {0x00,0x00,0xFF,0xFF,0xFF};
    uint16_t ui16CommandSize;   
    uint16_t ui16ReadBufferSize;
    uint16_t ui16WriteBufferSize;
    uint16_t i;
    uint8_t  sum = 0;

    *pLen = 0;

    result = phTalTama_WakeUp(psHwReference,pParam[0],pDataIn,pDataInSize,pDataOut,pDataOutSize,nTimeout);
    if (result != NFCSTATUS_SUCCESS||
        ( (pParam[0] == 0x14) && (pDataIn[0] == 0x01)
          && (phHal_eChipName532 ==	psHwReference->ChipName)
          && (chipFirmwareVerCompare(psHwReference, ">=", 1, 5))))
	{
		return result;
	}

	if(phHal_eChipName531   ==  psHwReference->ChipName)
    {
		/*	
         *  Normal Information Frame	(PN531)	
         */
	
		ui16CommandSize = (uint16_t)(pDataInSize + pParamSize + 8); 

		/* Preambule */
		pCmdBuffer[0] = 0x00;
		/* SYNC bytes */
		pCmdBuffer[1] = 0x00;
		pCmdBuffer[2] = 0xFF;
		/* LEN */
		pCmdBuffer[3] = (uint8_t)(ui16CommandSize - 7);
		/* LCS */
		pCmdBuffer[4] = (uint8_t)(0x00 - pCmdBuffer[3]);
		/* TFI */
		pCmdBuffer[5] = 0xD4;

		/* Command Tama and other parameters */
		memcpy(pCmdBuffer+6,pParam, pParamSize);
		/* Data */
		memcpy(pCmdBuffer+6+pParamSize, pDataIn, pDataInSize );
		/* DCS */
		for( i=5; i<(ui16CommandSize - 2); i++ )
			sum = sum + pCmdBuffer[i];
		pCmdBuffer[ui16CommandSize - 2] = (uint8_t)(0x00 - sum);
	  
		/* Postambule */
		pCmdBuffer[ui16CommandSize - 1] = 0x00;

        /****************************************
        Command send
        ****************************************/
	
    #ifdef PRINTF_DEBUG
        rt_kprintf("PC -> TAMA : size %d \n             ",ui16CommandSize);
		for(i=0; i < ui16CommandSize;i++)
		{
			rt_kprintf("%02X ",pCmdBuffer[i]);
		}

		rt_kprintf("\n");
	#endif

		/* send command */
		result = phDalNfc_Write( psHwReference,  
								 pCmdBuffer,  
								 ui16CommandSize,  
								 &ui16WriteBufferSize);

		if (result != NFCSTATUS_SUCCESS)
		{
#ifdef PRINTF_DEBUG
            rt_kprintf("<Status ERROR %d>\n",result);
#endif
            return result;
		}
	}
    else
	{
		/*	
         *  Extended Information Frame	(PN532 only)	
         */

//        assert(phHal_eChipName532   ==  psHwReference->ChipName);
		ui16CommandSize = (uint16_t)(pDataInSize + pParamSize + 11); 

		/* Preambule */
		pCmdBuffer[0] = 0x00;
		/* SYNC bytes */
		pCmdBuffer[1] = 0x00;
		pCmdBuffer[2] = 0xFF;
		/* Extended Info */
		pCmdBuffer[3] = 0xFF;
		pCmdBuffer[4] = 0xFF;
		/* LEN */
		pCmdBuffer[5] = (uint8_t)((ui16CommandSize - 10)	>> 8);
		pCmdBuffer[6] = (uint8_t)((ui16CommandSize - 10)	% 256 );
		/* LCS */
		pCmdBuffer[7] = (uint8_t)(0x00 - pCmdBuffer[5] - pCmdBuffer[6]);
		/* TFI */
		pCmdBuffer[8] = 0xD4;

		/* Command Tama and other parameters */
		memcpy(pCmdBuffer+9,pParam, pParamSize);
		/* Data */
		memcpy(pCmdBuffer+9+pParamSize, pDataIn, pDataInSize );
		/* DCS */
		for( i=8; i<(ui16CommandSize - 2); i++ )
			sum = sum + pCmdBuffer[i];
		pCmdBuffer[ui16CommandSize - 2] = (uint8_t)(0x00 - sum);
	  
		/* Postambule */
		pCmdBuffer[ui16CommandSize - 1] = 0x00;

        /****************************************
        Command send
        ****************************************/

    #ifdef PRINTF_DEBUG
		rt_kprintf("PC -> TAMA : size %d \n             ",ui16CommandSize);
		for(i=0; i < ui16CommandSize;i++)
		{
			rt_kprintf("%02X ",pCmdBuffer[i]);
		}

		rt_kprintf("\n");
	#endif

		/* send command */
		result = phDalNfc_Write( psHwReference,  
								 pCmdBuffer,  
								 ui16CommandSize,  
								 &ui16WriteBufferSize);

		if (result != NFCSTATUS_SUCCESS)
		{
#ifdef PRINTF_DEBUG
            rt_kprintf("<Status ERROR %d>\n",result);
#endif
            return result;
		}
	}

    /* read ACK  */
    result = phDalNfc_Read(psHwReference,  
                           pAckBuffer, 
                           6,  
                           &ui16ReadBufferSize, 
                           ACK_TIMEOUT );

#ifdef PRINTF_DEBUG
    rt_kprintf("TAMA -> PC : ACK \n             ");
    for(i=0; i < 6 ; i++)
    {
        rt_kprintf("%02X ",pAckBuffer[i]);
    }
    rt_kprintf("\n");
#endif

    if (result != NFCSTATUS_SUCCESS)
    {
#ifdef PRINTF_DEBUG
            rt_kprintf("<Status ERROR %d>\n",result);
#endif       
        return result;
    }
  
    /* verify if we have an ACK */
    if (memcmp(pAckBuffer,Ack,6)!=0)
    {
        return  PHNFCSTVAL(CID_NFC_TAL, NFCSTATUS_CMD_ABORTED);
    }

    /* read response  */
    result = phDalNfc_Read(psHwReference,  
                           pDataOut,
                           *pDataOutSize,  
                           &ui16ReadBufferSize, 
                           nTimeout);

    #ifdef PRINTF_DEBUG
    rt_kprintf("TAMA -> PC : size %d \n             ",ui16ReadBufferSize);
    for(i=0; i < ui16ReadBufferSize;i++)
    {
        rt_kprintf("%02X ",pDataOut[i]);
    }
    rt_kprintf("\n");
    #endif

    if (result != NFCSTATUS_SUCCESS)
    {
        NFCSTATUS result2 = NFCSTATUS_SUCCESS;        
        #ifdef PRINTF_DEBUG
        rt_kprintf("<Status ERROR %d>\n",result);
        rt_kprintf("PC -> TAMA : ABORT \n             ");
        for(i=0; i < 6;i++)
        {
            rt_kprintf("%02X ",Ack[i]);
        }
        rt_kprintf("\n");
        #endif
        /* send command */
        result2 = phDalNfc_Write( psHwReference,  
                                  Ack,
                                  6,  
                                  &ui16ReadBufferSize);
        return result;
    }

    /* verify if we have an error frame */
    if (memcmp(pDataOut,ErrorFrame,8)==0)
    {
#ifdef PRINTF_DEBUG
        rt_kprintf("return error code\n");
#endif
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
    }

   /* Verify we have a normal or extended information Frame */
    if (memcmp(pDataOut,ExtInfFrame,sizeof(ExtInfFrame))!=0)
    {
        /* Normal Information Frame */


        if ((uint8_t)(pDataOut[3]+pDataOut[4]) != 0)
        {   
    #ifdef PRINTF_DEBUG
            rt_kprintf("checksum error\n");
    #endif
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);    
        }
        sum = 0;
        for( i=5; i<ui16ReadBufferSize; i++ )
        {
            sum += pDataOut[i];
        }
        if ((uint8_t)sum != 0)
        {
    #ifdef PRINTF_DEBUG
            rt_kprintf("checksum error\n");
    #endif
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);    
        }

        *pLen   =    pDataOut[3];

    }
    else
    {
        /* Extended Information Frame */


        if ((uint8_t)(pDataOut[5]+pDataOut[6]+pDataOut[7]) != 0)
        {   
    #ifdef PRINTF_DEBUG
            rt_kprintf("checksum error\n");
    #endif
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);    
        }
        sum = 0;
        for( i=8; i<ui16ReadBufferSize; i++ )
        {
            sum += pDataOut[i];
        }
        if ((uint8_t)sum != 0)
        {
    #ifdef PRINTF_DEBUG
            rt_kprintf("checksum error\n");
    #endif
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);    
        }
        *pLen   =    (((uint16_t)pDataOut[5])<<8) +((uint16_t)pDataOut[6]);

        /* Fake we have a normal information frame, for the caller */
        for(i=3; i<3+*pLen; i++)
        {
            pDataOut[i] = pDataOut[i+3];
        }
        ui16ReadBufferSize  -=3;
    }

    /* set the size of the last output buffer */
    *pDataOutSize = ui16ReadBufferSize;

    if (*pDataOutSize < 7)
    {
        /* From the protocol, this is impossible that TAMA sends less than 7 bytes... */
        //assert(0);
        return PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_BOARD_COMMUNICATION_ERROR);
    }


    return result;
}


NFCSTATUS phTalTama_WakeUp(phHal_sHwReference_t   *psHwReference,
                             uint8_t    uiCmd,
                             uint8_t   *pBufferIn,
                             uint16_t   uiBufferInSize,
                             uint8_t   *pBufferOut,
                             uint16_t  *puiBufferOutSize,
                             uint32_t   nTimeout)
{
    /****************************************
     TAMA encapsulation
     ****************************************/
    uint8_t  Ack[]={0x00, 0x00,0xFF,0x00,0xFF,0x00};
    uint8_t  ErrorFrame[] = {0x00,0x00,0xFF,0x01,0xFF,0x7F,0x81,0x00};
    uint8_t  FrameWakeup[]= {0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00};
    uint16_t ui16WriteBufferSize;
	#ifdef PRINTF_DEBUG
    uint16_t i;
	#endif
    
    NFCSTATUS result = NFCSTATUS_SUCCESS;

    // if PN532 v1.5 is in Virtual mode, wake up it

	#ifdef PRINTF_DEBUG
		rt_kprintf("PC -> TAMA : size %d \n             ",sizeof(FrameWakeup));
		for(i=0; i < sizeof(FrameWakeup);i++)
		{
			rt_kprintf("%02X ",FrameWakeup[i]);
		}

		rt_kprintf("\n");
	#endif
        /* send command */
		result = phDalNfc_Write( psHwReference,  
								 FrameWakeup,  
								 sizeof(FrameWakeup),  
								 &ui16WriteBufferSize);

        if(result != NFCSTATUS_SUCCESS)
        {
        #ifdef PRINTF_DEBUG
            rt_kprintf("<Status ERROR %d>\n",result);
        #endif            
            return result;
        }

    psHwReference->Sleeping = 0;

    return result;
}

/**
 * Data structure filling
 * \param[out] startupToFill : data structure to fill. Have to be allocated
 * \param[in] databuffer : pointer on data returned by the TAMA (TargetData) to add in the startup structure
 */
     
void fillStartUp212_424(phHal_sPassive212_424Startup_t *startupToFill, uint8_t *dataBuffer)
{
    startupToFill->RespCode =  dataBuffer[2];
    memcpy(startupToFill->NFCID2t , & dataBuffer[3], 8);
    memcpy(startupToFill->PMm, & dataBuffer[11], 8);
     
    if(dataBuffer[1] == 20)
    {
        startupToFill->SystemCodeAvailable = 1;
        startupToFill->SystemCode[0] = dataBuffer[19];
        startupToFill->SystemCode[1] = dataBuffer[20];
    }
    else
    {
        startupToFill->SystemCodeAvailable = 0;
    }
}
     
/**
 * Data structure filling
 * \param[out] startupToFill : data structure to fill. Have to be allocated
 * \param[in] databuffer : pointer on data returned by the TAMA (TargetData) to add in the startup structure
 */
     
void fillStartUp106(phHal_sHwReference_t *psHwReference, phHal_sPassive106Startup_t *startupToFill, uint8_t *dataBuffer)
{
    startupToFill->SelRes = dataBuffer[3];

    if(phHal_eChipName531   ==  psHwReference->ChipName)
    {
        startupToFill->SensRes[0] = dataBuffer[1];
        startupToFill->SensRes[1] = dataBuffer[2];
        startupToFill->NFCID1tLength = dataBuffer[4];

        /*  In case of PN531, remove CT */
        if(startupToFill->NFCID1tLength !=  4)
        {
            startupToFill->NFCID1tLength = dataBuffer[4]    -   1;
            memcpy(startupToFill->NFCID1t,&dataBuffer[6],startupToFill->NFCID1tLength);
        }
        else
        {
            startupToFill->NFCID1tLength = dataBuffer[4];
            memcpy(startupToFill->NFCID1t,&dataBuffer[5],startupToFill->NFCID1tLength);
        }

    }
    else
    if(phHal_eChipName532   ==  psHwReference->ChipName)
    {
        startupToFill->SensRes[0] = dataBuffer[2];
        startupToFill->SensRes[1] = dataBuffer[1];
        startupToFill->NFCID1tLength = dataBuffer[4];
        memcpy(startupToFill->NFCID1t,&dataBuffer[5],startupToFill->NFCID1tLength);
    }
    else
    {
        //assert(0);
    }

}

/**
 * Data structure filling
 * \param[out] startupToFill : data structure to fill. Have to be allocated
 * \param[in] databuffer : pointer on data returned by the TAMA (TargetData) to add in the startup structure
 */

void fillStartUpJewel(phHal_sJewelStartup_t *startupToFill, uint8_t *dataBuffer)
{
    memcpy (startupToFill->SensRes, &dataBuffer[1], 2);
    memcpy (startupToFill->JewelId, &dataBuffer[3], 4);
}


void fillIso14443A(phHal_sISO_14443_4A_Protocol_t* isoCardInfo,
                   uint8_t *ATSBuffer)
{
    uint8_t ATSSize = ATSBuffer[0];
    uint8_t nbByteBeforeHistoByte = 0;
    
    // Byte TA
    if((ATSBuffer[1] & 0x10)!=0)
        nbByteBeforeHistoByte++;
    // Byte TB
    if((ATSBuffer[1] & 0x20)!=0)
        nbByteBeforeHistoByte++;
    // Byte TC
    if((ATSBuffer[1] & 0x40)!=0)
    {
        if ((ATSBuffer[2+nbByteBeforeHistoByte] & 0x01) !=0)
        {
            isoCardInfo->NADUsed = 1;
        }
        else
        {
            isoCardInfo->NADUsed = 0;
        }

        if ((ATSBuffer[2+nbByteBeforeHistoByte] & 0x02) !=0)
        {
            isoCardInfo->CIDUsed = 1;
        }
        else
        {
             isoCardInfo->CIDUsed = 0;
        }

        nbByteBeforeHistoByte++;
    }
    else
    {
            isoCardInfo->NADUsed = 0;
            isoCardInfo->CIDUsed = 0;
    }

    isoCardInfo->HistoBytesLength = ATSSize - nbByteBeforeHistoByte - 2;
    memcpy(isoCardInfo->HistoBytes,
           & ATSBuffer[2+nbByteBeforeHistoByte],
           isoCardInfo->HistoBytesLength);
}
     
    
void fillIso14443B (phHal_sISO_14443_4B_Protocol_t *protocolToFill, uint8_t *dataBuffer)
{
    memcpy(protocolToFill->ATQB, &dataBuffer[1], 12);
    protocolToFill->AttribLength = dataBuffer[13];
    memcpy(protocolToFill->Attrib, &dataBuffer[14], protocolToFill->AttribLength);
    protocolToFill->NADUsed =   protocolToFill->ATQB[11] & 0x02 >> 1;
    protocolToFill->CIDUsed =   protocolToFill->ATQB[11] & 0x01;            
}

/**
 * Data structure filling
 * \param[out] NfcProtocolToFill : data structure to fill. Have to be allocated
 * \param[in] databuffer : pointer on data returned by the TAMA (ATR_RES) to add in the structure
 * \param[in] dataBufferSize : size of the previous buffer
 */
     
void fillNfcProtocol(phHal_NfcProtocol_t *NfcProtocolToFill, uint8_t *dataBuffer, uint8_t dataBufferSize)
{
    memcpy(NfcProtocolToFill->NFCID3t,dataBuffer, 10);
    NfcProtocolToFill->DIDtUsed = dataBuffer[10];
      
    // PPT Traitment
    if( (dataBuffer[14]& 0x01) != 0)
        NfcProtocolToFill->NADtSup = 1;
    else
        NfcProtocolToFill->NADtSup = 0;
     
    if( (dataBuffer[14]& 0x02) != 0 )
    {
        NfcProtocolToFill->TgGeneralByteLength = (uint8_t)(dataBufferSize -  15);
        memcpy(NfcProtocolToFill->TgGeneralByte,&dataBuffer[15],NfcProtocolToFill->TgGeneralByteLength); /* DBO PR 704 */
    }
    else
    {
        NfcProtocolToFill->TgGeneralByteLength = 0;
    }
     
}
 
/**
 * specific fonction for polling in active mode
 * \param[in] param baudrate : the active baudrate to poll
 * \param[in,out] nbTargetFound : nb of target allready found
 */
    
     
NFCSTATUS pollActive(phHal_sHwReference_t           *psHwReference,
                     phHal_eOpModes_t                baudrate,
                     phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                     uint8_t                        *nbTargetFound,
                     phHal_sDevInputParam_t         *puDevInputParam)
{
    
    uint8_t Tama_inJumpForDEPSize;
    uint8_t Tama_inJumpForDEP[66];
    uint8_t outputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t OutputBufferSize = sizeof(outputBuffer);
    uint8_t next,index;
    uint8_t Tama_SetTamaParam[] = {0x14};
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint16_t    LEN                 =   0;
    phHal_sRemoteDevInformation_t * currentRemoteDevice = & psRemoteDevInfoList[*nbTargetFound];
    Tama_inJumpForDEPSize = 3;

    index = 3;
       
    switch (baudrate)
    {
        case phHal_eOpModesNfcActive106: // 106
            next = 0x00;
            Tama_inJumpForDEP[1] = 0x00;
            break;
        case phHal_eOpModesNfcActive212: // 212
            Tama_inJumpForDEPSize += 5;
            next = 0x01;
            // add payload
            Tama_inJumpForDEP[3] = puDevInputParam->NfcPollPayload[0];
            Tama_inJumpForDEP[4] = puDevInputParam->NfcPollPayload[1];
            Tama_inJumpForDEP[5] = puDevInputParam->NfcPollPayload[2];
            Tama_inJumpForDEP[6] = puDevInputParam->NfcPollPayload[3];
            Tama_inJumpForDEP[7] = puDevInputParam->NfcPollPayload[4];
            index += 5;
            Tama_inJumpForDEP[1] = 0x01;
            break;
        case phHal_eOpModesNfcActive424: // 424
            Tama_inJumpForDEPSize += 5;
            next = 0x01;
            // add payload
            Tama_inJumpForDEP[3] = puDevInputParam->NfcPollPayload[0];
            Tama_inJumpForDEP[4] = puDevInputParam->NfcPollPayload[1];
            Tama_inJumpForDEP[5] = puDevInputParam->NfcPollPayload[2];
            Tama_inJumpForDEP[6] = puDevInputParam->NfcPollPayload[3];
            Tama_inJumpForDEP[7] = puDevInputParam->NfcPollPayload[4];
            index += 5;
            Tama_inJumpForDEP[1] = 0x02;
            break;
        default:
            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
            break;
    }
       
    SetTamaParameter_Initialize(baudrate, &Tama_SetTamaParam[0], puDevInputParam->DIDiUsed,puDevInputParam->NfcNADiUsed);

    OutputBufferSize = sizeof(outputBuffer);
    result = phTalTama_Protocol(psHwReference,
                                SET_TAMA_PARAMETERS,
                                Tama_SetTamaParam,
                                1,
                                outputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                0);
       
    if(result != NFCSTATUS_SUCCESS)
    {
        return result;
    }

    //------------------------------------------------------------

    if(puDevInputParam->NFCIDAuto == 0)
    {
        Tama_inJumpForDEPSize += 10;
        next = (uint8_t) (next | 0x02);
        // add nfcid3 to the command
        memcpy(& Tama_inJumpForDEP[index],puDevInputParam->NFCID3i,10);
        index += 10;
    }
       
    if(puDevInputParam->GeneralByteLength > 0)
    {
        Tama_inJumpForDEPSize = (uint8_t)(Tama_inJumpForDEPSize 
                                          + puDevInputParam->GeneralByteLength);
        next = (uint8_t) (next | 0x04);
        // add general bytes to the command
        memcpy(& Tama_inJumpForDEP[index],
               puDevInputParam->GeneralByte,
               puDevInputParam->GeneralByteLength);
    }
       
    // other bytes
    Tama_inJumpForDEP[0] = 0x01;
    Tama_inJumpForDEP[2] = next;
     
    OutputBufferSize = sizeof(outputBuffer);
    // send injumpfordep
    result = phTalTama_Protocol(psHwReference,
                                IN_JUMP_FOR_DEP,
                                Tama_inJumpForDEP, 
                                Tama_inJumpForDEPSize, 
                                outputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                DAL_TIME_OUT);
       
    if (result != NFCSTATUS_SUCCESS )
    {
        return result;
    }
         
    switch (outputBuffer[7])
    {
        case 0x00:
            // one target found
            currentRemoteDevice->hLogHandle =  outputBuffer[8];
            currentRemoteDevice->SessionOpened = 0; 
            currentRemoteDevice->UsedDID    =   puDevInputParam->DIDiUsed;
            currentRemoteDevice->UsedNAD    =   puDevInputParam->NfcNADiUsed;

            switch (baudrate)
            {
                case phHal_eOpModesNfcActive106:
                    currentRemoteDevice->OpMode = phHal_eOpModesNfcActive106;
                    break;
                case phHal_eOpModesNfcActive212:
                    currentRemoteDevice->OpMode = phHal_eOpModesNfcActive212;
                    break;
                case phHal_eOpModesNfcActive424:
                    currentRemoteDevice->OpMode = phHal_eOpModesNfcActive424;
                    break;
                default:
                    return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
                    break;
            }
            currentRemoteDevice->RemoteDevInfo.NfcActiveInfo.CurrentState = phHal_eStateListPolledState ;
           
            fillNfcProtocol(& currentRemoteDevice->RemoteDevInfo.NfcActiveInfo.NfcProtocol,
                            & outputBuffer[9],
                            (uint8_t) (OutputBufferSize - 11)); // remove what's not ATR
            (*nbTargetFound)++;
            break;
           
        case 0x01:
            // no target found
            // continue
            break;
        default:
            // error
            result  =   PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
            break;
    }

    if( (*nbTargetFound)==0x00)
    {
        // We found no target: switch the field off
        
        // halt RF
        uint8_t Tama_RfConf[2];
    
        Tama_RfConf[0] = 0x01;
        Tama_RfConf[1] = 0x02;
    

        OutputBufferSize = sizeof(outputBuffer);
        phTalTama_Protocol( psHwReference,
                            RF_CONFIGURATION,
                            Tama_RfConf,
                            2,
                            outputBuffer,
                            &OutputBufferSize,
                            &LEN,
                            0);


        result  =   PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
    }
    
    return result;
}
     
/**
 * specific fonction for polling in passive 212 424 mode
 * \param[in] baudrate : the baudrate to poll
 * \param[in,out] nbTargetFound : nb of target allready found
 * \param[in] pNbrOfRemoteDev : maximum number of target to find
 */
    
     
NFCSTATUS pollPassive212_424( phHal_sHwReference_t           *psHwReference,
                              phHal_eOpModes_t                pollType,
                              phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                              uint8_t                        *pNbrOfRemoteDev,
                              uint8_t                        *nbTargetFound,
                              phHal_sDevInputParam_t         *puDevInputParam)
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    // initialisation
    uint8_t Tama_inListPassiveTarget[] = {0x01, 0x01, 0x00, 0xFF, 0xFF, 0x00, 0x01};
    uint16_t    LEN                 =   0;
 
    uint8_t OutputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t OutputBufferSize = sizeof(OutputBuffer);
 
    uint8_t ATROutputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t ATROutputBufferSize = sizeof(ATROutputBuffer);
    uint16_t Tama_InATRSize,i,index = 0;
       
    uint8_t nbPossibleTarget,nextTargetIndex;
     
    uint8_t next;
    uint8_t Tama_inATR[60];
    uint8_t Tama_SetTamaParam[] = {0x04};
    uint8_t Tama_RfConf[2];

    Tama_inListPassiveTarget[0] =(uint8_t) (*pNbrOfRemoteDev - *nbTargetFound);

    switch(pollType)
    {
	case phHal_eOpModesFelica212:
    case phHal_eOpModesFelica424:
        Tama_inListPassiveTarget[2] = puDevInputParam->FelicaPollPayload[0];
        Tama_inListPassiveTarget[3] = puDevInputParam->FelicaPollPayload[1];
        Tama_inListPassiveTarget[4] = puDevInputParam->FelicaPollPayload[2];
        Tama_inListPassiveTarget[5] = puDevInputParam->FelicaPollPayload[3];
        Tama_inListPassiveTarget[6] = puDevInputParam->FelicaPollPayload[4];
		break;
    case phHal_eOpModesNfcPassive212 :
    case phHal_eOpModesNfcPassive424 :
        Tama_inListPassiveTarget[2] = puDevInputParam->NfcPollPayload[0];
        Tama_inListPassiveTarget[3] = puDevInputParam->NfcPollPayload[1];
        Tama_inListPassiveTarget[4] = puDevInputParam->NfcPollPayload[2];
        Tama_inListPassiveTarget[5] = puDevInputParam->NfcPollPayload[3];
        Tama_inListPassiveTarget[6] = puDevInputParam->NfcPollPayload[4];
		break;
	default	:
        // Bad pollType
		break;
    }

    //############################################################
    //############################################################
    //##  Tama_inAtr command creation
    //############################################################
    //############################################################
     
    //------------------------------------------------------------
    // change TAMA configuration according to the Input Parameters
    if(puDevInputParam != NULL)
    {
        SetTamaParameter_Initialize(pollType, &Tama_SetTamaParam[0], puDevInputParam->DIDiUsed,puDevInputParam->NfcNADiUsed);
        
        OutputBufferSize = sizeof(OutputBuffer);
        result = phTalTama_Protocol(psHwReference,
                                    SET_TAMA_PARAMETERS,
                                    Tama_SetTamaParam,
                                    1,
                                    OutputBuffer,
                                    &OutputBufferSize,
                                    &LEN,
                                    0);
        
        if(result != NFCSTATUS_SUCCESS)
        {
            return result;
        }
    }
    //------------------------------------------------------------

    Tama_InATRSize = 2;
    next = 0x00;

    if (puDevInputParam != NULL)
    {     
        // verify nfcIdAuto
        if(puDevInputParam->NFCIDAuto == 0)
        {
            Tama_InATRSize += 10;
            next = 0x01;
        }

        // verify GeneralBytes
        if(puDevInputParam->GeneralByteLength > 0)
        {
            Tama_InATRSize =  (uint16_t) (Tama_InATRSize 
                                          + puDevInputParam->GeneralByteLength);
            next = (uint8_t) (next | 0x02);
        }
    }
     
    Tama_inATR[1] = next;
    index = 2;

    // add nfcid3
    if((next & 0x01) != 0)
    {
        memcpy(& Tama_inATR[index], puDevInputParam->NFCID3i,10);
        index += 10;
    }

     
    /* TODO : put back nfcid1 from InListPassiveTarget */

       
    // add general bytes
    if((next & 0x02) != 0)
    {
        memcpy(& Tama_inATR[index],
               puDevInputParam->GeneralByte,
               puDevInputParam->GeneralByteLength);
    }
     
     
    ////////////////////////////////////////////////////////////
    // change inlistPAssiveTarget baudrate 
    ////////////////////////////////////////////////////////////
     
    switch(pollType)
    {
        case phHal_eOpModesNfcPassive212:
        case phHal_eOpModesFelica212:
            //212
            Tama_inListPassiveTarget[1] = 0x01;
            break;
        case phHal_eOpModesFelica424:
        case phHal_eOpModesNfcPassive424:
            //424
            Tama_inListPassiveTarget[1] = 0x02;
            break;
    }
     
    ////////////////////////////////////////////////////////////
    // send inListPassiveTarget
    ////////////////////////////////////////////////////////////
    OutputBufferSize = sizeof(OutputBuffer);
    result = phTalTama_Protocol(psHwReference,
                                IN_LIST_PASSIVE_TARGET_CMD,
                                Tama_inListPassiveTarget,
                                7,
                                OutputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                DAL_TIME_OUT);
 
    if((result != NFCSTATUS_SUCCESS))
    {
        return result;
    }
     
    ////////////////////////////////////////////////////////////
    // Inlistpassivetarget process
    ////////////////////////////////////////////////////////////
       
    //  Nb Target found with the Inlistpassivetarget
    nbPossibleTarget = OutputBuffer[7];
       
    nextTargetIndex = 8;
       
    for(i=0;i<nbPossibleTarget;i++)
    {
        if(     phHal_eOpModesNfcPassive212 == pollType 
            ||  phHal_eOpModesNfcPassive424 == pollType )
        {
            // Target logical number
            Tama_inATR[0] = OutputBuffer[nextTargetIndex];
               
               
            // send inATR
            ATROutputBufferSize = sizeof(ATROutputBuffer);
            result = phTalTama_Protocol(psHwReference,
                                        IN_ATR,
                                        Tama_inATR,
                                        Tama_InATRSize,
                                        ATROutputBuffer,
                                        &ATROutputBufferSize,
                                        &LEN,
                                        DAL_TIME_OUT);
               
            if((result != NFCSTATUS_SUCCESS) 
            && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
            {
                return result;
            }
        }

        if(result == NFCSTATUS_SUCCESS) 
        {
            psRemoteDevInfoList[*nbTargetFound].UsedDID    =   puDevInputParam->DIDiUsed;
            psRemoteDevInfoList[*nbTargetFound].UsedNAD    =   puDevInputParam->NfcNADiUsed;

            switch (pollType)
            {
                case phHal_eOpModesFelica212: 
                case phHal_eOpModesFelica424: 
                    
                        if( !( 0x01 == OutputBuffer[nextTargetIndex+3] && 0xFE == OutputBuffer[nextTargetIndex+4] ) )
                        {
                            psRemoteDevInfoList[*nbTargetFound].hLogHandle =  OutputBuffer[nextTargetIndex];
                            psRemoteDevInfoList[*nbTargetFound].SessionOpened = 0;
                            if (pollType == phHal_eOpModesFelica212)
                                psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesFelica212 ; 
                            else
                                psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesFelica424 ; 
                                
                            psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.CardInfo212_424.CurrentState = phHal_eStateListPolledState;
                                
                            fillStartUp212_424(& psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.CardInfo212_424.Startup212_424,
                                            & OutputBuffer[nextTargetIndex]);
                            (*nbTargetFound) ++;
                        }
                        else
                        {
                            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
                        }
                         
                    break;
                case phHal_eOpModesNfcPassive212: // nfc 212
                case phHal_eOpModesNfcPassive424: // nfc 424
                    ////////////////////
                    // TARGET NFC 212
                    ////////////////////
                    switch (ATROutputBuffer[7])
                    {
                        case 0x00:
                            // fill the good structures
                            psRemoteDevInfoList[*nbTargetFound].hLogHandle =  OutputBuffer[nextTargetIndex];
                            psRemoteDevInfoList[*nbTargetFound].SessionOpened = 0;
                            if (pollType == phHal_eOpModesNfcPassive212)          
                                psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesNfcPassive212;
                            else
                                psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesNfcPassive424;
                
                            psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.NfcInfo212_424.CurrentState = phHal_eStateListPolledState;
                
                            fillStartUp212_424(& psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.NfcInfo212_424.Startup212_424,
                                               & OutputBuffer[nextTargetIndex]);
                            fillNfcProtocol(& psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.NfcInfo212_424.NfcProtocol, 
                                            & ATROutputBuffer[8], 
                                            (uint8_t) (ATROutputBufferSize - 10) );
                            (*nbTargetFound) ++; 
                            break;
                        case 0x01:
                            // unexpected target
                            break;
                        default:
                            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
                            break;    
                    }
                    break;
                default:
                    return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
                    break;
            }
        }
        nextTargetIndex = (uint8_t) (nextTargetIndex + 1 + OutputBuffer[nextTargetIndex+1]);
           
    } // END : for(i=0;i<nbPossibleTarget;i++)
     
    // Re-enable the RF field because IN_ATR command makes the PN532 disable it
	if (phHal_eChipName532 == psHwReference->ChipName)
	{
        Tama_RfConf[0] = 0x01;
        Tama_RfConf[1] = 0x03;
    
        OutputBufferSize = sizeof(OutputBuffer);
        result = phTalTama_Protocol(psHwReference,
                                    RF_CONFIGURATION,
                                    Tama_RfConf,
                                    2,
                                    OutputBuffer,
                                    &OutputBufferSize,
                                    &LEN,
                                    0);
    }

    if( (*nbTargetFound)==0x00)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
    }

    return result;
}
     

/**
 * specific fonction for polling in passive 106 mode type A
 * \param[in] baudrate : the baudrate to poll
 * \param[in,out] nbTargetFound : nb of target allready found
 * \param[in] pNbrOfRemoteDev : maximum number of target to find
 */

NFCSTATUS pollPassive106A(phHal_sHwReference_t           *psHwReference,
                          phHal_eOpModes_t               *pollType,
                          phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                          uint8_t                        *pNbrOfRemoteDev,
                          uint8_t                        *nbTargetFound,
                          phHal_sDevInputParam_t         *puDevInputParam )
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint8_t OutputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t OutputBufferSize = sizeof(OutputBuffer);
    uint8_t nbPossibleTarget,nextTargetIndex;
    uint16_t Tama_InATRSize,i,index = 0;
       
    // initialisation
    uint8_t Tama_inListPassiveTarget[12];
    uint16_t Tama_inListPassiveTarget_CommandSize = 2;

    uint16_t    LEN                 =   0;
       
    uint8_t next,ATSLenght;
    uint8_t Tama_inATR[60]; 
    uint8_t ATROutputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t ATROutputBufferSize = sizeof(ATROutputBuffer);
    uint8_t Tama_SetTamaParam[] = {0x04};
    Tama_inListPassiveTarget[0] = (uint8_t) (*pNbrOfRemoteDev - *nbTargetFound);
    Tama_inListPassiveTarget[1] = 0x00;
    

    //------------------------------------------------------------
    // change TAMA configuration according to the Input Parameters

    SetTamaParameter_Initialize(*pollType, &Tama_SetTamaParam[0], puDevInputParam->DIDiUsed,puDevInputParam->NfcNADiUsed);

    OutputBufferSize = sizeof(OutputBuffer);
    result = phTalTama_Protocol(psHwReference,
                                SET_TAMA_PARAMETERS,
                                Tama_SetTamaParam,
                                1,
                                OutputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                0);
       
    if(result != NFCSTATUS_SUCCESS)
    {
        return result;
    }

    //------------------------------------------------------------



    ////////////////////////////////////////////////////////////
    // send inListPassiveTarget
    ////////////////////////////////////////////////////////////
     
       
    // inListPassiveTarget
    OutputBufferSize = sizeof(OutputBuffer);
    result = phTalTama_Protocol(psHwReference,
                                IN_LIST_PASSIVE_TARGET_CMD,
                                Tama_inListPassiveTarget,
                                Tama_inListPassiveTarget_CommandSize,
                                OutputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                DAL_TIME_OUT);
       
    if((result != NFCSTATUS_SUCCESS))
    {
        return result;
    }
       
    //  Nb Target found with the Inlistpassivetarget
    nbPossibleTarget = OutputBuffer[7];
       
    nextTargetIndex = 8;
       
    for(i=0;i<nbPossibleTarget;i++)
    {
        uint8_t SAK = OutputBuffer[nextTargetIndex+3];
        
        ATSLenght = 0;
        psRemoteDevInfoList[*nbTargetFound].UsedDID    =   puDevInputParam->DIDiUsed;
        psRemoteDevInfoList[*nbTargetFound].UsedNAD    =   puDevInputParam->NfcNADiUsed;
        
        switch (*pollType)
        {
            case phHal_eOpModesNfcPassive106 :
                //############################################################
                //############################################################
                //##  Tama_inAtr command creation
                //############################################################
                //############################################################
     
                Tama_InATRSize = 2;
                next = 0x00;
     
                // verify nfcIdAuto
                if(puDevInputParam->NFCIDAuto == 0)
                {
                    Tama_InATRSize += 10;
                    next = 0x01;
                }
     
                // verify GeneralBytes
                if(puDevInputParam->GeneralByteLength > 0)
                {
                    Tama_InATRSize = (uint16_t) ( Tama_InATRSize 
                                                  + puDevInputParam->GeneralByteLength);
                    next = (uint8_t) (next | 0x02);
                }
     
                Tama_inATR[1] = next;
                index = 2;
     
                // add nfcid3
                if((next & 0x01) != 0)
                {
                    memcpy(& Tama_inATR[index], puDevInputParam->NFCID3i,10);
                    index += 10;
                }
       
                // add general bytes
     
                if((next & 0x02) != 0)
                {
                    memcpy(& Tama_inATR[index],
                           puDevInputParam->GeneralByte,
                           puDevInputParam->GeneralByteLength);
                }


                /////////////////////
                //check selRes

                if ((OutputBuffer[nextTargetIndex+3] & 0x40) > 0)
                {
                    // Target logical number
                    Tama_inATR[0] = OutputBuffer[nextTargetIndex];
 
                    // send inATR
 
                    ATROutputBufferSize = sizeof(ATROutputBuffer);        
                    result = phTalTama_Protocol(psHwReference,
                                                IN_ATR,
                                                Tama_inATR,
                                                Tama_InATRSize,
                                                ATROutputBuffer,
                                                &ATROutputBufferSize,
                                                &LEN,
                                                DAL_TIME_OUT);
              
                    if (result!=NFCSTATUS_SUCCESS 
                        && ((result & PHNFCSTBLOWER) != (NFCSTATUS_CMD_ABORTED & PHNFCSTBLOWER)))
                    {
                        return result;
                    }
              
                    if(result == NFCSTATUS_SUCCESS)
                    {
                        if(ATROutputBuffer[7]!=0x00)
                        {
                            return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
                        }
 
                        // fill the good structures
                        psRemoteDevInfoList[*nbTargetFound].hLogHandle =  OutputBuffer[nextTargetIndex];
                        psRemoteDevInfoList[*nbTargetFound].SessionOpened = 0;
                        psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesNfcPassive106;
                        psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.NfcInfo106.CurrentState = phHal_eStateListSleepState ;
 
                        fillStartUp106(   psHwReference,
                                        & psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.NfcInfo106.Startup106,
                                       & OutputBuffer[nextTargetIndex]);
                        fillNfcProtocol(& psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.NfcInfo106.NfcProtocol, 
                                        & ATROutputBuffer[8], 
                                        (uint8_t) (ATROutputBufferSize - 10) );
                        (*nbTargetFound) ++;
                    }
                }
                break;
            case phHal_eOpModesISO14443_4A :
            {
/*                if ( ( (OutputBuffer[nextTargetIndex+3] & 0x40) == 0)
                  && ( (OutputBuffer[nextTargetIndex+3] & 0x20) != 0)) */

                if( ( ( SAK & 0x40) ==0 )
                   &&(( SAK & 0x20) !=0 ) )
                {
                    // fill the good structures
                    psRemoteDevInfoList[*nbTargetFound].hLogHandle =  OutputBuffer[nextTargetIndex];
                    psRemoteDevInfoList[*nbTargetFound].SessionOpened = 0;
                    psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesISO14443_4A;
                    psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.ISO14443_4A_Info.CurrentState = phHal_eStateListHaltState ;
              
                    fillStartUp106(  psHwReference,
                                    &psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.ISO14443_4A_Info.Startup106,
                                   & OutputBuffer[nextTargetIndex]);

                    ATSLenght = OutputBuffer[nextTargetIndex + 5 + OutputBuffer[nextTargetIndex + 4]];
                    fillIso14443A(& psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.ISO14443_4A_Info.ISO14443_4A_Protocol,
                                 & OutputBuffer[nextTargetIndex + 5 + OutputBuffer[nextTargetIndex + 4]]);

                    (*nbTargetFound) ++;
                } else if (( (OutputBuffer[nextTargetIndex+3] & 0x40) <= 0) && (pollType[1] == phHal_eOpModesMifare))
                {
                    // fill the good structures
                    psRemoteDevInfoList[*nbTargetFound].hLogHandle = OutputBuffer[nextTargetIndex];
                    psRemoteDevInfoList[*nbTargetFound].SessionOpened = 0;
                    psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesMifare;
                    psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.CardInfo106.CurrentState = phHal_eStateListHaltState ;
                    fillStartUp106(  psHwReference,
                                    &psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.CardInfo106.Startup106,
                                   & OutputBuffer[nextTargetIndex]);
              
                    (*nbTargetFound) ++;
                }
            }
            break;
            case phHal_eOpModesMifare :
//                if  ( (OutputBuffer[nextTargetIndex+3] & 0x40) == 0)
                if  (( (SAK & 0x40) == 0)
                     && ((SAK & 0x20) == 0 || (((SAK & 0x28) == 0x28) || ((SAK & 0x30) == 0x30)))
                     )
                {
                    // fill the good structures
                    psRemoteDevInfoList[*nbTargetFound].hLogHandle = OutputBuffer[nextTargetIndex];
                    psRemoteDevInfoList[*nbTargetFound].SessionOpened = 0;
                    psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesMifare;
                    psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.CardInfo106.CurrentState = phHal_eStateListHaltState ;
                    fillStartUp106(  psHwReference,
                                    &psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.CardInfo106.Startup106,
                                   & OutputBuffer[nextTargetIndex]);
              
                    (*nbTargetFound) ++;
                }
                break;
            default:
                return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_INVALID_PARAMETER);
                break;
        }
        nextTargetIndex = (uint8_t) (nextTargetIndex + 5 + OutputBuffer[nextTargetIndex+4]) + ATSLenght;
    } // END : for(i=0;i<nbPossibleTarget;i++)

    if( (*nbTargetFound)==0x00)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
    }

    return result;
}

/**
 * specific fonction for polling in passive 106 mode type B
 * \param[in] psHwReference : Hardware Reference
 * \param[in] pollType : operation modes under which to look for Remote Devices 
 * \param[out] psRemoteDevInfoList : Remote Device Information structure list
 * \param[in,out] nbTargetFound : nb of target already found
 * \param[out] puDevInputParam : input device parameter 
 */

NFCSTATUS pollPassive106B(phHal_sHwReference_t           *psHwReference,
                          phHal_eOpModes_t                pollType,
                          phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                          uint8_t                        *pNbrOfRemoteDev,
                          uint8_t                        *nbTargetFound,
                          phHal_sDevInputParam_t         *puDevInputParam )
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint16_t    LEN                 =   0;
    uint8_t OutputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t OutputBufferSize = sizeof(OutputBuffer);
    uint8_t nbPossibleTarget,nextTargetIndex;
    uint16_t i;

    // initialisation
    uint8_t Tama_inListPassiveTarget[12];
    uint16_t Tama_inListPassiveTarget_CommandSize = 3;
       
    uint8_t Tama_SetTamaParam[] = {0x04};
    Tama_inListPassiveTarget[0] = (uint8_t) (*pNbrOfRemoteDev - *nbTargetFound);
    Tama_inListPassiveTarget[1] = 0x03; /* for 106 kbps type B */
    Tama_inListPassiveTarget[2] = puDevInputParam->ISO14443_4B_AFI;

    //------------------------------------------------------------
    // change TAMA configuration according to the Input Parameters

    SetTamaParameter_Initialize(pollType, &Tama_SetTamaParam[0], puDevInputParam->DIDiUsed,puDevInputParam->NfcNADiUsed);

    OutputBufferSize = sizeof(OutputBuffer);
    result = phTalTama_Protocol(psHwReference,
                                SET_TAMA_PARAMETERS,
                                Tama_SetTamaParam,
                                1,
                                OutputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                0);
       
    if(result != NFCSTATUS_SUCCESS)
    {
        return result;
    }

    //------------------------------------------------------------

    ////////////////////////////////////////////////////////////
    // send inListPassiveTarget
    ////////////////////////////////////////////////////////////
       
    // inListPassiveTarget
    OutputBufferSize = sizeof(OutputBuffer);
    result = phTalTama_Protocol(psHwReference,
                                IN_LIST_PASSIVE_TARGET_CMD,
                                Tama_inListPassiveTarget,
                                Tama_inListPassiveTarget_CommandSize,
                                OutputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                DAL_TIME_OUT);
       
    if((result != NFCSTATUS_SUCCESS))
    {
        return result;
    }
       
    //  Nb Target found with the Inlistpassivetarget
    nbPossibleTarget = OutputBuffer[7];
       
    nextTargetIndex = 8;
       
    for(i=0;i<nbPossibleTarget;i++)
    {
        // fill the good structures
        psRemoteDevInfoList[*nbTargetFound].hLogHandle = OutputBuffer[nextTargetIndex];
        psRemoteDevInfoList[*nbTargetFound].SessionOpened = 0;
        psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesISO14443_4B;
        psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.ISO14443_4B_Info.CurrentState = phHal_eStateListHaltState ;
        fillIso14443B (&psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.ISO14443_4B_Info.ISO14443_4B_Protocol,
                       &OutputBuffer[nextTargetIndex]);
        (*nbTargetFound) ++;
        nextTargetIndex = (uint8_t) (nextTargetIndex + 13 + OutputBuffer[nextTargetIndex+12]);
    }

    if( (*nbTargetFound)==0x00)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
    }

    return result;
}

/**
 * specific fonction for polling in Jewel passive 106 mode
 * \param[in] psHwReference : Hardware Reference
 * \param[in] pollType : operation modes under which to look for Remote Devices 
 * \param[out] psRemoteDevInfoList : Remote Device Information structure list
 * \param[in,out] nbTargetFound : nb of target already found
 * \param[out] puDevInputParam : input device parameter 
 */

NFCSTATUS pollPassive106Jewel(phHal_sHwReference_t           *psHwReference,
                              phHal_eOpModes_t                pollType,
                              phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                              uint8_t                        *pNbrOfRemoteDev,
                              uint8_t                        *nbTargetFound,
                              phHal_sDevInputParam_t         *puDevInputParam )
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint16_t    LEN                 =   0;
    uint8_t OutputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t OutputBufferSize = sizeof(OutputBuffer);
    uint8_t nbPossibleTarget,nextTargetIndex;
    uint16_t i;

    // initialisation
    uint8_t Tama_inListPassiveTarget[12];
    uint16_t Tama_inListPassiveTarget_CommandSize = 2;
       
    uint8_t Tama_SetTamaParam[] = {0x04};
    if(*pNbrOfRemoteDev>1)
        *pNbrOfRemoteDev = 1;
    Tama_inListPassiveTarget[0] = (uint8_t) (*pNbrOfRemoteDev - *nbTargetFound);
    Tama_inListPassiveTarget[1] = 0x04; /* for 106 kbps Innovision Jewel tag */



    //------------------------------------------------------------
    // change TAMA configuration according to the Input Parameters

    OutputBufferSize = sizeof(OutputBuffer);
    result = phTalTama_Protocol(psHwReference,
                                SET_TAMA_PARAMETERS,
                                Tama_SetTamaParam,
                                1,
                                OutputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                0);
       
    if(result != NFCSTATUS_SUCCESS)
    {
        return result;
    }

    //------------------------------------------------------------

    ////////////////////////////////////////////////////////////
    // send inListPassiveTarget
    ////////////////////////////////////////////////////////////
       
    // inListPassiveTarget
    OutputBufferSize = sizeof(OutputBuffer);
    result = phTalTama_Protocol(psHwReference,
                                IN_LIST_PASSIVE_TARGET_CMD,
                                Tama_inListPassiveTarget,
                                Tama_inListPassiveTarget_CommandSize,
                                OutputBuffer,
                                &OutputBufferSize,
                                &LEN,
                                DAL_TIME_OUT);
       
    if((result != NFCSTATUS_SUCCESS))
    {
        return result;
    }
       
    //  Nb Target found with the Inlistpassivetarget
    nbPossibleTarget = OutputBuffer[7];
       
    nextTargetIndex = 8;
       
    for(i=0;i<nbPossibleTarget;i++)
    {
        // fill the good structures
        psRemoteDevInfoList[*nbTargetFound].hLogHandle = OutputBuffer[nextTargetIndex];
        psRemoteDevInfoList[*nbTargetFound].SessionOpened = 0;
        psRemoteDevInfoList[*nbTargetFound].OpMode = phHal_eOpModesJewel;
        psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.ISO14443_4B_Info.CurrentState = phHal_eStateListHaltState ;
        fillStartUpJewel (&psRemoteDevInfoList[*nbTargetFound].RemoteDevInfo.JewelInfo.StartupJewel,
                          &OutputBuffer[nextTargetIndex]);
        (*nbTargetFound) ++;
        nextTargetIndex += 7;
    }

    if( (*nbTargetFound)==0x00)
    {
        return  PHNFCSTVAL(CID_NFC_TAL,NFCSTATUS_CMD_ABORTED);
    }

    return result;
}

/*	
 *  Internally Used Functions	
 */

uint8_t chipFirmwareVerCompare(phHal_sHwReference_t   *psHwReference, char const * const op, uint8_t ver, uint8_t rev)
{
	uint16_t	rv1	=	(((uint16_t)psHwReference->FirmwareVersion)	<< 8)	+	(uint16_t)psHwReference->FirmwareRevision;
	uint16_t	rv2	=	(((uint16_t)ver)	<< 8)	+	(uint16_t)rev;

	if(strlen(op) != 1	&&	strlen(op)	!=	2)
	{
		return 0;
	}
    
	if(!strcmp("<", op))
	{
		if(rv1	<	rv2)
			return 1;
		else
			return 0;
	}
	else
	if(!strcmp("<=", op))
	{
			if(rv1	<=	rv2)
				return 1;
			else
				return 0;
	}
	else
	if(!strcmp(">", op))
	{
		if(rv1	>	rv2)
			return 1;
		else
			return 0;
	}
	else
	if(!strcmp(">=", op))
	{
			if(rv1	>=	rv2)
				return 1;
			else
				return 0;
	}
	else
	if(!strcmp("==", op))
	{
			if(rv1	==	rv2)
				return 1;
			else
				return 0;
	}
	else
	{
		return 0;
	}
}

/**
 * specific fonction for automatic polling (only available for PN532)
 * \param[in] psHwReference : Hardware Reference
 * \param[in] OpModes : operation modes under which to look for Remote Devices 
 * \param[out] psRemoteDevInfoList : Remote Device Information structure list
 * \param[in,out] pNbrOfRemoteDev : maximum number of remote devices to detect
 * \param[out] puDevInputParam : input device parameter 
 */
#ifdef AUTO_POLL

NFCSTATUS autoPoll(phHal_sHwReference_t           *psHwReference,
                   phHal_eOpModes_t                OpModes[],
                   phHal_sRemoteDevInformation_t   psRemoteDevInfoList[],
                   uint8_t                        *pNbrOfRemoteDev,
                   phHal_sDevInputParam_t         *puDevInputParam )
{
    NFCSTATUS result = NFCSTATUS_SUCCESS;
    uint16_t    LEN                 =   0;
    uint8_t Tama_inAutoPoll[17];
    uint8_t Tama_inAutoPoll_CommandSize;
    uint8_t OutputBuffer[MAX_SIZE_DAL_BUFFER];
    uint16_t OutputBufferSize = sizeof(OutputBuffer);
    uint8_t targetType;
    uint8_t dataLength;
    uint8_t nextTargetIndex;
    uint8_t ATSIndex;
    uint8_t NfcProtocolIndex;
    uint8_t i;
    uint8_t pollGenericPassive106 = 0;
    uint8_t pollGenericPassive212 = 0;
    uint8_t pollGenericPassive424 = 0;

    Tama_inAutoPoll[0] = 1; // PollNr set to no retries
    Tama_inAutoPoll[1] = 1; // Period N*150ms
    Tama_inAutoPoll_CommandSize = 2;

	for (i=0; (Tama_inAutoPoll_CommandSize <= 17) && (OpModes[i] != phHal_eOpModesArrayTerminator); i++)
    {
        switch (OpModes[i])
        {
            case phHal_eOpModesNfcActive106:
				Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_DEP_ACTIVE_106;
                break;

            case phHal_eOpModesNfcActive212:
				Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_DEP_ACTIVE_212;
				break;

            case phHal_eOpModesNfcActive424:
				Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_DEP_ACTIVE_424;
				break;

            case phHal_eOpModesISO14443_4B:
                Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_ISO14443_4B;
				break;
                
            case phHal_eOpModesJewel:
                Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_JEWEL;
				break;

            case phHal_eOpModesISO14443_4A:
                if(OpModes[i+1] == phHal_eOpModesMifare)
                {
                    i++;
                    if (!pollGenericPassive106)
                    {
                        Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_GENERIC_PASSIVE_106;
                        pollGenericPassive106 = 1;
                    }
                } else
                {
                    Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_ISO14443_4A;
                }
				break;
            case phHal_eOpModesMifare:
                Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_MIFARE;
				break;
			case phHal_eOpModesNfcPassive106:
                Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_DEP_PASSIVE_106;
				break;
			case phHal_eOpModesNfcPassive212:
                Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_DEP_PASSIVE_212;
				break;
            case phHal_eOpModesFelica212:
                Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_FELICA_212;
				break;
			case phHal_eOpModesNfcPassive424:
                Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_DEP_PASSIVE_424;
				break;
            case phHal_eOpModesFelica424:
                Tama_inAutoPoll[Tama_inAutoPoll_CommandSize++] = TARGET_TYPE_FELICA_424;
				break;
            default:
                result = NFCSTATUS_INVALID_PARAMETER;
                break;
        }
    }

    result = phTalTama_Protocol(psHwReference,
                                IN_AUTO_POLL,
                                Tama_inAutoPoll,
                                Tama_inAutoPoll_CommandSize,
                                OutputBuffer,
                                &OutputBufferSize,
                                AUTO_POLL_TIME_OUT);

    if (result != NFCSTATUS_SUCCESS)
    {
        return result;
    }
    
    *pNbrOfRemoteDev = OutputBuffer[7];
    nextTargetIndex = 8;

    for (i=0; i<*pNbrOfRemoteDev; i++)
    {
        targetType = OutputBuffer[nextTargetIndex];
        dataLength = OutputBuffer[nextTargetIndex+1];
        nextTargetIndex += 2;

        psRemoteDevInfoList[i].hLogHandle = OutputBuffer[nextTargetIndex];
        psRemoteDevInfoList[i].SessionOpened = 0;
        psRemoteDevInfoList[i].UsedDID =   puDevInputParam->DIDiUsed;
        psRemoteDevInfoList[i].UsedNAD =   puDevInputParam->NfcNADiUsed;

        switch (targetType)
        {
            case TARGET_TYPE_DEP_ACTIVE_106:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesNfcActive106;
                psRemoteDevInfoList[i].RemoteDevInfo.NfcActiveInfo.CurrentState = phHal_eStateListPolledState ;
                fillNfcProtocol (&psRemoteDevInfoList[i].RemoteDevInfo.NfcActiveInfo.NfcProtocol, 
                                 &OutputBuffer[nextTargetIndex], 
                                 dataLength);
                break;

            case TARGET_TYPE_DEP_ACTIVE_212:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesNfcActive212;
                psRemoteDevInfoList[i].RemoteDevInfo.NfcActiveInfo.CurrentState = phHal_eStateListPolledState ;
                fillNfcProtocol (&psRemoteDevInfoList[i].RemoteDevInfo.NfcActiveInfo.NfcProtocol, 
                                 &OutputBuffer[nextTargetIndex], 
                                 dataLength);
                break;

            case TARGET_TYPE_DEP_ACTIVE_424:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesNfcActive424;
                psRemoteDevInfoList[i].RemoteDevInfo.NfcActiveInfo.CurrentState = phHal_eStateListPolledState ;
                fillNfcProtocol (&psRemoteDevInfoList[i].RemoteDevInfo.NfcActiveInfo.NfcProtocol, 
                                 &OutputBuffer[nextTargetIndex], 
                                 dataLength);
                break;

            case TARGET_TYPE_DEP_PASSIVE_106:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesNfcPassive106;
                psRemoteDevInfoList[i].RemoteDevInfo.NfcInfo106.CurrentState = phHal_eStateListSleepState ;
                fillStartUp106 (psHwReference,
                                &psRemoteDevInfoList[i].RemoteDevInfo.NfcInfo106.Startup106,
                                &OutputBuffer[nextTargetIndex]);
                ATSIndex = nextTargetIndex + 5 + OutputBuffer[nextTargetIndex + 4];
                NfcProtocolIndex = ATSIndex + OutputBuffer[ATSIndex];
                fillNfcProtocol (&psRemoteDevInfoList[i].RemoteDevInfo.NfcInfo106.NfcProtocol, 
                                 &OutputBuffer[NfcProtocolIndex], 
                                 (dataLength - NfcProtocolIndex));
                break;

            case TARGET_TYPE_DEP_PASSIVE_212:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesNfcPassive212;
                psRemoteDevInfoList[i].RemoteDevInfo.NfcInfo212_424.CurrentState = phHal_eStateListPolledState ;
                fillStartUp212_424 (&psRemoteDevInfoList[i].RemoteDevInfo.NfcInfo212_424.Startup212_424,
                                    &OutputBuffer[nextTargetIndex]);
                break;

            case TARGET_TYPE_DEP_PASSIVE_424:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesNfcPassive424;
                psRemoteDevInfoList[i].RemoteDevInfo.NfcInfo212_424.CurrentState = phHal_eStateListPolledState ;
                fillStartUp212_424 (&psRemoteDevInfoList[i].RemoteDevInfo.NfcInfo212_424.Startup212_424,
                                    &OutputBuffer[nextTargetIndex]);
                break;

            case TARGET_TYPE_MIFARE:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesMifare;
                psRemoteDevInfoList[i].RemoteDevInfo.CardInfo106.CurrentState = phHal_eStateListHaltState ;
                fillStartUp106 (psHwReference,
                                &psRemoteDevInfoList[i].RemoteDevInfo.CardInfo106.Startup106,
                                &OutputBuffer[nextTargetIndex]);
                break;

            case TARGET_TYPE_ISO14443_4A:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesISO14443_4A;
                psRemoteDevInfoList[i].RemoteDevInfo.CardInfo106.CurrentState = phHal_eStateListHaltState ;
                fillStartUp106 (psHwReference,
                                &psRemoteDevInfoList[i].RemoteDevInfo.CardInfo106.Startup106,
                                &OutputBuffer[nextTargetIndex]);
                fillIso14443A (&psRemoteDevInfoList[i].RemoteDevInfo.ISO14443_4A_Info.ISO14443_4A_Protocol,
                               &OutputBuffer[nextTargetIndex + 5 + OutputBuffer[nextTargetIndex + 4]]);
                break;

            case TARGET_TYPE_ISO14443_4B:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesISO14443_4B;
                psRemoteDevInfoList[i].RemoteDevInfo.CardInfo106.CurrentState = phHal_eStateListHaltState ;
                fillIso14443B (&psRemoteDevInfoList[i].RemoteDevInfo.ISO14443_4B_Info.ISO14443_4B_Protocol,
                               &OutputBuffer[nextTargetIndex]);
                break;

            case TARGET_TYPE_JEWEL:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesJewel;
                psRemoteDevInfoList[i].RemoteDevInfo.CardInfo106.CurrentState = phHal_eStateListHaltState ;
                fillStartUpJewel (&psRemoteDevInfoList[i].RemoteDevInfo.JewelInfo.StartupJewel,
                                  &OutputBuffer[nextTargetIndex]);
                break;

			case TARGET_TYPE_FELICA_212:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesFelica212;
                psRemoteDevInfoList[i].RemoteDevInfo.CardInfo212_424.CurrentState = phHal_eStateListPolledState ;
                fillStartUp212_424 (&psRemoteDevInfoList[i].RemoteDevInfo.CardInfo212_424.Startup212_424,
                                    &OutputBuffer[nextTargetIndex]);
                break;
                
            case TARGET_TYPE_FELICA_424:
                psRemoteDevInfoList[i].OpMode = phHal_eOpModesFelica424;
                psRemoteDevInfoList[i].RemoteDevInfo.CardInfo212_424.CurrentState = phHal_eStateListPolledState ;
                fillStartUp212_424 (&psRemoteDevInfoList[i].RemoteDevInfo.CardInfo212_424.Startup212_424,
                                    &OutputBuffer[nextTargetIndex]);
                break;

            default:
                result = NFCSTATUS_INVALID_PARAMETER;
                break;
        }
        nextTargetIndex += dataLength;
    }
    
    return result;
}
#endif /*AUTO_POLL*/

#endif /* PHHAL_VERSION_ACQUISITION */
