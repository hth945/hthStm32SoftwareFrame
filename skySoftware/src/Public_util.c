#include "Public_util.h"

//一个简单的hash算法
unsigned int hash_index(unsigned int *data, int len)
{
	unsigned int h = 0;
	int i;
	for (i = 0; i < len; i++)
	{
		h = h *31 + data[i];
	}
	return h;
}

