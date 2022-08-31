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
//注:P_I2C_transfer函数结束后会自动添加停止位

typedef struct{
    uint16_t type;
    uint16_t addr;
    uint16_t len;
    uint8_t  *buf;
}i2c_msg;

//底层基础函数 一般不用,遇到奇葩格式的时候可以用
void P_I2C_Start(i2c_adapter *adap);
void P_I2C_Stop(i2c_adapter *adap);
void P_I2C_SendByte(i2c_adapter *adap,uint8_t dat);
int P_I2C_WaitAck(i2c_adapter *adap);
uint8_t P_I2C_ReadByte(i2c_adapter *adap);
void P_I2C_ReplyAck(i2c_adapter *adap,uint8_t ack);

/*---iic接口---*/

//配置默认值,P_I2C_Init前调用
void P_I2C_DeInit(i2c_adapter *adap);

//配置接口
void P_I2C_Init(i2c_adapter *adap);

//基本通信函数
int P_I2C_transfer(i2c_adapter *adap,i2c_msg *msgs,uint16_t msgs_num);

//判断器件地址是否存在
int P_I2C_DevExist(i2c_adapter *adap,uint8_t dev_addr);

//单字节写
int P_I2C_WriteReg(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t dat);

//多字节写
int P_I2C_WriteRegBuf(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint16_t len,uint8_t *dat);

//单字节读
int P_I2C_ReadReg(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t *dat);

//多字节读
int P_I2C_ReadRegBuf(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint16_t len,uint8_t *dat);

//单字节掩码写
void P_I2C_WriteRegMask(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t mask,uint8_t dat);

#endif
