#include "myFIFO.h"
#include <string.h>  


#define min(x, y) ((x) < (y) ? (x) : (y)) 


//缓冲区大小必须为2的整数次幂
int myFIFOInit(MyFIFO_t *fifo,unsigned char *buf, int size)
{
	int ret;  
	
	ret = size & (size - 1);  
    if (ret)
        return -1;
	
	fifo->buf = buf;  
    fifo->size = size;  
    fifo->in = fifo->out = 0;  
	memset(fifo->buf, 0, size);
    return 0;  
}

unsigned int myFIFORead(MyFIFO_t *fifo,unsigned char *buf, unsigned int len)  
{
    unsigned int l;  
    len = min(len, fifo->in - fifo->out);  
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));  
    memcpy(buf, fifo->buf + (fifo->out & (fifo->size - 1)), l);  
    memcpy(buf + l, fifo->buf, len - l);  
    fifo->out += len;  
    return len;  
}

unsigned int myFIFOWrite(MyFIFO_t *fifo,unsigned char *buf, unsigned int len)
{
    unsigned int l;  
    len = min(len, fifo->size - fifo->in + fifo->out);  
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));  
    memcpy(fifo->buf + (fifo->in & (fifo->size - 1)), buf, l);  
    memcpy(fifo->buf, buf + l, len - l);  
    fifo->in += len;  
    return len;  
}

