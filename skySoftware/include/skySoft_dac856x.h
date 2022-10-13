#ifndef _SKYSOFT_DAC856X_H__
#define _SKYSOFT_DAC856X_H__

#include "stdint.h"
#include "Public_SoftSPI.h"


typedef enum{
    write_dac            =0x0,
    update_dac           =0x1,
    write_update_all_dac =0x2,
    write_update_dac     =0x3,
    power_down           =0x4,
    clear_code           =0x5,
    write_ldac           =0x6,
    soft_rst             =0x7,
    inter_ref1           =0x8,
    inter_ref2           =0x9,
}dac856x_mode;

typedef enum{
    channl_A   =0x0,
    channl_B   =0x1,
    channl_C   =0x2,
    channl_D   =0x3,
    channl_E   =0x4,
    channl_F   =0x5,
    channl_G   =0x6,
    channl_H   =0x7,
    channl_all =0xf,
}dac856x_channl;

typedef struct{
    spi_adapter adap;
    void (* delay)(void);//延时
    void (* ldac_set)(uint8_t);//设置LDAC电平
    
}dac856x_drv;


void skySoft_dac856x_DeInit(dac856x_drv * drv);
void skySoft_dac856x_Init(dac856x_drv * drv);
void skySoft_dac856x_set_dac(dac856x_drv * drv,dac856x_channl ch,uint16_t dat);



void skySoft_dac856x_write(dac856x_drv * drv,uint8_t control,uint8_t addr,uint16_t dat,uint8_t feature);
void skySoft_dac856x_Ld(dac856x_drv * drv);


#endif
