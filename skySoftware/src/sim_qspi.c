/********************************* SKYCODE *************************************
* Copyright (c) 2020, ����˼�����Լ������޹�˾
*
* �ļ���   ��sim_qspi.c   
* ����     ��
             1.ģ��QSPI�ӿ�����
*
* ��    �� ��V1.0.0
* ��    �� ��ma_xiaoteng
* ������� ��2020-6-2
* �޸����� ��          
*******************************************************************************/

#include "sys.h"
#include "sysIO.h"
#include "sim_qspi.h"


/*
*������ : QSPI_Init
*����   : ��ʼ���������е�࣬
*����   : 1. *qspi_adapter - �����Ľṹ��ָ�룬���ú���ʱ��ҪΪ����ֵ
          2. void (*f_set_cs)(uint8_t),   //����ָ��, ָ��CS���ſ��ƺ���������=0 ����͵�ƽ��=1����ߵ�ƽ
          3. void (*f_set_sclk)(uint8_t), //SCL���ſ��ƺ���
          4. void (*f_set_io0)(uint8_t),  //����������ڿ��ƺ��� 
          5. void (*f_set_io1)(uint8_t),
          6. void (*f_set_io2)(uint8_t),
          7. void (*f_set_io3)(uint8_t),
          8. uint8_t (*f_read_io0)(void), //�����߻ض�����
          9. uint8_t (*f_read_io1)(void),
          10. uint8_t (*f_read_io2)(void),
          11. uint8_t (*f_read_io3)(void),
          12. void (*f_change_io)(uint8_t), //�����߷����л������ڶ���дʱ���л�,����=1����� ����=0����
          13. void (*f_delay)(void)  //ʱ�Ӽ��ʱ�䣬��ʱ����

*����   : ��
*/
void QSPI_Init(SIM_QSPI_Adapter *qspi_adapter, 
               void (*f_set_cs)(uint8_t),   //CS���ſ��ƺ���������=0 ����͵�ƽ��=1����ߵ�ƽ
               void (*f_set_sclk)(uint8_t), //SCL���ſ��ƺ���
               void (*f_set_io0)(uint8_t),  //����������ڿ��ƺ��� 
               void (*f_set_io1)(uint8_t),
               void (*f_set_io2)(uint8_t),
               void (*f_set_io3)(uint8_t),
               uint8_t (*f_read_io0)(void), //�����߻ض�����
               uint8_t (*f_read_io1)(void),
               uint8_t (*f_read_io2)(void),
               uint8_t (*f_read_io3)(void),
               void (*f_change_io)(uint8_t), //�����߷����л������ڶ���дʱ���л�,����=1����� ����=0����
               void (*f_delay)(void)  //ʱ����ʱ����
               )
{
    /*ƨ��û�ɣ����ǰѲ�����ֵ���ṹ��*/
    qspi_adapter->set_cs = f_set_cs;
    qspi_adapter->set_sclk = f_set_sclk;
    qspi_adapter->set_io0 = f_set_io0;
    qspi_adapter->set_io1 = f_set_io1;
    qspi_adapter->set_io2 = f_set_io2;
    qspi_adapter->set_io3 = f_set_io3;
    qspi_adapter->read_io0 = f_read_io0;
    qspi_adapter->read_io1 = f_read_io1;
    qspi_adapter->read_io2 = f_read_io2;
    qspi_adapter->read_io3 = f_read_io3;
    qspi_adapter->change_io = f_change_io; 
    qspi_adapter->delay = f_delay;
    
    /**/
    qspi_adapter->change_io(1);
    qspi_adapter->set_cs(1);
    
}

/*
*������ : QSPI_1W_WriteByte
*����   : �������߷�ʽ дһ���ֽ����ݣ��������CS
*����   : 1. *qspi_adapter - �����Ľṹ��ָ��
          2. u8 dat   - Ҫд��һ���ֽ�����

*����   : ��
*/
void QSPI_1W_WriteByte(SIM_QSPI_Adapter *qspi_adapter, u8 dat)
{
    unsigned int i;
    
    for(i=0; i<8; i++)
    {
        if(dat&0x80)
        {
            qspi_adapter->set_io0(1);
        }
        else
        {
            qspi_adapter->set_io0(0);
        }
        dat <<= 1;
        
        qspi_adapter->delay();
        qspi_adapter->set_sclk(1);
        qspi_adapter->delay();
        qspi_adapter->set_sclk(0);
    }
}

/*
*������ : QSPI_1W_ReadByte
*����   : �������߷�ʽ ��һ���ֽ����ݣ��������CS
*����   : 1. *qspi_adapter - �����Ľṹ��ָ��
*����   : 1. �ض���ֵ
*/
uint8_t QSPI_1W_ReadByte(SIM_QSPI_Adapter *qspi_adapter)
{
    unsigned int i;
    uint8_t rd_data;
    
    for(i=0; i<8; i++)
    {        
        qspi_adapter->delay();
        qspi_adapter->set_sclk(1);
        
        rd_data <<= 1;
        rd_data |= qspi_adapter->read_io0();
        
        qspi_adapter->delay();
        qspi_adapter->set_sclk(0);
    }
    
    return rd_data;
}

/*
*������ : QSPI_1W_ReadWriteData
*����   : д�Ͷ�ͬһ����������ִ��д ��ִ�ж�
*����   : 1. *qspi_adapter      - �����Ľṹ��ָ��
*         2. wr_len             - д�ĳ���
*         3. uint8_t *p_wr_data - д������
*         4. rd_len             - ���ĳ���
*         5. p_rd_data          - ��������
*����   : ��
*/
void QSPI_1W_ReadWriteData(SIM_QSPI_Adapter *qspi_adapter, unsigned int wr_len, uint8_t *p_wr_data, 
                                                           unsigned int rd_len, uint8_t *p_rd_data)
{
    unsigned int i;
    
    qspi_adapter->set_cs(0);

    qspi_adapter->change_io(1); //������IO����Ϊ���
    
    for(i=0; i<wr_len; i++)
    {
        QSPI_1W_WriteByte(qspi_adapter, *p_wr_data++);
    }
    
    if(rd_len)
    {
        qspi_adapter->change_io(0); //������IO����Ϊ����
    }
    
    for(i=0; i<rd_len; i++)
    {
        *p_rd_data = QSPI_1W_ReadByte(qspi_adapter);
        p_rd_data++;
    }
        
    qspi_adapter->set_cs(1);
    
    qspi_adapter->change_io(1); //������IO����Ϊ���
    
}

/*
*������ : QSPI_1W_WriteByte
*����   : �������߷�ʽ��дһ���ֽ����ݣ��������CS
*����   : 1. *qspi_adapter - �����Ľṹ��ָ��
*         2. u8 dat   - Ҫд��һ���ֽ�����
*
*����   : ��
*/
void QSPI_4W_WriteByte(SIM_QSPI_Adapter *qspi_adapter, uint8_t dat)
{
    qspi_adapter->set_io3((dat&0x80) ? 1 : 0);
    qspi_adapter->set_io2((dat&0x40) ? 1 : 0);
    qspi_adapter->set_io1((dat&0x20) ? 1 : 0);
    qspi_adapter->set_io0((dat&0x10) ? 1 : 0);
    
    qspi_adapter->delay();
    qspi_adapter->set_sclk(1);
    qspi_adapter->delay();
    qspi_adapter->set_sclk(0);
    
    qspi_adapter->set_io3((dat&0x08) ? 1 : 0);
    qspi_adapter->set_io2((dat&0x04) ? 1 : 0);
    qspi_adapter->set_io1((dat&0x02) ? 1 : 0);
    qspi_adapter->set_io0((dat&0x01) ? 1 : 0);
    
    qspi_adapter->delay();
    qspi_adapter->set_sclk(1);
    qspi_adapter->delay();
    qspi_adapter->set_sclk(0);
}

/*
*������ : QSPI_4W_WriteData
*����   : 4�����߷�ʽ��дһ�������ݣ������CS��д��ʼSC=0,д�ڼ�CSһֱά�ֵͣ�д����CS=1��
*����   : 1. *qspi_adapter     - �����Ľṹ��ָ��
          2. uint8_t *p_data   - д������
*����   : ��
*/
void QSPI_4W_WriteData(SIM_QSPI_Adapter *qspi_adapter, uint8_t cmd, unsigned int len, uint8_t *p_data)
{
    unsigned int i;
//    uint8_t a;
    
    /*д����*/    
    QSPI_1W_WriteByte(qspi_adapter, cmd);
    
    /**/
    for(i=0; i<len; i++)
    {        
        QSPI_4W_WriteByte(qspi_adapter, *p_data++); 
    }
}

