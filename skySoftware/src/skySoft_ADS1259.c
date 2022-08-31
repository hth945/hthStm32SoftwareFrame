#include "skySoft_ADS1259.h"

__weak void skySoft_ADS12xx_Set_void(uint8_t level);
__weak uint8_t skySoft_ADS12xx_Get_void(void);
__weak void skySoft_ADS12xx_Delay_ms(uint16_t us);
void skySoft_ADS12xx_DeInit(ads12xx_drv *drv){
    drv->rst_set=skySoft_ADS12xx_Set_void;
    drv->start_set=skySoft_ADS12xx_Set_void;
    drv->drdy_read=skySoft_ADS12xx_Get_void;
    drv->delayms=skySoft_ADS12xx_Delay_ms;

	drv->config.CONFIG0=0x05;
	drv->config.CONFIG1=Sinc2_Filter;
	drv->config.CONFIG2=ADS1259_Speed10HZ;
	drv->config.OFC0=0x00;
	drv->config.OFC1=0x00;
	drv->config.OFC2=0x00;
	drv->config.FSC0=0x00;
	drv->config.FSC1=0x00;
	drv->config.FSC2=0x40;

    P_SPI_DeInit(&drv->adap);    
}

void skySoft_ADS12xx_Init(ads12xx_drv *drv){
    spi_msg msg[4];
    uint8_t i;
    uint8_t par[4]={0xc5,
                    ADS_CMD_SDATAC,
                    ADS_CMD_SLEEP,
                    ADS_CMD_START
                    };
    
    P_SPI_Init(&drv->adap);
                    
	drv->start_set(1);
	drv->rst_set(0);
	drv->delayms(5);
    drv->rst_set(1);
    drv->delayms(50);
                    
    //����
    for(i=0;i<2;i++){
        msg[i].len=1;
        msg[i].type=SPI_OnlyW|SPI_MidStop;
        msg[i].txbuf=&par[i];    
    }
                    
    //����������Ϣ                   
    skySoft_ADS1259_WrReg(drv,0,sizeof(Init_data),(uint8_t *)&drv->config);
                    

    P_SPI_transfer(&drv->adap,msg,4);    
}

__weak void skySoft_ADS12xx_Set_void(uint8_t level){
}

__weak uint8_t skySoft_ADS12xx_Get_void(void){
    return 0;
}

__weak void skySoft_ADS12xx_Delay_ms(uint16_t us){
}
/*******************************************************************************
*������ : ADS1259_WrReg
*����   : spi��дlen���ֽڡ�дһ�������ȥ
*����   : 
*����   :   
*����   : �ⲿ����
*******************************************************************************/
void skySoft_ADS1259_WrReg(ads12xx_drv *drv,uint8_t addr, uint8_t len, uint8_t* data){
    spi_msg msg[2];
    uint8_t txbuf[2];
    
    txbuf[0]=ADS_CMD_WREG|(addr&0x0f);
    txbuf[1]=len-1; 
    msg[0].len=2;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;
    
    msg[1].len=len;
    msg[1].type=SPI_OnlyW;
    msg[1].txbuf=data;
    P_SPI_transfer(&drv->adap,msg,2);
}

/*******************************************************************************
*������ : ADS1259_RdADC
*����   : ��ȡads1259������adcֵ��ͬʱ����
*����   : 
*����   :   
*����   : �ⲿ����
*******************************************************************************/
int skySoft_ADS1259_RdADC(ads12xx_drv *drv,uint32_t *ADC_dat){
    spi_msg msg[2];
    uint8_t txbuf1[1];
    uint8_t rxbuf2[3];
    uint32_t temp;
    
    if(!drv->drdy_read()){//�͵�ʱ�����Ч����
        txbuf1[0]=ADS_CMD_RDATA; 
        msg[0].len=1;
        msg[0].type=SPI_OnlyW;
        msg[0].txbuf=txbuf1;
        
        msg[1].len=3;
        msg[1].type=SPI_OnlyR;
        msg[1].rxbuf=rxbuf2;
        P_SPI_transfer(&drv->adap,msg,2);

        temp=rxbuf2[0];
        temp<<=8;
        temp|=rxbuf2[1];
        temp<<=8;
        temp|=rxbuf2[2];
        
        *ADC_dat=temp;
        return 0;
    }
    return -1;    
}


