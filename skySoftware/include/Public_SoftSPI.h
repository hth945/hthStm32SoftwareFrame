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
    DataSize_8b=8,
    DataSize_9b=9,
}spi_mode;

struct _Tspi_adapter;
typedef struct _Tspi_adapter{
	void *userData;
    void (* mosi_set)(struct _Tspi_adapter *self, uint8_t);//设置mosi电平
    uint8_t (* miso_read)(struct _Tspi_adapter *self);//读取miso电平
    void (* sck_set)(struct _Tspi_adapter *self, uint8_t);//设置sck电平
    void (* cs_set)(struct _Tspi_adapter *self, uint8_t);//设置cs电平
    void (* delay)(struct _Tspi_adapter *self);//延时
    uint8_t (* rwbyte)(struct _Tspi_adapter *self, uint8_t);//底层通信,可对接硬件spi.非0既是调用硬件SPI
    
    spi_mode CPOL;    //时钟默认极性
    spi_mode CPHA;    //第几边缘采样
    spi_mode FirstBit;//首位是MSB或者LSB 默认是MSB
    spi_mode DataSize;//默认是8位模式,9位模式启用dc
}spi_adapter;

typedef enum{
    SPI_WR           =0x0000,//双向传输
    SPI_OnlyW        =0x0001,//只发送
    SPI_OnlyR        =0x0002,//只接收
    
    SPI_MidConti     =0x0000,//连续包中间cs使能
    SPI_MidStop      =0x0100,//连续包中间cs不使能
    SPI_ExitConti    =0x0200,//退出包继续cs使能
}spi_type;

typedef struct{
    uint16_t type;   
    uint32_t len;    //数据包的长度
    uint8_t  dc;     //只有9位spi的时候使用
    uint8_t  *txbuf; //只有双向传输与只发送的时候使用
    uint8_t  *rxbuf; //只有双向传输与只接收的时候使用
}spi_msg;

//默认初始化会给一些初值函数指针会给空函数,防止未定义跑飞.使用Init之前先调用
void P_SPI_DeInit(spi_adapter *adap);

//初始化,把引脚设置到相对应模式的电平
void P_SPI_Init(spi_adapter *adap);

//传输函数
int P_SPI_transfer(spi_adapter *adap,spi_msg *msgs,uint16_t msgs_num);

#endif
