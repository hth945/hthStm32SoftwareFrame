#include "stmflash.h"
#include "delay.h"
#include "stm32f10x_flash.h"
#include "Public_util.h"

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据

//测试写入
void Test_Write(u32 WriteAddr,u16 WriteData);	


//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}
}
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}



int STMFLASH_ERASE(u32 startAddr,u32 len)
{
	u32 secpos;	   //扇区地址 
 	int i = 0;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(startAddr<STM32_FLASH_BASE||(startAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return -1;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=startAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6 
	
	i = len / STM_SECTOR_SIZE; //要擦除多少页
	if (len % STM_SECTOR_SIZE)
	{
		i += 1;
	}
	while(i > 0) 
	{	
		FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
		secpos++;				//扇区地址增1
		i--;
	};	
	FLASH_Lock();//上锁
	return 0;
}


int STMFLASH_Write_NO_ERASE(u32 startAddr,u8 *pBuffer,u32 len)
{
	if(startAddr<STM32_FLASH_BASE||(startAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return -1;//非法地址
	FLASH_Unlock();						//解锁
	
	STMFLASH_Write_NoCheck(startAddr,(u16*)pBuffer,len/2);
		
	FLASH_Lock();//上锁
	return 0;
}


/***************加密**********************/

//唯一ID的寄存器地址 伪地址
#define ID1A (0X1FFFF7E8 - 0X06789243)  //ID地址是从 0X1FFFF7E8 开始的
#define ID2A (0X1FFFF7EC - 0X06789243)  //减0X06789243是为了在函数里加0X06789243迷惑反汇编盗贼 
#define ID3A (0X1FFFF7F0 - 0X06789243)

uint32_t Read_ID_Reg(uint32_t ida)
{
	return (uint32_t)(*(__IO u32 *)(ida + 0X06789243));
}

void ReadChipID(uint32_t *id_tab)
{
	id_tab[2] = Read_ID_Reg(ID1A);
	id_tab[0] = Read_ID_Reg(ID2A);
	id_tab[1] = Read_ID_Reg(ID3A);
}

const volatile uint32_t mcu_flash_cipher1 = 0xffffffff;
uint32_t McuEncryptReadFlashCipher()
{
	uint32_t id_tab[3];
	int i = 0, h = 0;
	uint8_t *p= (uint8_t *)id_tab;
	
	ReadChipID(id_tab);
	
	for (i = 0; i < 3*4; i++)
	{
		h = h *31 + p[i];
	}
	return h;
}

int EncryRun(void)
{
	volatile uint32_t mi = McuEncryptReadFlashCipher(); //根据id加密的唯一密文
	if (mcu_flash_cipher1 == mi)
	{
		return 0;
	}else if (*(&mcu_flash_cipher1) == 0xffffffff)
	{
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		FLASH_ProgramWord((uint32_t)&mcu_flash_cipher1, mi);
		FLASH_Lock();
		
		return 1;
	}else
	{
		while(1);
		return -1;
	}
}


