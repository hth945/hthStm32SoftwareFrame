/**
 ******************************************************************************
 * @file    myFIFO.c 
 * @author  
 * @version V1.1.0
 * @date    2021.08.10
 * @brief   
 ******************************************************************************
 * @version v1.1.0  2021.08.10
 *          -->添加局部变量，当min(x, y)被引用时，作为y参数传入。
 *             解决中断写FIFO，轮询连续读FIFO情况下，写FIFO中断打断读FIFO过程，
 *             导致的FIFO->in被隐式改变，从而造成min(x,y)中的y参数变换两次，
 *             求解出来的读取字节长度len发生错误。最终导致读取的字节长度发生异常。
 ******************************************************************************  
 */

#include "myFIFO.h"
#include <string.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

/**
 * @brief   FIFO初始化
 * @param   *fifo: FIFO结构
 *          *buf: 备份缓冲区
 *          size: FIFO字节长度
 * @return  -1: FIFO初始化失败
 *           0: FIFO初始化成功
 * @attention  缓冲区大小必须为2的整数次幂
 */
int myFIFOInit(MyFIFO_t *fifo, unsigned char *buf, int size)
{
    int ret;

    ret = size & (size - 1);
    if (ret) {
        return -1;
	}
    fifo->buf = buf;
    fifo->size = size;
    fifo->in = fifo->out = 0;
    memset(fifo->buf, 0, size);
	
    return 0;
}

/**
 * @brief   读取FIFO
 * @param   *fifo: FIFO结构
 *          *buf: 读FIFO内容缓冲区
 *          len: 读取字节长度
 * @return  读取字节长度
 */
unsigned int myFIFORead(MyFIFO_t *fifo, unsigned char *buf, unsigned int len)
{
    unsigned int l;
	int fifo_remain_len;
	int fifo_circle_len;

	fifo_remain_len = fifo->in - fifo->out;
	len = min(len, fifo_remain_len);
	fifo_circle_len = fifo->size - (fifo->out & (fifo->size - 1));
    l = min(len, fifo_circle_len);
    memcpy(buf, fifo->buf + (fifo->out & (fifo->size - 1)), l);
    memcpy(buf + l, fifo->buf, len - l);
    fifo->out += len;

    return len;
}

/**
 * @brief   写入FIFO
 * @param   *fifo: FIFO结构
 *          *buf: 写FIFO内容缓冲区
 *          len: 写入字节长度
 * @return  写入字节长度
 */
unsigned int myFIFOWrite(MyFIFO_t *fifo, unsigned char *buf, unsigned int len)
{
    unsigned int l;
	int fifo_remain_len;
    int fifo_circle_len;
	
	fifo_remain_len = fifo->size - fifo->in + fifo->out;
    len = min(len, fifo_remain_len);
	fifo_circle_len = fifo->size - (fifo->in & (fifo->size - 1));
    l = min(len, fifo_circle_len);
    memcpy(fifo->buf + (fifo->in & (fifo->size - 1)), buf, l);
    memcpy(fifo->buf, buf + l, len - l);
    fifo->in += len;
	
    return len;
}
