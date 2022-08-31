#ifndef _PUBLIC_I2C_H__
#define	_PUBLIC_I2C_H__

#include "stdint.h"

typedef struct{
    void (* sda_set)(uint8_t);//����sda��ƽ
    uint8_t (* sda_read)(void);//��ȡsda��ƽ
    void (* sda_outmode)(void);//��ȡsda���ģʽ
    void (* sda_inmode)(void);//��ȡsda����ģʽ
    void (* scl_set)(uint8_t);//����scl��ƽ
    void (* delay)(void);//��ʱ
    uint16_t wait_overtime_cnt;//��ack�ȴ�����
}i2c_adapter;

typedef enum{
    //��Щ���ܻ�
    I2C_S_W         =0x0000,//Start+WAddr+WDat
    I2C_W           =0x0001,//WDat

    I2C_S_R		    =0x0100,//Start+RAddr+RDat
    I2C_R           =0x0200,//RDat
    
    //��Щ�ܻ�
    I2C_P           =0x8000,//Stop ��;��Ҫ����ֹͣλ
    
}i2c_type;
//ע:P_I2C_transfer������������Զ����ֹͣλ

typedef struct{
    uint16_t type;
    uint16_t addr;
    uint16_t len;
    uint8_t  *buf;
}i2c_msg;

//�ײ�������� һ�㲻��,���������ʽ��ʱ�������
void P_I2C_Start(i2c_adapter *adap);
void P_I2C_Stop(i2c_adapter *adap);
void P_I2C_SendByte(i2c_adapter *adap,uint8_t dat);
int P_I2C_WaitAck(i2c_adapter *adap);
uint8_t P_I2C_ReadByte(i2c_adapter *adap);
void P_I2C_ReplyAck(i2c_adapter *adap,uint8_t ack);

/*---iic�ӿ�---*/

//����Ĭ��ֵ,P_I2C_Initǰ����
void P_I2C_DeInit(i2c_adapter *adap);

//���ýӿ�
void P_I2C_Init(i2c_adapter *adap);

//����ͨ�ź���
int P_I2C_transfer(i2c_adapter *adap,i2c_msg *msgs,uint16_t msgs_num);

//�ж�������ַ�Ƿ����
int P_I2C_DevExist(i2c_adapter *adap,uint8_t dev_addr);

//���ֽ�д
int P_I2C_WriteReg(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t dat);

//���ֽ�д
int P_I2C_WriteRegBuf(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint16_t len,uint8_t *dat);

//���ֽڶ�
int P_I2C_ReadReg(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t *dat);

//���ֽڶ�
int P_I2C_ReadRegBuf(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint16_t len,uint8_t *dat);

//���ֽ�����д
void P_I2C_WriteRegMask(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t mask,uint8_t dat);

#endif
