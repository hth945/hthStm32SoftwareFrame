#ifndef __MYFIFO_H
#define __MYFIFO_H


typedef struct
{  
    unsigned char *buf;  
    unsigned int size;  //缓冲区大小必须为2的整数次幂
    unsigned int in;  
    unsigned int out;  
}MyFIFO_t;  

int myFIFOInit(MyFIFO_t *fifo,unsigned char *buf, int size);
unsigned int myFIFORead(MyFIFO_t *fifo,unsigned char *buf, unsigned int len);
unsigned int myFIFOWrite(MyFIFO_t *fifo,unsigned char *buf, unsigned int len);


#endif

