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

//�ⲿ��Ӧ��ֱ�ӵ��ýṹ��ĺ���ָ��
struct _Tsky_comDriver;
typedef struct _Tsky_comDriver
{
	void *userData;       //bsp���Լ�ʹ�õ�ָ��(�ɸı�����), �ⲿӦ�ò㾡����Ҫʹ��           
	void *Parent;         //һ���������ṹ������� sky_comDriver,��Ӧ�ø��˱�����ֵ
    MyFIFO_t *fifo;                        //Ӧ�ò�ʵ�ֵ�fifo�� �������˻���fifo��д������ ���ÿ���Ϊ�� 
	
	int (*single)(struct _Tsky_comDriver *self,uint8_t *data, int len);  //���������ж�   Ӧ�ò�ʵ�ִ˺��� ���ÿ���Ϊ��  ���ж��ź�,����Ϊ�����userData��
	int (*write)(struct _Tsky_comDriver *self,uint8_t *data, int len);  //д���ݺ���     ������ʵ�� 
	
	//׼������
	//int (*write)(uint8_t *data, int len);  
}sky_comDriver; //����ʹ�õľ����

//Ӧ�ò�ʹ�ô˺����������ݣ��˺�������õײ��write2
int devWrite(sky_comDriver *dev, uint8_t *data, int len);

//���������ж��е��ã�������д�뵽dev�С�  �˺����ᴥ��dev�е�single����������fifoд����
int Write2dev(sky_comDriver *dev, uint8_t *data, int len);

#endif
