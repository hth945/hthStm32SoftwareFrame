
#ifndef _SYS_H_
#define _SYS_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "systick.h"
#include "delay.h"
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


// /* Exported types ------------------------------------------------------------*/
// typedef int32_t  s32;
// typedef int16_t s16;
// typedef int8_t  s8;

// typedef const int32_t sc32;  /*!< Read Only */
// typedef const int16_t sc16;  /*!< Read Only */
// typedef const int8_t sc8;   /*!< Read Only */

// typedef __IO int32_t  vs32;
// typedef __IO int16_t  vs16;
// typedef __IO int8_t   vs8;

// typedef __I int32_t vsc32;  /*!< Read Only */
// typedef __I int16_t vsc16;  /*!< Read Only */
// typedef __I int8_t vsc8;   /*!< Read Only */

// typedef uint32_t  u32;
// typedef uint16_t u16;
// typedef uint8_t  u8;

// typedef const uint32_t uc32;  /*!< Read Only */
// typedef const uint16_t uc16;  /*!< Read Only */
// typedef const uint8_t uc8;   /*!< Read Only */

// typedef __IO uint32_t  vu32;
// typedef __IO uint16_t vu16;
// typedef __IO uint8_t  vu8;

// typedef __I uint32_t vuc32;  /*!< Read Only */
// typedef __I uint16_t vuc16;  /*!< Read Only */
// typedef __I uint8_t vuc8;   /*!< Read Only */

typedef enum {FALSE = 0, TRUE = !FALSE} bool;



/* Exported functions ------------------------------------------------------- */

/*
*函得：Sys_CreateTask
*描述  ：添加需要定r绦械暮
*参数  ：task_num - 任站
         fp       - 函抵羔
         td       - g隔rg，即每隔tdrg 绦幸淮fp函
*返回  ：{用MeasureTimeStart()r到F在rgL度 单位1个SYSTICK (1ms)
*/
void Sys_CreateTask(u8 task_num, void (*fp)(void), unsigned int td);

#endif
