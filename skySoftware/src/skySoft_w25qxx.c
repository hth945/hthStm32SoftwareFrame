//V0.0.2

#include "skySoft_w25qxx.h"

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q256
//容量为32M字节,共有512个Block,8192个Sector 

						
////////////////////////////////////////////////////////////////////////////////// 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9



//-------------------

__weak void skySoft_W25QXX_Delay(struct _Tspi_adapter *self){
}

        
void skySoft_W25QXX_DeInit(w25qxx_drv * drv){
    P_SPI_DeInit(&drv->adap);
    drv->addr_over3byte=0;
    drv->delay=skySoft_W25QXX_Delay;
    drv->id=0;
    drv->adap.delay=skySoft_W25QXX_Delay;   
}  

void skySoft_W25QXX_Init(w25qxx_drv * drv){
    P_SPI_Init(&drv->adap);
}  

//读取W25QXX的状态寄存器，W25QXX一共有3个状态寄存器
//状态寄存器1：
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
//状态寄存器2：
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//状态寄存器3：
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:状态寄存器号，范:1~3
//返回值:状态寄存器值
uint8_t skySoft_W25QXX_ReadSR(w25qxx_drv * drv,uint8_t regno)   {  
	uint8_t byte=0,command=0; 
    spi_msg msg[2];
    
    switch(regno){
        case 1:
            command=W25X_ReadStatusReg1;    //读状态寄存器1指令
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //读状态寄存器2指令
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //读状态寄存器3指令
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }    

    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=&command;
    
    msg[1].len=1;
    msg[1].type=SPI_OnlyR;
    msg[1].rxbuf=&byte;  
    
    P_SPI_transfer(&drv->adap,msg,2);
    
	return byte;   
} 
//写W25QXX状态寄存器
void skySoft_W25QXX_Write_SR(w25qxx_drv * drv,uint8_t regno,uint8_t sr)   {   
    uint8_t command=0;
    spi_msg msg[2];
    
    switch(regno){
        case 1:
            command=W25X_WriteStatusReg1;    //写状态寄存器1指令
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //写状态寄存器2指令
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //写状态寄存器3指令
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }   
    
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=&command;
    
    msg[1].len=1;
    msg[1].type=SPI_OnlyW;
    msg[1].txbuf=&sr;  
    
    P_SPI_transfer(&drv->adap,msg,2);   
}   

//等待空闲
void skySoft_W25QXX_Wait_Busy(w25qxx_drv * drv){   
	while((skySoft_W25QXX_ReadSR(drv,1)&0x01)==0x01);   // 等待BUSY位清空
}  

//W25QXX写使能与写禁止	
//将WEL置位与清零  
void skySoft_W25QXX_Write_Enable(w25qxx_drv * drv,uint8_t en)   {
    spi_msg msg[1];
    uint8_t txbuf[2];
    
    txbuf[0]=en?W25X_WriteEnable:W25X_WriteDisable;
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);   
} 


//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
//0XEF18,表示芯片型号为W25Q256
uint16_t skySoft_W25QXX_ReadID(w25qxx_drv * drv){
	uint16_t Temp = 0;	  
    spi_msg msg[2];
    uint8_t txbuf[4],rxbuf[2];
    
    txbuf[0]=0x90;
    txbuf[1]=0x00;
    txbuf[2]=0x00;
    txbuf[3]=0x00;
    msg[0].len=4;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf; 
    
    msg[1].len=2;
    msg[1].type=SPI_OnlyR;
    msg[1].rxbuf=rxbuf;   
    
    P_SPI_transfer(&drv->adap,msg,2);  

    Temp=rxbuf[0]<<8;
    Temp|=rxbuf[1];
    
    if(Temp==0XEF18){
        drv->addr_over3byte=1;
    }
	return Temp;
}   	

//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//addr:开始读取的地址(24bit)
//len:要读取的字节数(最大65535)
//buf:数据存储区
void skySoft_W25QXX_Read(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf){ 
    spi_msg msg[3];
    uint8_t txbuf1[1],txbuf2[4];
  
    txbuf1[0]=W25X_ReadData;
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf1;
    
    if(drv->addr_over3byte==0){
        txbuf2[0]=(uint8_t)((addr)>>16);
        txbuf2[1]=(uint8_t)((addr)>>8);
        txbuf2[2]=(uint8_t)((addr)>>0);
        msg[1].len=3;
        msg[1].type=SPI_OnlyW;
        msg[1].txbuf=txbuf2;        
    }else{
        txbuf2[0]=(uint8_t)((addr)>>24);
        txbuf2[1]=(uint8_t)((addr)>>16);
        txbuf2[2]=(uint8_t)((addr)>>8);
        txbuf2[3]=(uint8_t)((addr)>>0);
        msg[1].len=4;
        msg[1].type=SPI_OnlyW;
        msg[1].txbuf=txbuf2;        
    }

    msg[2].len=len;
    msg[2].type=SPI_OnlyR;
    msg[2].rxbuf=buf;     

    P_SPI_transfer(&drv->adap,msg,3);  
    
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void skySoft_W25QXX_Write_Page(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf){
    spi_msg msg[3];
    uint8_t txbuf1[1],txbuf2[4];
    
    skySoft_W25QXX_Write_Enable(drv,1);//SET WEL 
    
    txbuf1[0]=W25X_PageProgram;
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf1;    
    
    if(drv->addr_over3byte==0){
        txbuf2[0]=(uint8_t)((addr)>>16);
        txbuf2[1]=(uint8_t)((addr)>>8);
        txbuf2[2]=(uint8_t)((addr)>>0);
        msg[1].len=3;
        msg[1].type=SPI_OnlyW;
        msg[1].txbuf=txbuf2;        
    }else{
        txbuf2[0]=(uint8_t)((addr)>>24);
        txbuf2[1]=(uint8_t)((addr)>>16);
        txbuf2[2]=(uint8_t)((addr)>>8);
        txbuf2[3]=(uint8_t)((addr)>>0);
        msg[1].len=4;
        msg[1].type=SPI_OnlyW;
        msg[1].txbuf=txbuf2;        
    }    

    msg[2].len=len;
    msg[2].type=SPI_OnlyW;
    msg[2].txbuf=buf;     

    P_SPI_transfer(&drv->adap,msg,3); 

	skySoft_W25QXX_Wait_Busy(drv);					   //等待写入结束
    
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void skySoft_W25QXX_Write_NoCheck(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf){ 			 		 
	uint16_t pageremain;	   
    
	pageremain=256-addr%256; //单页剩余的字节数		 	    
	if(len<=pageremain)
        pageremain=len;//不大于256个字节
    
	while(1){	   
		skySoft_W25QXX_Write_Page(drv,addr,pageremain,buf);
		if(len==pageremain)
            break;//写入结束了
	 	else{ //NumByteToWrite>pageremain
			buf+=pageremain;
			addr+=pageremain;	

			len-=pageremain;//减去已经写入了的字节数
			if(len>256)
                pageremain=256; //一次可以写入256个字节
			else 
                pageremain=len; 	  //不够256个字节了
		}
	};	    
} 

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms
void skySoft_W25QXX_Erase_Sector(w25qxx_drv * drv,uint32_t addr){  
    spi_msg msg[2];
    uint8_t txbuf1[1],txbuf2[4];    

 	addr*=4096;
    skySoft_W25QXX_Write_Enable(drv,1);//SET WEL 	 
    skySoft_W25QXX_Wait_Busy(drv);   
    
    txbuf1[0]=W25X_SectorErase;//发送扇区擦除指令 
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf1;    
    
    if(drv->addr_over3byte==0){
        txbuf2[0]=(uint8_t)((addr)>>16);
        txbuf2[1]=(uint8_t)((addr)>>8);
        txbuf2[2]=(uint8_t)((addr)>>0);
        msg[1].len=3;
        msg[1].type=SPI_OnlyW;
        msg[1].txbuf=txbuf2;        
    }else{
        txbuf2[0]=(uint8_t)((addr)>>24);
        txbuf2[1]=(uint8_t)((addr)>>16);
        txbuf2[2]=(uint8_t)((addr)>>8);
        txbuf2[3]=(uint8_t)((addr)>>0);
        msg[1].len=4;
        msg[1].type=SPI_OnlyW;
        msg[1].txbuf=txbuf2;        
    }    

    P_SPI_transfer(&drv->adap,msg,2); 
    
    skySoft_W25QXX_Wait_Busy(drv);//等待擦除完成
}  

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
uint8_t W25QXX_BUFFER[4096];		 
void skySoft_W25QXX_Write(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf){ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;
    
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=addr/4096;//扇区地址  
	secoff=addr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   

 	if(len<=secremain)
        secremain=len;//不大于4096个字节
    
	while(1) {	
		skySoft_W25QXX_Read(drv,secpos*4096,4096,W25QXX_BUF);//读出整个扇区的内容
		for(i=0;i<secremain;i++){//校验数据
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
        
		if(i<secremain){//需要擦除
			skySoft_W25QXX_Erase_Sector(drv,secpos);//擦除这个扇区
			for(i=0;i<secremain;i++){	   //复制
				W25QXX_BUF[i+secoff]=buf[i];	  
			}
			skySoft_W25QXX_Write_NoCheck(drv,secpos*4096,4096,W25QXX_BUF);//写入整个扇区  
		}else{
            skySoft_W25QXX_Write_NoCheck(drv,addr,secremain,buf);//写已经擦除了的,直接写入扇区剩余区间. 
        }
        
		if(len==secremain){//写入结束了
            break;
		}else{//写入未结束
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	buf+=secremain; //指针偏移
			addr+=secremain;//写地址偏移	   
		   	len-=secremain;	//字节数递减
			if(len>4096)
                secremain=4096;	//下一个扇区还是写不完
			else 
                secremain=len;	//下一个扇区可以写完了
		}	 
	};	 
}

//擦除整个芯片		  
//等待时间超长...
void skySoft_W25QXX_Erase_Chip(w25qxx_drv * drv){   
    spi_msg msg[1];
    uint8_t txbuf[2];
    
    skySoft_W25QXX_Write_Enable(drv,1);//SET WEL 
    skySoft_W25QXX_Wait_Busy(drv);   

    txbuf[0]=W25X_ChipErase;//发送片擦除命令  
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);  
    
	skySoft_W25QXX_Wait_Busy(drv); //等待芯片擦除结束
}   


/*
//进入掉电模式
void skySoft_W25QXX_PowerDown(w25qxx_drv * drv){ 
    spi_msg msg[1];
    uint8_t txbuf[2];
    
    txbuf[0]=W25X_PowerDown;//发送掉电命令 
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);      
    drv->delay(3);//等待TPD  
}   
*/

/*
//唤醒
void skySoft_W25QXX_WAKEUP(w25qxx_drv * drv){  
    spi_msg msg[1];
    uint8_t txbuf[2];
    
    txbuf[0]=W25X_ReleasePowerDown;
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);  
    drv->delay(3);//等待TRES1                            
}   
*/

