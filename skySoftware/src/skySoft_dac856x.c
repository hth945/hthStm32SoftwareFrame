//V0.0.1

#include "skySoft_dac856x.h"

__weak void skySoft_DAC856X_Delay(void){};
__weak void skySoft_DAC856X_Void_Set(uint8_t level){};
void skySoft_dac856x_DeInit(dac856x_drv * drv){
    P_SPI_DeInit(&drv->adap);
    
    drv->adap.CPOL=CPOL_High;
    drv->adap.CPHA=CPHA_1Edge;
    
    drv->delay=skySoft_DAC856X_Delay;
    drv->ldac_set=skySoft_DAC856X_Void_Set;
}  

void skySoft_dac856x_Init(dac856x_drv * drv){
    P_SPI_Init(&drv->adap);
    drv->ldac_set(0);

	skySoft_dac856x_write(drv,inter_ref2,0x0,0xA000,0); /* Power up internal reference all the time regardless DAC states */
	skySoft_dac856x_write(drv,write_update_dac,channl_all,0,0); 
    skySoft_dac856x_write(drv,clear_code,0,0,3); //取消使用clr脚
    skySoft_dac856x_Ld(drv);    
}  

void skySoft_dac856x_set_dac(dac856x_drv * drv,dac856x_channl ch,uint16_t dat){
    skySoft_dac856x_write(drv,write_update_dac,ch,dat,0);
    skySoft_dac856x_Ld(drv);  
}
    
void skySoft_dac856x_Ld(dac856x_drv * drv){
    drv->delay();
    drv->ldac_set(0);
	drv->delay();
	drv->ldac_set(1);
}

void skySoft_dac856x_write(dac856x_drv * drv,uint8_t control,uint8_t addr,uint16_t dat,uint8_t feature){	
    spi_msg msg[1];
    uint8_t txbuf[4];
    
    txbuf[0]=control&0xf;
    txbuf[1]=addr<<4|dat>>12;
    txbuf[2]=0xff & (dat>>4);
    txbuf[3]=(0xf & dat)<<4 | (feature&0xf);
    msg[0].len=4;
    msg[0].type=SPI_OnlyW;
    msg[0].txbuf=txbuf;    
    
    P_SPI_transfer(&drv->adap,msg,1);  
}

