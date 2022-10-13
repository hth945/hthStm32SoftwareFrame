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

#ifndef _SIM_QSPI_
#define _SIM_QSPI_

#include "sys.h"

typedef struct
{
    void (*set_cs)(uint8_t);   //CS���ſ��ƺ���������=0 ����͵�ƽ��=1����ߵ�ƽ
    void (*set_sclk)(uint8_t); //SCL���ſ��ƺ���
    void (*set_io0)(uint8_t);  //����������ڿ��ƺ��� 
    void (*set_io1)(uint8_t);
    void (*set_io2)(uint8_t);
    void (*set_io3)(uint8_t);
    uint8_t (*read_io0)(void); //�����߻ض�����
    uint8_t (*read_io1)(void);
    uint8_t (*read_io2)(void);
    uint8_t (*read_io3)(void);
    void (*change_io)(uint8_t); //�����߷����л������ڶ���дʱ���л�
    void (*delay)(void);        //ÿ��ʱ�ӵ�ʱ����ʱ��
}SIM_QSPI_Adapter;


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
               );

/*
*������ : QSPI_1W_WriteByte
*����   : �������߷�ʽ��дһ���ֽ����ݣ��������CS
*����   : 1. *qspi_adapter - �����Ľṹ��ָ��
          2. u8 dat   - Ҫд��һ���ֽ�����

*����   : ��
*/
void QSPI_1W_WriteByte(SIM_QSPI_Adapter *qspi_adapter, u8 dat);


/*
*������ : QSPI_4W_WriteByte
*����   : 4�����߷�ʽ��дһ���ֽ����ݣ��������CS
*����   : 1. *qspi_adapter - �����Ľṹ��ָ��
          2. u8 dat   - Ҫд��һ���ֽ�����

*����   : ��
*/
void QSPI_4W_WriteByte(SIM_QSPI_Adapter *qspi_adapter, uint8_t dat);

/*
*������ : QSPI_1W_ReadWriteData
*����   : д�Ͷ�ͬһ����������ִ��д ��ִ�ж��������CS��д��ʼSC=0,д���ڼ�CSһֱά�ֵͣ�������CS=1��
*         ����Ҫ���Ļ��������Ȳ���rd_len����Ϊ0��
*����   : 1. *qspi_adapter      - �����Ľṹ��ָ��
*         2. wr_len             - д�ĳ���
*         3. uint8_t *p_wr_data - д������
*         4. rd_len             - ���ĳ���
*         5. p_rd_data          - ��������
*����   : ��
*/
void QSPI_1W_ReadWriteData(SIM_QSPI_Adapter *qspi_adapter, unsigned int wr_len, uint8_t *p_wr_data, 
                                                            unsigned int rd_len, uint8_t *p_rd_data);
/*
*������ : QSPI_4W_WriteData
*����   : 4�����߷�ʽ��дһ�������ݣ������CS��д��ʼSC=0,д�ڼ�CSһֱά�ֵͣ�д����CS=1��
*����   : 1. *qspi_adapter     - �����Ľṹ��ָ��
          2. uint8_t *p_data   - д������
*����   : ��
*/
void QSPI_4W_WriteData(SIM_QSPI_Adapter *qspi_adapter, uint8_t cmd, unsigned int len, uint8_t *p_data);

#endif
