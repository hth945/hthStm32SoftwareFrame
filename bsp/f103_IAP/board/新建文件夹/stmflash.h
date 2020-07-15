#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

//用户根据自己的需要设置
#define STM32_FLASH_SIZE 64 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
//FLASH解锁键值
 



int STMFLASH_Write_NO_ERASE(u32 startAddr,u8 *pBuffer,u32 len);
int STMFLASH_ERASE(u32 startAddr,u32 len);
int EncryRun(void);

#endif

















