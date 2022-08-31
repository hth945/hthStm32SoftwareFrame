//V0.0.4
//1.������9λ����ģʽ
//2.�������˳�������csʹ��
//V0.0.3
//1.�޸���len���Ȳ�����ԭ����uint16�ĳ�uint32_t
//V0.0.2
//1.�����Ӳ��spi�ӿ�
//V0.0.1
//1.����

#include "Public_SoftSPI.h"

__weak uint8_t P_SPI_Void_Read(spi_adapter *adap);
__weak void P_SPI_Void_Set(spi_adapter *adap, uint8_t level);
__weak void P_SPI_Delay(spi_adapter *adap);  

uint8_t P_SPI_ReadWriteByte(spi_adapter *adap,uint8_t dat,uint8_t dc);

void P_SPI_DeInit(spi_adapter *adap){
    adap->mosi_set=P_SPI_Void_Set;
    adap->miso_read=P_SPI_Void_Read;
    adap->sck_set=P_SPI_Void_Set;
    adap->cs_set=P_SPI_Void_Set;
    adap->delay=P_SPI_Delay;
    adap->rwbyte=0;
    
    adap->CPOL=CPOL_Low;
    adap->CPHA=CPHA_2Edge;
    adap->FirstBit=First_MSB;
    adap->DataSize=DataSize_8b;
}

void P_SPI_Init(spi_adapter *adap){
    if(adap->CPOL==CPOL_Low){
        adap->sck_set(adap, 0);
    }else{
        adap->sck_set(adap, 1);
    }
    adap->cs_set(adap, 1);
    adap->mosi_set(adap, 0);
}

__weak void P_SPI_Void_Set(spi_adapter *adap, uint8_t level){
}

__weak uint8_t P_SPI_Void_Read(spi_adapter *adap){
    return 0;
}

__weak void P_SPI_Delay(spi_adapter *adap){
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
    uint8_t dc;
    
    for(i=0;i<msgs_num;i++){;
        len=msgs[i].len;
        txbuf=msgs[i].txbuf;
        rxbuf=msgs[i].rxbuf;
        wr_type=msgs[i].type&0x00ff;
        mid_type=msgs[i].type&0xff00;
        dc=msgs[i].dc;
        
        adap->cs_set(adap,  0);//ѡ���豸
        adap->delay(adap);//������
        
        if(wr_type==SPI_WR){//��дһ��
            for(j=0;j<len;j++){
                rxbuf[j]=P_SPI_ReadWriteByte(adap,txbuf[j],dc);
                adap->delay(adap);//������
            }
        }else if(wr_type==SPI_OnlyW){//ֻд
            for(j=0;j<len;j++){
                P_SPI_ReadWriteByte(adap,txbuf[j],dc);
                adap->delay(adap);//������
            }            
        }else if(wr_type==SPI_OnlyR){//ֻ��
            for(j=0;j<len;j++){
                rxbuf[j]=P_SPI_ReadWriteByte(adap,0xff,dc);
                adap->delay(adap);//������
            }            
        }
        
        if(mid_type==SPI_MidStop){
            adap->cs_set( adap,1);//ʧ���豸
        }else if(mid_type==SPI_MidConti){
            tail_stop=1;
        }else if(mid_type==SPI_ExitConti){
            tail_stop=0;
        }
            
        adap->delay(adap);//������
    }
    
    if(tail_stop){
        adap->cs_set( adap,1);//ʧ���豸
    }
    
    adap->delay(adap);//������
    return 0;
}

//����MSB ����LSB
uint8_t P_SPI_Getmosi_set(spi_adapter *adap,uint8_t dat){
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

uint8_t P_SPI_Getsck_set( spi_adapter *adap,uint8_t index){
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

uint8_t P_SPI_ReadWriteByte0(spi_adapter *adap,uint8_t dat,uint8_t dc);
uint8_t P_SPI_ReadWriteByte1(spi_adapter *adap,uint8_t dat,uint8_t dc);
uint8_t P_SPI_ReadWriteByte(spi_adapter *adap,uint8_t dat,uint8_t dc){
    if(adap->rwbyte!=0){//�����ж��Ƿ���Ӳ��spi
        return adap->rwbyte(adap, dat);
    }
    
    if(adap->CPHA==CPHA_1Edge){
        return P_SPI_ReadWriteByte0(adap,dat,dc);
    }else{
        return P_SPI_ReadWriteByte1(adap,dat,dc);
    }
}

//��һ��ʱ���زɼ�
uint8_t P_SPI_ReadWriteByte0(spi_adapter *adap,uint8_t dat,uint8_t dc){
    uint8_t i;
    if(adap->DataSize==DataSize_9b){//9λ������DC
        adap->mosi_set(adap, dc);//��ǰ��������
        adap->delay(adap);
        adap->sck_set(adap, P_SPI_Getsck_set(adap,0));//��һ��ʱ�ӱ�Ե,��ʱ���ǲɼ�,9λ���ɼ���һλ
        adap->delay(adap);
		adap->sck_set(adap, P_SPI_Getsck_set(adap,1));//�������ݵı�Ե
    }    
    
	for(i=0;i<8;i++){
        adap->mosi_set(adap, P_SPI_Getmosi_set(adap,dat));//��ǰ��������
        adap->delay(adap);
        adap->sck_set(adap, P_SPI_Getsck_set(adap, 0));//��һ��ʱ�ӱ�Ե,��ʱ���ǲɼ�
		P_SPI_DatShift(adap,&dat);
        if(adap->miso_read(adap))dat|=P_SPI_GetMISO_Set(adap);
        adap->delay(adap);
		adap->sck_set(adap, P_SPI_Getsck_set(adap, 1));//�������ݵı�Ե
	}
	return dat;
}

//�ڶ���ʱ���زɼ�
uint8_t P_SPI_ReadWriteByte1(spi_adapter *adap,uint8_t dat,uint8_t dc){
    uint8_t i;
    
    if(adap->DataSize==DataSize_9b){//9λ������DC
        adap->mosi_set(adap, dc);
        adap->sck_set(adap, P_SPI_Getsck_set(adap, 0));//��һ��ʱ�ӱ�Ե,��ʱ���ǽ�������   
        adap->delay(adap);
        adap->sck_set(adap, P_SPI_Getsck_set(adap, 1));//�ڶ���ʱ�ӱ�Ե,�ɼ�����,9λ���ɼ���һλ
        adap->delay(adap);
    }
    
	for(i=0;i<8;i++){
        adap->mosi_set(adap, P_SPI_Getmosi_set(adap, dat));
        adap->sck_set(adap, P_SPI_Getsck_set(adap, 0));//��һ��ʱ�ӱ�Ե,��ʱ���ǽ�������   
		P_SPI_DatShift(adap,&dat);
        adap->delay(adap);
        adap->sck_set(adap, P_SPI_Getsck_set(adap, 1));//�ڶ���ʱ�ӱ�Ե,�ɼ�����
        if(adap->miso_read(adap))dat|=P_SPI_GetMISO_Set(adap);
        adap->delay(adap);
	}
	return dat;
}






