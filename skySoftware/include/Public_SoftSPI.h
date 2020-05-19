#ifndef _PUBLIC_SOFTSPI_H__
#define	_PUBLIC_SOFTSPI_H__

#include "stdint.h"

typedef enum{
    CPOL_Low   =0,
    CPOL_High  =1,
    CPHA_1Edge =0,
    CPHA_2Edge =1,
    First_MSB =0,
    First_LSB =1,
}spi_mode;

typedef struct{
    void (* mosi_set)(uint8_t);//设置mosi电平
    uint8_t (* miso_read)(void);//读取miso电平
    void (* sck_set)(uint8_t);//设置sck电平
    void (* cs_set)(uint8_t);//设置cs电平
    void (* delay)(void);//延时
    uint8_t (* rwbyte)(uint8_t);//底层通信,可对接硬件spi
    
    spi_mode CPOL;    //时钟默认极性
    spi_mode CPHA;    //第几边缘采样
    spi_mode FirstBit;//首位是高
}spi_adapter;

typedef enum{
    SPI_WR           =0x0000,//双向传输
    SPI_OnlyW        =0x0001,//只发送
    SPI_OnlyR        =0x0002,//只接收
    
    SPI_MidConti     =0x0000,//连续包中间cs使能
    SPI_MidStop      =0x0100,//连续包中间cs不使能
}spi_type;

typedef struct{
    uint16_t type;    
    uint32_t len;
    uint8_t  *txbuf;
    uint8_t  *rxbuf;
}spi_msg;

void P_SPI_DeInit(spi_adapter *adap);
void P_SPI_Init(spi_adapter *adap);


int P_SPI_transfer(spi_adapter *adap,spi_msg *msgs,uint16_t msgs_num);

#endif
