/**
 ******************************************************************************
 * @file    myFIFO.c 
 * @author  
 * @version V1.1.0
 * @date    2021.08.10
 * @brief   
 ******************************************************************************
 * @version v1.1.0  2021.08.10
 *          -->��Ӿֲ���������min(x, y)������ʱ����Ϊy�������롣
 *             ����ж�дFIFO����ѯ������FIFO����£�дFIFO�жϴ�϶�FIFO���̣�
 *             ���µ�FIFO->in����ʽ�ı䣬�Ӷ����min(x,y)�е�y�����任���Σ�
 *             �������Ķ�ȡ�ֽڳ���len�����������յ��¶�ȡ���ֽڳ��ȷ����쳣��
 ******************************************************************************  
 */

#include "myFIFO.h"
#include <string.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

/**
 * @brief   FIFO��ʼ��
 * @param   *fifo: FIFO�ṹ
 *          *buf: ���ݻ�����
 *          size: FIFO�ֽڳ���
 * @return  -1: FIFO��ʼ��ʧ��
 *           0: FIFO��ʼ���ɹ�
 * @attention  ��������С����Ϊ2����������
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
 * @brief   ��ȡFIFO
 * @param   *fifo: FIFO�ṹ
 *          *buf: ��FIFO���ݻ�����
 *          len: ��ȡ�ֽڳ���
 * @return  ��ȡ�ֽڳ���
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
 * @brief   д��FIFO
 * @param   *fifo: FIFO�ṹ
 *          *buf: дFIFO���ݻ�����
 *          len: д���ֽڳ���
 * @return  д���ֽڳ���
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
