#include "skySoft_INA226.h"
#include <string.h>


/*
*函数名 : INA226_Init
*描述   : 初始化 INA226 驱动程序，初始化后IC处于复位状态
*参数   :  1. INA226_Drv *ina226_drv
           2. i2c_adapter *i2c_adap  I2C驱动的句柄
           3. i2c_addr  从机地址
*返回   : =0 成功，<0 失败
*/
int skySoft_INA226_Init(INA226_Drv *ina226_drv, i2c_adapter *i2c_adap, u8 i2c_addr)
{
 ina226_drv->i2c_addr = i2c_addr;
 ina226_drv->i2c_adap = *i2c_adap;
 //memcpy(&(ina226_drv->i2c_adap), i2c_adap,sizeof(i2c_adapter));
 return skySoft_INA226_SendData(ina226_drv, INA226_CFG_REG, 0x8000); //复位
}

/******************************************************************************
*函数名 : skySoft_INA226_SendData
*描述   : 
*参数   :  1. INA226_Drv *ina226_drv
           2. 
           3. 
           4. 
*返回   : =0 成功，<0 失败
******************************************************************************/
int skySoft_INA226_SendData(INA226_Drv *drv, uint8_t reg, uint16_t data) //写数据
{
    i2c_msg msg[1];
    uint8_t buf1[3];
    
    buf1[0] = reg;
    buf1[1] = (uint8_t)(data>>8);
    buf1[2] = (uint8_t)(data&0x00FF);
    msg[0].type=I2C_S_W;
    msg[0].addr=drv->i2c_addr;
    msg[0].len=3;
    msg[0].buf=buf1;

    return P_I2C_transfer(&drv->i2c_adap,msg,1);  
}



int skySoft_INA226_ReadData(INA226_Drv *drv,uint8_t reg, uint16_t *rd_data)  //读数据
{ 
    int ret;
    i2c_msg msg[2];
    uint8_t buf1[1],buf2[2];
    
    buf1[0]=reg;
    msg[0].type=I2C_S_W;
    msg[0].addr= drv->i2c_addr;
    msg[0].len = 1;
    msg[0].buf = buf1;
    
    msg[1].type  = I2C_S_R;
    msg[1].addr =  drv->i2c_addr;
    msg[1].len = 2;
    msg[1].buf = buf2;
    ret = P_I2C_transfer(&drv-> i2c_adap,msg,2);
    *rd_data = buf2[1]|(buf2[0]<<8);
    return ret;
}   

/*
*函数名 : skySoft_INA226_SetRshunt
*描述   : 设置电流采样电阻的阻值
*参数   :  1. INA226_Drv *ina226_drv
           2. r_ohm  电流采样电阻的阻值单位 欧姆
*返回   : =0 成功，<0 失败
*/
int skySoft_INA226_SetRshunt(INA226_Drv *ina226_drv, float r_ohm)
{
    ina226_drv->r_shunt_ohm = r_ohm;
    return 0;
}

/*
*函数名 : skySoft_INA226_GetIuA
*描述   : 读取电流值
*参数   :  1. INA226_Drv *ina226_drv
           2. 电流值 单位 uA
*返回   : =0 成功，<0 失败
*/
int skySoft_INA226_GetIuA(INA226_Drv *ina226_drv, float *uA)
{
    int16_t adc;
    int ret;
    float sv;

    ret=skySoft_INA226_ReadData(ina226_drv, INA226_SV_REG, (uint16_t*)(&adc));
    if(ret<0) return ret;
    
    sv = adc;
    sv *= 2.5f; //shunt 电压 单位uV
    *uA = sv / ina226_drv->r_shunt_ohm ;
    
    return ret;
}

/*
*函数名 : skySoft_INA226_GetIuA
*描述   : 读取VBUS电压值
*参数   :  1. INA226_Drv *ina226_drv
           2. 电压值 单位 V
*返回   : =0 成功，<0 失败
*/
int skySoft_INA226_GetVol(INA226_Drv *ina226_drv, float *vol)
{
    int16_t adc;
    int ret;
    float v;
    
    ret=skySoft_INA226_ReadData(ina226_drv, INA226_BV_REG, (uint16_t*)(&adc));
    if(ret<0) return ret;
    
    v=adc;
    *vol = v*0.00125f;
    
    return 0;
}
















