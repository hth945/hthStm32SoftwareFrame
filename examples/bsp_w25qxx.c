#include "bsp_w25qxx.h"
#include "skySoft_w25qxx.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "sysio.h"
#include "spi.h"

void W25QXX_GPIO_Init(void);
void W25QXX_Delay(void);
void W25QXX_CS(uint8_t level){PAout(15)=level;}
void W25QXX_MOSI(uint8_t level){PBout(5)=level;}
uint8_t W25QXX_MISO(void){return PBin(4);}
void W25QXX_SCK(uint8_t level){PBout(3)=level;}
    
w25qxx_drv w25q;
void W25QXX_Init(){
    
    W25QXX_GPIO_Init();
    SPI1_Init();
	
    skySoft_W25QXX_DeInit(&w25q);
    w25q.adap.CPOL=CPOL_High;
    w25q.adap.CPHA=CPHA_2Edge;
    w25q.adap.cs_set=W25QXX_CS;
    w25q.adap.mosi_set=W25QXX_MOSI;
    w25q.adap.miso_read=W25QXX_MISO;
    w25q.adap.sck_set=W25QXX_SCK;
    w25q.delay=W25QXX_Delay;
    
    w25q.adap.rwbyte=SPI1_ReadWriteByte;
    
    skySoft_W25QXX_Init(&w25q);
}  
 
void W25QXX_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;		 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_3;		 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;		 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
}

void W25QXX_Delay(void){
//    Delay_us(1);
}

uint16_t W25QXX_ReadID(void){
    return skySoft_W25QXX_ReadID(&w25q);
}

void W25QXX_Read(uint32_t addr,uint16_t len,uint8_t* buf){
    skySoft_W25QXX_Read(&w25q,addr,len,buf);
}

void W25QXX_Write_Page(uint32_t addr,uint16_t len,uint8_t* buf){
    skySoft_W25QXX_Write_Page(&w25q,addr,len,buf);
}

void W25QXX_Write_NoCheck(uint32_t addr,uint16_t len,uint8_t* buf){
    skySoft_W25QXX_Write_NoCheck(&w25q,addr,len,buf);
}

void W25QXX_Write(uint32_t addr,uint16_t len,uint8_t* buf){
    skySoft_W25QXX_Write(&w25q,addr,len,buf);
}

void W25QXX_Erase_Sector(uint32_t addr){
    skySoft_W25QXX_Erase_Sector(&w25q,addr);
}

void W25QXX_Erase_Chip(void){
    skySoft_W25QXX_Erase_Chip(&w25q);
}
