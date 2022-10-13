#include "skySoft_INA226.h"
#include <string.h>


/*
*������ : INA226_Init
*����   : ��ʼ�� INA226 �������򣬳�ʼ����IC���ڸ�λ״̬
*����   :  1. INA226_Drv *ina226_drv
           2. i2c_adapter *i2c_adap  I2C�����ľ��
           3. i2c_addr  �ӻ���ַ
*����   : =0 �ɹ���<0 ʧ��
*/
int skySoft_INA226_Init(INA226_Drv *ina226_drv, i2c_adapter *i2c_adap, u8 i2c_addr)
{
 ina226_drv->i2c_addr = i2c_addr;
 ina226_drv->i2c_adap = *i2c_adap;
 //memcpy(&(ina226_drv->i2c_adap), i2c_adap,sizeof(i2c_adapter));
 return skySoft_INA226_SendData(ina226_drv, INA226_CFG_REG, 0x8000); //��λ
}

/******************************************************************************
*������ : skySoft_INA226_SendData
*����   : 
*����   :  1. INA226_Drv *ina226_drv
           2. 
           3. 
           4. 
*����   : =0 �ɹ���<0 ʧ��
******************************************************************************/
int skySoft_INA226_SendData(INA226_Drv *drv, uint8_t reg, uint16_t data) //д����
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



int skySoft_INA226_ReadData(INA226_Drv *drv,uint8_t reg, uint16_t *rd_data)  //������
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
*������ : skySoft_INA226_SetRshunt
*����   : ���õ��������������ֵ
*����   :  1. INA226_Drv *ina226_drv
           2. r_ohm  ���������������ֵ��λ ŷķ
*����   : =0 �ɹ���<0 ʧ��
*/
int skySoft_INA226_SetRshunt(INA226_Drv *ina226_drv, float r_ohm)
{
    ina226_drv->r_shunt_ohm = r_ohm;
    return 0;
}

/*
*������ : skySoft_INA226_GetIuA
*����   : ��ȡ����ֵ
*����   :  1. INA226_Drv *ina226_drv
           2. ����ֵ ��λ uA
*����   : =0 �ɹ���<0 ʧ��
*/
int skySoft_INA226_GetIuA(INA226_Drv *ina226_drv, float *uA)
{
    int16_t adc;
    int ret;
    float sv;

    ret=skySoft_INA226_ReadData(ina226_drv, INA226_SV_REG, (uint16_t*)(&adc));
    if(ret<0) return ret;
    
    sv = adc;
    sv *= 2.5f; //shunt ��ѹ ��λuV
    *uA = sv / ina226_drv->r_shunt_ohm ;
    
    return ret;
}

/*
*������ : skySoft_INA226_GetIuA
*����   : ��ȡVBUS��ѹֵ
*����   :  1. INA226_Drv *ina226_drv
           2. ��ѹֵ ��λ V
*����   : =0 �ɹ���<0 ʧ��
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
















