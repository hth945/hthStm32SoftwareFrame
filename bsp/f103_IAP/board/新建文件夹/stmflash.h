#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 64 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
 



int STMFLASH_Write_NO_ERASE(u32 startAddr,u8 *pBuffer,u32 len);
int STMFLASH_ERASE(u32 startAddr,u32 len);
int EncryRun(void);

#endif

















