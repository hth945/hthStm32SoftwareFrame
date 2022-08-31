//V0.0.2

#include "skySoft_w25qxx.h"

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q256
//����Ϊ32M�ֽ�,����512��Block,8192��Sector 

						
////////////////////////////////////////////////////////////////////////////////// 
//ָ���
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

//��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
//״̬�Ĵ���1��
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//״̬�Ĵ���2��
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//״̬�Ĵ���3��
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:״̬�Ĵ����ţ���:1~3
//����ֵ:״̬�Ĵ���ֵ
uint8_t skySoft_W25QXX_ReadSR(w25qxx_drv * drv,uint8_t regno)   {  
	uint8_t byte=0,command=0; 
    spi_msg msg[2];
    
    switch(regno){
        case 1:
            command=W25X_ReadStatusReg1;    //��״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //��״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //��״̬�Ĵ���3ָ��
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
//дW25QXX״̬�Ĵ���
void skySoft_W25QXX_Write_SR(w25qxx_drv * drv,uint8_t regno,uint8_t sr)   {   
    uint8_t command=0;
    spi_msg msg[2];
    
    switch(regno){
        case 1:
            command=W25X_WriteStatusReg1;    //д״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //д״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //д״̬�Ĵ���3ָ��
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

//�ȴ�����
void skySoft_W25QXX_Wait_Busy(w25qxx_drv * drv){   
	while((skySoft_W25QXX_ReadSR(drv,1)&0x01)==0x01);   // �ȴ�BUSYλ���
}  

//W25QXXдʹ����д��ֹ	
//��WEL��λ������  
void skySoft_W25QXX_Write_Enable(w25qxx_drv * drv,uint8_t en)   {
    spi_msg msg[1];
    uint8_t txbuf[2];
    
    txbuf[0]=en?W25X_WriteEnable:W25X_WriteDisable;
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);   
} 


//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
//0XEF18,��ʾоƬ�ͺ�ΪW25Q256
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

//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//addr:��ʼ��ȡ�ĵ�ַ(24bit)
//len:Ҫ��ȡ���ֽ���(���65535)
//buf:���ݴ洢��
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
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
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

	skySoft_W25QXX_Wait_Busy(drv);					   //�ȴ�д�����
    
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void skySoft_W25QXX_Write_NoCheck(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf){ 			 		 
	uint16_t pageremain;	   
    
	pageremain=256-addr%256; //��ҳʣ����ֽ���		 	    
	if(len<=pageremain)
        pageremain=len;//������256���ֽ�
    
	while(1){	   
		skySoft_W25QXX_Write_Page(drv,addr,pageremain,buf);
		if(len==pageremain)
            break;//д�������
	 	else{ //NumByteToWrite>pageremain
			buf+=pageremain;
			addr+=pageremain;	

			len-=pageremain;//��ȥ�Ѿ�д���˵��ֽ���
			if(len>256)
                pageremain=256; //һ�ο���д��256���ֽ�
			else 
                pageremain=len; 	  //����256���ֽ���
		}
	};	    
} 

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms
void skySoft_W25QXX_Erase_Sector(w25qxx_drv * drv,uint32_t addr){  
    spi_msg msg[2];
    uint8_t txbuf1[1],txbuf2[4];    

 	addr*=4096;
    skySoft_W25QXX_Write_Enable(drv,1);//SET WEL 	 
    skySoft_W25QXX_Wait_Busy(drv);   
    
    txbuf1[0]=W25X_SectorErase;//������������ָ�� 
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
    
    skySoft_W25QXX_Wait_Busy(drv);//�ȴ��������
}  

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
uint8_t W25QXX_BUFFER[4096];		 
void skySoft_W25QXX_Write(w25qxx_drv * drv,uint32_t addr,uint16_t len,uint8_t* buf){ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;
    
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=addr/4096;//������ַ  
	secoff=addr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   

 	if(len<=secremain)
        secremain=len;//������4096���ֽ�
    
	while(1) {	
		skySoft_W25QXX_Read(drv,secpos*4096,4096,W25QXX_BUF);//������������������
		for(i=0;i<secremain;i++){//У������
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
        
		if(i<secremain){//��Ҫ����
			skySoft_W25QXX_Erase_Sector(drv,secpos);//�����������
			for(i=0;i<secremain;i++){	   //����
				W25QXX_BUF[i+secoff]=buf[i];	  
			}
			skySoft_W25QXX_Write_NoCheck(drv,secpos*4096,4096,W25QXX_BUF);//д����������  
		}else{
            skySoft_W25QXX_Write_NoCheck(drv,addr,secremain,buf);//д�Ѿ������˵�,ֱ��д������ʣ������. 
        }
        
		if(len==secremain){//д�������
            break;
		}else{//д��δ����
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	buf+=secremain; //ָ��ƫ��
			addr+=secremain;//д��ַƫ��	   
		   	len-=secremain;	//�ֽ����ݼ�
			if(len>4096)
                secremain=4096;	//��һ����������д����
			else 
                secremain=len;	//��һ����������д����
		}	 
	};	 
}

//��������оƬ		  
//�ȴ�ʱ�䳬��...
void skySoft_W25QXX_Erase_Chip(w25qxx_drv * drv){   
    spi_msg msg[1];
    uint8_t txbuf[2];
    
    skySoft_W25QXX_Write_Enable(drv,1);//SET WEL 
    skySoft_W25QXX_Wait_Busy(drv);   

    txbuf[0]=W25X_ChipErase;//����Ƭ��������  
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);  
    
	skySoft_W25QXX_Wait_Busy(drv); //�ȴ�оƬ��������
}   


/*
//�������ģʽ
void skySoft_W25QXX_PowerDown(w25qxx_drv * drv){ 
    spi_msg msg[1];
    uint8_t txbuf[2];
    
    txbuf[0]=W25X_PowerDown;//���͵������� 
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);      
    drv->delay(3);//�ȴ�TPD  
}   
*/

/*
//����
void skySoft_W25QXX_WAKEUP(w25qxx_drv * drv){  
    spi_msg msg[1];
    uint8_t txbuf[2];
    
    txbuf[0]=W25X_ReleasePowerDown;
    msg[0].len=1;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);  
    drv->delay(3);//�ȴ�TRES1                            
}   
*/

