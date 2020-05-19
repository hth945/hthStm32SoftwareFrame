#ifndef _SKYSOFT_ADS1259_H__
#define	_SKYSOFT_ADS1259_H__

#include "stdint.h"
#include "Public_SoftSPI.h"

//COMMAND DEFINITIONS
#define ADS_CMD_SLEEP  0x04
#define ADS_CMD_START  0x08
#define ADS_CMD_WREG   0x40
#define ADS_CMD_SDATAC 0x11
#define ADS_CMD_RDATA  0x12


typedef enum {
    ADS1259_Speed10HZ    = 0x00,
    ADS1259_Speed16_6HZ  = 0x01,
    ADS1259_Speed50HZ    = 0x02,
    ADS1259_Speed60HZ    = 0x03,
    ADS1259_Speed400HZ   = 0x04,
    ADS1259_Speed1200HZ  = 0x05,
    ADS1259_Speed3600HZ  = 0x06,
    ADS1259_Speed14400HZ = 0x07,
}ADS1259_Speed;

typedef enum {
	Sinc1_Filter=0x00,
	Sinc2_Filter=0x10,
}Digital_Filter_Mode;

typedef struct {
	uint8_t             CONFIG0;
	Digital_Filter_Mode CONFIG1;
	ADS1259_Speed       CONFIG2;
	uint8_t OFC0;
	uint8_t OFC1;
	uint8_t OFC2;
	uint8_t FSC0;
	uint8_t FSC1;
	uint8_t FSC2;
}Init_data;

typedef struct{
    spi_adapter adap;
    void (* rst_set)(uint8_t);//设置rst电平
    void (* start_set)(uint8_t);//设置start电平
    uint8_t (* drdy_read)(void);//读取drdy电平
    void (* delayms)(uint16_t);//微秒延时
    
    Init_data config;//配置信息
}ads12xx_drv;

void skySoft_ADS12xx_DeInit(ads12xx_drv *drv);
void skySoft_ADS12xx_Init(ads12xx_drv *drv);
void skySoft_ADS1259_WrReg(ads12xx_drv *drv,uint8_t addr, uint8_t len, uint8_t* data);
int skySoft_ADS1259_RdADC(ads12xx_drv *drv,uint32_t *ADC_dat);



#endif


