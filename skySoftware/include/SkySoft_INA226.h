#ifndef _SKYSOFT_DAC856X_H__
#define _SKYSOFT_DAC856X_H__



//#include "stm32f4xx.h"
#include "Public_SoftI2C.h"
#include "delay.h"


#define 	INA226_CFG_REG  0x00   //ע������
#define 	INA226_SV_REG   0x01   //�����������ѹ�Ĵ���
#define 	INA226_BV_REG   0x02   //���ߵ�ѹ�Ĵ���
#define 	INA226_PWR_REG  0x03   //���ʼĴ���
#define 	INA226_CUR_REG  0x04   //�����Ĵ���
#define 	INA226_CAL_REG  0x05   //У׼�Ĵ���
#define 	INA226_ONFF_REG 0x06   //���üĴ���
#define 	INA226_AL_REG   0x07   //�������ƼĴ���
#define 	INA226_ID_REG  0XFF   //����Ψһ��оƬ��ʶ��(ID�ļ�)
//#define   	INA226_GETALADDR  0x14 


//Configuration Register (00h) (Read/Write)
#define RST           (1<<15)
#define DEFAULT       (1<<14)
#define AVG_1         (0<<9)//ƽ���������� //��Ĭ��ģʽ��
#define AVG_4         (1<<9)
#define AVG_16        (2<<9)
#define AVG_64        (3<<9)
#define AVG_128       (4<<9)
#define AVG_256       (5<<9)
#define AVG_512       (6<<9)
#define AVG_1024      (7<<9)
#define VBUSCT_140us  (0<<6)//ĸ�ߵ�ѹ����ʱ��
#define VBUSCT_204us  (1<<6)
#define VBUSCT_332us  (2<<6)
#define VBUSCT_588us  (3<<6)
#define VBUSCT_1100us (4<<6)//��Ĭ��ģʽ��
#define VBUSCT_2116us (5<<6)
#define VBUSCT_4156us (6<<6)
#define VBUSCT_8244us (7<<6)
#define VSHCT_140us   (0<<3)//����������ʱ��
#define VSHCT_204us   (1<<3)
#define VSHCT_332us   (2<<3)
#define VSHCT_588us   (3<<3)
#define VSHCT_1100us  (4<<3)//��Ĭ��ģʽ��
#define VSHCT_2116us  (5<<3)
#define VSHCT_4156us  (6<<3)
#define VSHCT_8244us  (7<<3)
#define MODE_Shutdown1  (0<<0)
#define MODE_Shunt_T    (1<<0)//���������ٴ���
#define MODE_Bus_T      (2<<0)//ĸ�ߵ�ѹ���ٴ���
#define MODE_ShuntBus_T (3<<0)//��������ĸ�ߵ�ѹ���ٴ���
#define MODE_Shutdown2  (4<<0)
#define MODE_Shunt_C    (5<<0)//��������������
#define MODE_Bus_C      (6<<0)//ĸ�ߵ�ѹ��������
#define MODE_ShuntBus_C (7<<0)//��������ĸ�ߵ�ѹ�������� ��Ĭ��ģʽ��


typedef struct{
    uint8_t i2c_addr; //i2c �豸��ַ
    i2c_adapter i2c_adap; //i2c ��������
    float r_shunt_ohm; //������������ ��λŷķ
}INA226_Drv;

/*
*������ : INA226_Init
*����   : ��ʼ�� INA226 �������򣬳�ʼ����IC���ڸ�λ״̬
*����   :  1. INA226_Drv *ina226_drv
           2. i2c_adapter *i2c_adap  I2C�����ľ��
           3. i2c_addr  �ӻ���ַ
*����   : =0 �ɹ���<0 ʧ��
*/
int skySoft_INA226_Init(INA226_Drv *ina226_drv, i2c_adapter *i2c_adap, u8 i2c_addr);

/*
*������ : skySoft_INA226_SetRshunt
*����   : ���õ��������������ֵ
*����   :  1. INA226_Drv *ina226_drv
           2. r_ohm  ���������������ֵ��λ ŷķ
*����   : =0 �ɹ���<0 ʧ��
*/
int skySoft_INA226_SetRshunt(INA226_Drv *ina226_drv, float r_ohm);

/*
*������ : skySoft_INA226_GetIuA
*����   : ��ȡ����ֵ
*����   :  1. INA226_Drv *ina226_drv
           2. ����ֵ ��λ uA
*����   : =0 �ɹ���<0 ʧ��
*/
int skySoft_INA226_GetIuA(INA226_Drv *ina226_drv, float *uA);

int skySoft_INA226_SendData(INA226_Drv *ina226_drv, uint8_t reg, uint16_t data); //д����
int skySoft_INA226_ReadData(INA226_Drv *ina226_drv, uint8_t reg, uint16_t *rd_data);  //������


#endif






