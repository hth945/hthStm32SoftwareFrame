#ifndef __COMMONIOINTERFACE_H
#define __COMMONIOINTERFACE_H

//#include "systick.h"
#include <stdio.h>
#include "myFIFO.h"
#include <string.h>
//#include "delay.h"
#include "stdlib.h"
#include "stdint.h"

//typedef struct
//{  
//    MyFIFO_t *fifo;
//}sky_comDevice;  

//外部不应该直接调用结构体的函数指针
struct _Tsky_comDriver;
typedef struct _Tsky_comDriver
{
	void *userData;       //bsp层自己使用的指针(可改变类型), 外部应用层尽量不要使用           
	void *Parent;         //一旦有其他结构体包含了 sky_comDriver,就应该给此变量赋值
    MyFIFO_t *fifo;                        //应用层实现的fifo， 有数据了会向fifo中写入数据 不用可以为空 
	
	int (*single)(struct _Tsky_comDriver *self,uint8_t *data, int len);  //接收数据中断   应用层实现此函数 不用可以为空  （中断信号,参数为自身的userData）
	int (*write)(struct _Tsky_comDriver *self,uint8_t *data, int len);  //写数据函数     驱动中实现 
	
	//准备弃用
	//int (*write)(uint8_t *data, int len);  
}sky_comDriver; //驱动使用的句柄，

//应用层使用此函数发送数据，此函数会调用底层的write2
int devWrite(sky_comDriver *dev, uint8_t *data, int len);

//驱动接收中断中调用，把数据写入到dev中。  此函数会触发dev中的single函数，会向fifo写数据
int Write2dev(sky_comDriver *dev, uint8_t *data, int len);

#endif
