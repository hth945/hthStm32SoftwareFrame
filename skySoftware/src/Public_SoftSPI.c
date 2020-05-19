//V0.0.3
//1.修改了len长度不足由原来的uint16改成uint32_t
//V0.0.2
//1.添加了硬件spi接口
//V0.0.1
//1.建立

#include "Public_SoftSPI.h"

__weak uint8_t P_SPI_Void_Read(void);
__weak void P_SPI_Void_Set(uint8_t level);
__weak void P_SPI_Delay(void);  

uint8_t P_SPI_ReadWriteByte(spi_adapter *adap,uint8_t dat);

void P_SPI_DeInit(spi_adapter *adap){
    adap->mosi_set=P_SPI_Void_Set;
    adap->miso_read=P_SPI_Void_Read;
    adap->sck_set=P_SPI_Void_Set;
    adap->cs_set=P_SPI_Void_Set;
    adap->delay=P_SPI_Delay;

    adap->CPOL=CPOL_Low;
    adap->CPHA=CPHA_2Edge;
    
    adap->rwbyte=0;
}

void P_SPI_Init(spi_adapter *adap){
    if(adap->CPOL==CPOL_Low){
        adap->sck_set(0);
    }else{
        adap->sck_set(1);
    }
    adap->cs_set(1);
    adap->mosi_set(0);
}

__weak void P_SPI_Void_Set(uint8_t level){
}

__weak uint8_t P_SPI_Void_Read(){
    return 0;
}

__weak void P_SPI_Delay(){
    uint8_t i;
    for(i=0;i<168;i++);
}

int P_SPI_transfer(spi_adapter *adap,spi_msg *msgs,uint16_t msgs_num){
    uint16_t i;
    uint32_t j;
    uint32_t len=0;
    uint8_t *txbuf,*rxbuf;
    uint8_t tail_stop=0;
    uint16_t wr_type;
    uint16_t mid_type;
    
    for(i=0;i<msgs_num;i++){;
        len=msgs[i].len;
        txbuf=msgs[i].txbuf;
        rxbuf=msgs[i].rxbuf;
        wr_type=msgs[i].type&0x00ff;
        mid_type=msgs[i].type&0xff00;
        
        adap->cs_set(0);//选中设备
        adap->delay();//插入间隔
        
        if(wr_type==SPI_WR){
            for(j=0;j<len;j++){
                rxbuf[j]=P_SPI_ReadWriteByte(adap,txbuf[j]);
                adap->delay();//插入间隔
            }
        }else if(wr_type==SPI_OnlyW){
            for(j=0;j<len;j++){
                P_SPI_ReadWriteByte(adap,txbuf[j]);
                adap->delay();//插入间隔
            }            
        }else if(wr_type==SPI_OnlyR){
            for(j=0;j<len;j++){
                rxbuf[j]=P_SPI_ReadWriteByte(adap,0xff);
                adap->delay();//插入间隔
            }            
        }
        
        if(mid_type==SPI_MidStop){
            adap->cs_set(1);//失能设备
        }else if(mid_type==SPI_MidConti){
            tail_stop=1;
        }
            
        adap->delay();//插入间隔
    }
    
    if(tail_stop){
        adap->cs_set(1);//失能设备
    }
    
    adap->delay();//插入间隔
    return 0;
}

uint8_t P_SPI_GetMOSI_Set(spi_adapter *adap,uint8_t dat){
    if(adap->FirstBit==First_MSB){
        return dat&0x80?1:0;
    }else{
        return dat&0x01?1:0;
    }
}

uint8_t P_SPI_GetMISO_Set(spi_adapter *adap){
    if(adap->FirstBit==First_MSB){
        return 0x01;
    }else{
        return 0x80;
    }
}

uint8_t P_SPI_GetSCK_Set(spi_adapter *adap,uint8_t index){
    if(adap->CPOL==CPOL_Low){
        if(index==0){
            return 1;
        }else{
            return 0;
        }
    }else{
        if(index==0){
            return 0;
        }else{
            return 1;
        }
    }
}

void P_SPI_DatShift(spi_adapter *adap,uint8_t *dat){
    if(adap->FirstBit==First_MSB){
        *dat<<=1;
    }else{
        *dat>>=1;
    }
}

uint8_t P_SPI_ReadWriteByte0(spi_adapter *adap,uint8_t dat);
uint8_t P_SPI_ReadWriteByte1(spi_adapter *adap,uint8_t dat);
uint8_t P_SPI_ReadWriteByte(spi_adapter *adap,uint8_t dat){
    if(adap->rwbyte!=0){//这里判断是否用硬件spi
        return adap->rwbyte(dat);
    }
    
    if(adap->CPHA==CPHA_1Edge){
        return P_SPI_ReadWriteByte0(adap,dat);
    }else{
        return P_SPI_ReadWriteByte1(adap,dat);
    }
}

//第一个时钟沿采集
uint8_t P_SPI_ReadWriteByte0(spi_adapter *adap,uint8_t dat){
    uint8_t i;
	for(i=0;i<8;i++){
        adap->mosi_set(P_SPI_GetMOSI_Set(adap,dat));
        adap->delay();
        adap->sck_set(P_SPI_GetSCK_Set(adap,0));
		P_SPI_DatShift(adap,&dat);
        if(adap->miso_read())dat|=P_SPI_GetMISO_Set(adap);
        adap->delay();
		adap->sck_set(P_SPI_GetSCK_Set(adap,1));
	}
	return dat;
}

//第二个时钟沿采集
uint8_t P_SPI_ReadWriteByte1(spi_adapter *adap,uint8_t dat){
    uint8_t i;
	for(i=0;i<8;i++){
        adap->mosi_set(P_SPI_GetMOSI_Set(adap,dat));
        adap->sck_set(P_SPI_GetSCK_Set(adap,0));        
		P_SPI_DatShift(adap,&dat);
        adap->delay();
        adap->sck_set(P_SPI_GetSCK_Set(adap,1));
        if(adap->miso_read())dat|=P_SPI_GetMISO_Set(adap);
        adap->delay();
	}
	return dat;
}






