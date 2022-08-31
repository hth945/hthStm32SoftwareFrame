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

#ifndef _SIM_QSPI_
#define _SIM_QSPI_

#include "sys.h"

typedef struct
{
    void (*set_cs)(uint8_t);   //CS引脚控制函数，参数=0 输出低电平，=1输出高电平
    void (*set_sclk)(uint8_t); //SCL引脚控制函数
    void (*set_io0)(uint8_t);  //数据线输出口控制函数 
    void (*set_io1)(uint8_t);
    void (*set_io2)(uint8_t);
    void (*set_io3)(uint8_t);
    uint8_t (*read_io0)(void); //数据线回读函数
    uint8_t (*read_io1)(void);
    uint8_t (*read_io2)(void);
    uint8_t (*read_io3)(void);
    void (*change_io)(uint8_t); //数据线方向切换，用于读和写时的切换
    void (*delay)(void);        //每个时钟的时间间隔时间
}SIM_QSPI_Adapter;


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
               );

/*
*函数名 : QSPI_1W_WriteByte
*描述   : 单数据线方式，写一个字节数据，不会控制CS
*参数   : 1. *qspi_adapter - 上下文结构体指针
          2. u8 dat   - 要写的一个字节数据

*返回   : 无
*/
void QSPI_1W_WriteByte(SIM_QSPI_Adapter *qspi_adapter, u8 dat);


/*
*函数名 : QSPI_4W_WriteByte
*描述   : 4数据线方式，写一个字节数据，不会控制CS
*参数   : 1. *qspi_adapter - 上下文结构体指针
          2. u8 dat   - 要写的一个字节数据

*返回   : 无
*/
void QSPI_4W_WriteByte(SIM_QSPI_Adapter *qspi_adapter, uint8_t dat);

/*
*函数名 : QSPI_1W_ReadWriteData
*描述   : 写和读同一个函数，先执行写 再执行读。会控制CS，写开始SC=0,写读期间CS一直维持低，读结束CS=1。
*         不需要读的话，读长度参数rd_len设置为0。
*参数   : 1. *qspi_adapter      - 上下文结构体指针
*         2. wr_len             - 写的长度
*         3. uint8_t *p_wr_data - 写的数据
*         4. rd_len             - 读的长度
*         5. p_rd_data          - 读的数据
*返回   : 无
*/
void QSPI_1W_ReadWriteData(SIM_QSPI_Adapter *qspi_adapter, unsigned int wr_len, uint8_t *p_wr_data, 
                                                            unsigned int rd_len, uint8_t *p_rd_data);
/*
*函数名 : QSPI_4W_WriteData
*描述   : 4数据线方式，写一个串数据，会控制CS，写开始SC=0,写期间CS一直维持低，写结束CS=1。
*参数   : 1. *qspi_adapter     - 上下文结构体指针
          2. uint8_t *p_data   - 写的数据
*返回   : 无
*/
void QSPI_4W_WriteData(SIM_QSPI_Adapter *qspi_adapter, uint8_t cmd, unsigned int len, uint8_t *p_data);

#endif
