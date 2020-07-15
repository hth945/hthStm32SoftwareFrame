#include "stmflash.h"
#include "delay.h"
#include "stm32f10x_flash.h"
#include "Public_util.h"

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);	


//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}
}
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}



int STMFLASH_ERASE(u32 startAddr,u32 len)
{
	u32 secpos;	   //������ַ 
 	int i = 0;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if(startAddr<STM32_FLASH_BASE||(startAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return -1;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr=startAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6 
	
	i = len / STM_SECTOR_SIZE; //Ҫ��������ҳ
	if (len % STM_SECTOR_SIZE)
	{
		i += 1;
	}
	while(i > 0) 
	{	
		FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
		secpos++;				//������ַ��1
		i--;
	};	
	FLASH_Lock();//����
	return 0;
}


int STMFLASH_Write_NO_ERASE(u32 startAddr,u8 *pBuffer,u32 len)
{
	if(startAddr<STM32_FLASH_BASE||(startAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return -1;//�Ƿ���ַ
	FLASH_Unlock();						//����
	
	STMFLASH_Write_NoCheck(startAddr,(u16*)pBuffer,len/2);
		
	FLASH_Lock();//����
	return 0;
}


/***************����**********************/

//ΨһID�ļĴ�����ַ α��ַ
#define ID1A (0X1FFFF7E8 - 0X06789243)  //ID��ַ�Ǵ� 0X1FFFF7E8 ��ʼ��
#define ID2A (0X1FFFF7EC - 0X06789243)  //��0X06789243��Ϊ���ں������0X06789243�Ի󷴻����� 
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
	volatile uint32_t mi = McuEncryptReadFlashCipher(); //����id���ܵ�Ψһ����
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


