#ifndef _BUS_H_
#define _BUS_H_

#include <stdio.h>

#define OFFSETOF(TYPE, MEMBER) ((int)(&((TYPE *)0)->MEMBER))  //求偏移地址

//一个简单的hash算法
unsigned int hash_index(unsigned int *data, int len);
#endif

