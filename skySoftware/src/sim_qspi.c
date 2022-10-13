/********************************* SKYCODE *************************************
* Copyright (c) 2020, 深圳思凯测试技术有限公司
*
* 文件名   ：sim_qspi.c   
* 描述     ：
             1.模拟QSPI接口驱动
*
* 版    本 ：V1.0.0
* 作    者 ：ma_xiaoteng
* 完成日期 ：2020-6-2
* 修改描述 ：          
*******************************************************************************/

#include "sys.h"
#include "sysIO.h"
#include "sim_qspi.h"


/*
*函数名 : QSPI_Init
*描述   : 初始化，参数有点多，
*参数   : 1. *qspi_adapter - 上下文结构体指针，调用函数时不要为它赋值
          2. void (*f_set_cs)(uint8_t),   //函数指针, 指向CS引脚控制函数，参数=0 输出低电平，=1输出高电平
          3. void (*f_set_sclk)(uint8_t), //SCL引脚控制函数
          4. void (*f_set_io0)(uint8_t),  //数据线输出口控制函数 
          5. void (*f_set_io1)(uint8_t),
          6. void (*f_set_io2)(uint8_t),
          7. void (*f_set_io3)(uint8_t),
          8. uint8_t (*f_read_io0)(void), //数据线回读函数
          9. uint8_t (*f_read_io1)(void),
          10. uint8_t (*f_read_io2)(void),
          11. uint8_t (*f_read_io3)(void),
          12. void (*f_change_io)(uint8_t), //数据线方向切换，用于读和写时的切换,参数=1输出， 参数=0输入
          13. void (*f_delay)(void)  //时钟间隔时间，延时函数

*返回   : 无
*/
void QSPI_Init(SIM_QSPI_Adapter *qspi_adapter, 
               void (*f_set_cs)(uint8_t),   //CS引脚控制函数，参数=0 输出低电平，=1输出高电平
               void (*f_set_sclk)(uint8_t), //SCL引脚控制函数
               void (*f_set_io0)(uint8_t),  //数据线输出口控制函数 
               void (*f_set_io1)(uint8_t),
               void (*f_set_io2)(uint8_t),
               void (*f_set_io3)(uint8_t),
               uint8_t (*f_read_io0)(void), //数据线回读函数
               uint8_t (*f_read_io1)(void),
               uint8_t (*f_read_io2)(void),
               uint8_t (*f_read_io3)(void),
               void (*f_change_io)(uint8_t), //数据线方向切换，用于读和写时的切换,参数=1输出， 参数=0输入
               void (*f_delay)(void)  //时钟延时函数
               )
{
    /*屁事没干，就是把参数赋值到结构体*/
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
*函数名 : QSPI_1W_WriteByte
*描述   : 单数据线方式 写一个字节数据，不会控制CS
*参数   : 1. *qspi_adapter - 上下文结构体指针
          2. u8 dat   - 要写的一个字节数据

*返回   : 无
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
*函数名 : QSPI_1W_ReadByte
*描述   : 单数据线方式 读一个字节数据，不会控制CS
*参数   : 1. *qspi_adapter - 上下文结构体指针
*返回   : 1. 回读的值
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
*函数名 : QSPI_1W_ReadWriteData
*描述   : 写和读同一个函数，先执行写 再执行读
*参数   : 1. *qspi_adapter      - 上下文结构体指针
*         2. wr_len             - 写的长度
*         3. uint8_t *p_wr_data - 写的数据
*         4. rd_len             - 读的长度
*         5. p_rd_data          - 读的数据
*返回   : 无
*/
void QSPI_1W_ReadWriteData(SIM_QSPI_Adapter *qspi_adapter, unsigned int wr_len, uint8_t *p_wr_data, 
                                                           unsigned int rd_len, uint8_t *p_rd_data)
{
    unsigned int i;
    
    qspi_adapter->set_cs(0);

    qspi_adapter->change_io(1); //把数据IO设置为输出
    
    for(i=0; i<wr_len; i++)
    {
        QSPI_1W_WriteByte(qspi_adapter, *p_wr_data++);
    }
    
    if(rd_len)
    {
        qspi_adapter->change_io(0); //把数据IO设置为输入
    }
    
    for(i=0; i<rd_len; i++)
    {
        *p_rd_data = QSPI_1W_ReadByte(qspi_adapter);
        p_rd_data++;
    }
        
    qspi_adapter->set_cs(1);
    
    qspi_adapter->change_io(1); //把数据IO设置为输出
    
}

/*
*函数名 : QSPI_1W_WriteByte
*描述   : 单数据线方式，写一个字节数据，不会控制CS
*参数   : 1. *qspi_adapter - 上下文结构体指针
*         2. u8 dat   - 要写的一个字节数据
*
*返回   : 无
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
*函数名 : QSPI_4W_WriteData
*描述   : 4数据线方式，写一个串数据，会控制CS，写开始SC=0,写期间CS一直维持低，写结束CS=1。
*参数   : 1. *qspi_adapter     - 上下文结构体指针
          2. uint8_t *p_data   - 写的数据
*返回   : 无
*/
void QSPI_4W_WriteData(SIM_QSPI_Adapter *qspi_adapter, uint8_t cmd, unsigned int len, uint8_t *p_data)
{
    unsigned int i;
//    uint8_t a;
    
    /*写命令*/    
    QSPI_1W_WriteByte(qspi_adapter, cmd);
    
    /**/
    for(i=0; i<len; i++)
    {        
        QSPI_4W_WriteByte(qspi_adapter, *p_data++); 
    }
}

