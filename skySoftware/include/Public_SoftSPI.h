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
    void (* mosi_set)(struct _Tspi_adapter *self, uint8_t);//����mosi��ƽ
    uint8_t (* miso_read)(struct _Tspi_adapter *self);//��ȡmiso��ƽ
    void (* sck_set)(struct _Tspi_adapter *self, uint8_t);//����sck��ƽ
    void (* cs_set)(struct _Tspi_adapter *self, uint8_t);//����cs��ƽ
    void (* delay)(struct _Tspi_adapter *self);//��ʱ
    uint8_t (* rwbyte)(struct _Tspi_adapter *self, uint8_t);//�ײ�ͨ��,�ɶԽ�Ӳ��spi.��0���ǵ���Ӳ��SPI
    
    spi_mode CPOL;    //ʱ��Ĭ�ϼ���
    spi_mode CPHA;    //�ڼ���Ե����
    spi_mode FirstBit;//��λ��MSB����LSB Ĭ����MSB
    spi_mode DataSize;//Ĭ����8λģʽ,9λģʽ����dc
}spi_adapter;

typedef enum{
    SPI_WR           =0x0000,//˫����
    SPI_OnlyW        =0x0001,//ֻ����
    SPI_OnlyR        =0x0002,//ֻ����
    
    SPI_MidConti     =0x0000,//�������м�csʹ��
    SPI_MidStop      =0x0100,//�������м�cs��ʹ��
    SPI_ExitConti    =0x0200,//�˳�������csʹ��
}spi_type;

typedef struct{
    uint16_t type;   
    uint32_t len;    //���ݰ��ĳ���
    uint8_t  dc;     //ֻ��9λspi��ʱ��ʹ��
    uint8_t  *txbuf; //ֻ��˫������ֻ���͵�ʱ��ʹ��
    uint8_t  *rxbuf; //ֻ��˫������ֻ���յ�ʱ��ʹ��
}spi_msg;

//Ĭ�ϳ�ʼ�����һЩ��ֵ����ָ�����պ���,��ֹδ�����ܷ�.ʹ��Init֮ǰ�ȵ���
void P_SPI_DeInit(spi_adapter *adap);

//��ʼ��,���������õ����Ӧģʽ�ĵ�ƽ
void P_SPI_Init(spi_adapter *adap);

//���亯��
int P_SPI_transfer(spi_adapter *adap,spi_msg *msgs,uint16_t msgs_num);

#endif
