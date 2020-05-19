#ifndef _PUBLIC_I2C_H__
#define	_PUBLIC_I2C_H__

#include "stdint.h"

typedef struct{
    void (* sda_set)(uint8_t);//设置sda电平
    uint8_t (* sda_read)(void);//读取sda电平
    void (* sda_outmode)(void);//读取sda输出模式
    void (* sda_inmode)(void);//读取sda输人模式
    void (* scl_set)(uint8_t);//设置scl电平
    void (* delay)(void);//延时
    uint16_t wait_overtime_cnt;//读ack等待计数
}i2c_adapter;

typedef enum{
    //这些不能或
    I2C_S_W         =0x0000,//Start+WAddr+WDat
    I2C_W           =0x0001,//WDat

    I2C_S_R		    =0x0100,//Start+RAddr+RDat
    I2C_R           =0x0200,//RDat
    
    //这些能或
    I2C_P           =0x8000,//Stop 中途需要发送停止位
    
}i2c_type;
//注:P_I2C_transfer函数结束会自动添加停止位

typedef struct{
    uint16_t type;
    uint16_t addr;
    uint16_t len;
    uint8_t  *buf;
}i2c_msg;

//iic接口
void P_I2C_DeInit(i2c_adapter *adap);
void P_I2C_Init(i2c_adapter *adap);
int P_I2C_transfer(i2c_adapter *adap,i2c_msg *msgs,uint16_t msgs_num);


#endif
