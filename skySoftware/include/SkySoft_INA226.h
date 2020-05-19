#ifndef _SKYSOFT_DAC856X_H__
#define _SKYSOFT_DAC856X_H__



#include "stm32f4xx.h"
#include "Public_SoftI2C.h"
#include "delay.h"


#define 	INA226_CFG_REG  0x00   //注册配置
#define 	INA226_SV_REG   0x01   //分流器电阻电压寄存器
#define 	INA226_BV_REG   0x02   //总线电压寄存器
#define 	INA226_PWR_REG  0x03   //功率寄存器
#define 	INA226_CUR_REG  0x04   //电流寄存器
#define 	INA226_CAL_REG  0x05   //校准寄存器
#define 	INA226_ONFF_REG 0x06   //启用寄存器
#define 	INA226_AL_REG   0x07   //警报限制寄存器
#define 	INA226_ID_REG  0XFF   //包含唯一的芯片标识号(ID文件)
//#define   	INA226_GETALADDR  0x14 


//Configuration Register (00h) (Read/Write)
#define RST           (1<<15)
#define DEFAULT       (1<<14)
#define AVG_1         (0<<9)//平均采样次数 //（默认模式）
#define AVG_4         (1<<9)
#define AVG_16        (2<<9)
#define AVG_64        (3<<9)
#define AVG_128       (4<<9)
#define AVG_256       (5<<9)
#define AVG_512       (6<<9)
#define AVG_1024      (7<<9)
#define VBUSCT_140us  (0<<6)//母线电压采样时间
#define VBUSCT_204us  (1<<6)
#define VBUSCT_332us  (2<<6)
#define VBUSCT_588us  (3<<6)
#define VBUSCT_1100us (4<<6)//（默认模式）
#define VBUSCT_2116us (5<<6)
#define VBUSCT_4156us (6<<6)
#define VBUSCT_8244us (7<<6)
#define VSHCT_140us   (0<<3)//分流器采样时间
#define VSHCT_204us   (1<<3)
#define VSHCT_332us   (2<<3)
#define VSHCT_588us   (3<<3)
#define VSHCT_1100us  (4<<3)//（默认模式）
#define VSHCT_2116us  (5<<3)
#define VSHCT_4156us  (6<<3)
#define VSHCT_8244us  (7<<3)
#define MODE_Shutdown1  (0<<0)
#define MODE_Shunt_T    (1<<0)//分流器跟踪触发
#define MODE_Bus_T      (2<<0)//母线电压跟踪触发
#define MODE_ShuntBus_T (3<<0)//分流器与母线电压跟踪触发
#define MODE_Shutdown2  (4<<0)
#define MODE_Shunt_C    (5<<0)//分流器连续触发
#define MODE_Bus_C      (6<<0)//母线电压连续触发
#define MODE_ShuntBus_C (7<<0)//分流器与母线电压连续触发 （默认模式）


typedef struct{
    uint8_t i2c_addr; //i2c 设备地址
    i2c_adapter i2c_adap; //i2c 驱动配置
    float r_shunt_ohm; //电流采样电阻 单位欧姆
}INA226_Drv;

/*
*函数名 : INA226_Init
*描述   : 初始化 INA226 驱动程序，初始化后IC处于复位状态
*参数   :  1. INA226_Drv *ina226_drv
           2. i2c_adapter *i2c_adap  I2C驱动的句柄
           3. i2c_addr  从机地址
*返回   : =0 成功，<0 失败
*/
int skySoft_INA226_Init(INA226_Drv *ina226_drv, i2c_adapter *i2c_adap, u8 i2c_addr);

/*
*函数名 : skySoft_INA226_SetRshunt
*描述   : 设置电流采样电阻的阻值
*参数   :  1. INA226_Drv *ina226_drv
           2. r_ohm  电流采样电阻的阻值单位 欧姆
*返回   : =0 成功，<0 失败
*/
int skySoft_INA226_SetRshunt(INA226_Drv *ina226_drv, float r_ohm);

/*
*函数名 : skySoft_INA226_GetIuA
*描述   : 读取电流值
*参数   :  1. INA226_Drv *ina226_drv
           2. 电流值 单位 uA
*返回   : =0 成功，<0 失败
*/
int skySoft_INA226_GetIuA(INA226_Drv *ina226_drv, float *uA);

int skySoft_INA226_SendData(INA226_Drv *ina226_drv, uint8_t reg, uint16_t data); //写数据
int skySoft_INA226_ReadData(INA226_Drv *ina226_drv, uint8_t reg, uint16_t *rd_data);  //读数据


#endif






