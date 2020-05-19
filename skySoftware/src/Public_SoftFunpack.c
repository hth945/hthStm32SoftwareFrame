#include "Public_SoftFunpack.h"
#include "string.h"

#if               /* ARMCC */ (  (defined ( __CC_ARM ) && defined ( __TARGET_FPU_VFP ))    \
                  /* Clang */ || (defined ( __CLANG_ARM ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) \
                  /* IAR */   || (defined ( __ICCARM__ ) && defined ( __ARMVFP__ ))        \
                  /* GNU */   || (defined ( __GNUC__ ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) )
#define USE_FPU   1
#else
#define USE_FPU   0
#endif


//int asm_runF(void *fun,uint32_t *stack, int len, uint32_t r[4]);
int asm_runF(void *fun,uint32_t *stack, int len, uint32_t rs[4+16]);

int P_doPack(uint8_t *funName,uint8_t *data, funpack_parameter *pt, int len)
{
	int i,n,tem;
	int dataLen = 0; //结构体数据段的偏移
	int NameLen;
	int offset = 0; //顺序存储时的偏移
	
	NameLen = strlen((char *)funName);
	memcpy(data, funName, NameLen + 1);
	offset = NameLen+1;
	
	data[offset] = len;
	offset++;
	
	for (i = 0; i < len; i++)
	{
		data[offset] = pt[i].type;
		offset++;
	}
	
	/***********之后的数据要保证4字节对齐，通信的***************/
	n = (offset) % 4;
	offset += (4-n)%4;
	
	dataLen = offset + len * 4;
	for (i = 0; i < len; i++)
	{
		if ((pt[i].type == 1) || (pt[i].type == 4))
		{
			memcpy(&data[offset],&pt[i].value,4);  //这里data不一定4字节对齐
			offset += 4;
		}else if (pt[i].type == 2)
		{
			memcpy(&data[offset],&dataLen,4);  //数据相对于data的偏移
			offset += 4;
			memcpy(&data[dataLen],pt[i].data,pt[i].len);  //具体数据
			
			/*如果pt[i].len 没有4字节对齐 需要补齐*/
			dataLen += pt[i].len;  
			tem = (pt[i].len) % 4;
			dataLen += (4-tem)%4;
			
		}else if (pt[i].type == 3)  //只输出  输出缓冲区大小 
		{
			memcpy(&data[offset],&pt[i].len,4);  //接收时的数据长度
			offset += 4;
		}else
		{
			return -1;
		}
	}
	return dataLen;
}



//返回 输出长度有多少
int P_unPack(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen)
{
	int i,len;
	int outDataLen = 4;
	int n;
	int NameLen;
	int re;
	void *p;	
	int offset = 0; //顺序存储时的偏移
	
	
	uint32_t par[10];
	
	if ((((uint32_t)data)%4) != 0)
		while(1);
	
	NameLen = strlen((char *)data);
	len = data[NameLen + 1];
	i = 0;
	while(1)
	{
		if (fn[i].func == 0)
		{
			return -1;
		}
		
		if (strcmp((char *)fn[i].name,(char *)data) == 0)
		{
			p = fn[i].func;
			break;
		}
		i++;
	}
	
	
	/***********之后的数据要保证4字节对齐，通信的***************/
	offset = NameLen+1+1+len; //字符串长度 + '\0' + [len]
	n = (offset) % 4;
	offset += (4-n)%4;

	
	for (i = 0; i < len; i++)
	{
		if (data[i+NameLen+2] == 1)
		{
//			memcpy(&prageme[i], &data[len + n + i * 4],4);
			par[i] = *((uint32_t *)&data[offset + i * 4]);  //这里data一定4字节对齐
		}else if (data[i+NameLen+2] == 2)
		{
			par[i] = *((uint32_t *)&data[offset + i * 4]) + (uint32_t)data;
		}else if (data[i+NameLen+2] == 3)
		{
			par[i] = (uint32_t)outData + outDataLen;
			outDataLen += *((uint32_t *)&data[offset + i * 4]);
		}else
		{
			return -1;
		}
	}
	
	
	//执行函数
	switch (data[NameLen + 1])
	{
	case 0:
		re = ((int (*)())p)();
		break;
	case 1:
		re = ((int (*)())p)(par[0]);
		break;
	case 2:
		re = ((int (*)())p)(par[0],par[1]);
		break;
	case 3:
		re = ((int (*)())p)(par[0],par[1],par[2]);
		break;
	case 4:
		re = ((int (*)())p)(par[0],par[1],par[2],par[3]);
		break;
	case 5:
		re = ((int (*)())p)(par[0],par[1],par[2],par[3],par[4]);
		break;
	case 6:
		re = ((int (*)())p)(par[0],par[1],par[2],par[3],par[4],par[5]);
		break;
	case 7:
		re = ((int (*)())p)(par[0],par[1],par[2],par[3],par[4],par[5],par[6]);
		break;
	case 8:
		re = ((int (*)())p)(par[0],par[1],par[2],par[3],par[4],par[5],par[6],par[7]);
		break;
	case 9:
		re = ((int (*)())p)(par[0],par[1],par[2],par[3],par[4],par[5],par[6],par[7],par[8]);
		break;
	case 10:
		re = ((int (*)())p)(par[0],par[1],par[2],par[3],par[4],par[5],par[6],par[7],par[8],par[9]);
		break;
	default:
		return -1;
	}
	memcpy(outData,&re,4);
	*outLen = outDataLen;
	return re;
}



#if USE_FPU


//返回 输出长度有多少
int P_unPackASM_fpu(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen)
{
	int i,len;
	int outDataLen = 4;
	int n;
	int NameLen;
	int re;
	void *p;
	int offset = 0; //顺序存储时的偏移
	
//	uint32_t par[10];
	uint32_t tem;
	uint32_t rs[4+16];  //函数传递时使用的寄存器(若使用的硬件浮点，则包含16个s寄存器组，若不适用硬件浮点，则浮点数和普通公用4个r寄存器)
	uint32_t stack[40]; //最大支持40个参数存放在栈空间
	int rLen = 0; //当前r寄存器组使用的个数
	int sLen = 0; //当前s寄存器组使用的个数
	int stackLen = 0; //当前s寄存器组使用的个数
	
	NameLen = strlen((char *)data);
	len = data[NameLen + 1];
	i = 0;
	while(1)
	{
		if (fn[i].func == 0)
		{
			return -1;
		}
		
		if (strcmp((char *)fn[i].name,(char *)data) == 0)
		{
			p = fn[i].func;
			break;
		}
		i++;
	}

	/***********之后的数据要保证4字节对齐，通信的***************/
	offset = NameLen+1+1+len; //字符串长度 + '\0' + [len]
	n = (offset) % 4;
	offset += (4-n)%4;
	
	for (i = 0; i < len; i++)
	{
		if (data[i+NameLen+2] == 4)
		{
			tem = *((uint32_t *)&data[offset + i * 4]);  //这里data一定4字节对齐
			
			//tem中的数据添加到寄存器或栈中
			if (sLen < 16)
			{
				rs[sLen+4] = tem;
				sLen++;
			}else
			{
				stack[stackLen] = tem;
				stackLen++;
			}
		}else
		{
			if (data[i+NameLen+2] == 1)
			{
				tem = *((uint32_t *)&data[offset + i * 4]);  //这里data一定4字节对齐
			}else if (data[i+NameLen+2] == 2)
			{
				tem = *((uint32_t *)&data[offset + i * 4]) + (uint32_t)data;
				
			}else if (data[i+NameLen+2] == 3)
			{
				tem = (uint32_t)outData + outDataLen;
				outDataLen += *((uint32_t *)&data[offset + i * 4]);
			}else
			{
				return -1;
			}
			
			//tem中的数据添加到寄存器或栈中
			if (rLen < 4)
			{
				rs[rLen] = tem;
				rLen++;
			}else
			{
				stack[stackLen] = tem;
				stackLen++;
			}
		}
	}
	
	re = asm_runF(p, stack, stackLen, rs);
	
	memcpy(outData,&re,4);
	*outLen = outDataLen;
	return re;
}

#else



//返回 输出长度有多少
int P_unPackASM(const funpack_funNametab *fn, uint8_t *data, uint8_t *outData,int *outLen)
{
	int i,len;
	int outDataLen = 4;
	int n;
	int NameLen;
	int re;
	void *p;
	int offset = 0; //顺序存储时的偏移
	
//	uint32_t par[10];
	uint32_t tem;
	uint32_t rs[4+16];  //函数传递时使用的寄存器(若使用的硬件浮点，则包含16个s寄存器组，若不适用硬件浮点，则浮点数和普通公用4个r寄存器)
	uint32_t stack[40]; //最大支持40个参数存放在栈空间
	int rLen = 0; //当前r寄存器组使用的个数
	//int sLen = 0; //当前s寄存器组使用的个数
	int stackLen = 0; //当前s寄存器组使用的个数
	
	NameLen = strlen((char *)data);
	len = data[NameLen + 1];
	i = 0;
	while(1)
	{
		if (fn[i].func == 0)
		{
			return -1;
		}
		
		if (strcmp((char *)fn[i].name,(char *)data) == 0)
		{
			p = fn[i].func;
			break;
		}
		i++;
	}

	offset = NameLen+1+1+len; //字符串长度 + '\0' + [len]
	n = (offset) % 4;
	offset += (4-n)%4;
	
	for (i = 0; i < len; i++)
	{
		if ((data[i+NameLen+2] == 1) || (data[i+NameLen+2] == 4))
		{
			tem = *((uint32_t *)&data[offset + i * 4]);  //这里data一定4字节对齐
		}else if (data[i+NameLen+2] == 2)
		{
			tem = *((uint32_t *)&data[offset + i * 4]) + (uint32_t)data;
			
		}else if (data[i+NameLen+2] == 3)
		{
			tem = (uint32_t)outData + outDataLen;
			outDataLen += *((uint32_t *)&data[offset + i * 4]);
		}else
		{
			return -1;
		}
		
		//tem中的数据添加到寄存器或栈中
		if (rLen < 4)
		{
			rs[rLen] = tem;
			rLen++;
		}else
		{
			stack[stackLen] = tem;
			stackLen++;
		}
	}
	
	re = asm_runF(p, stack, stackLen, rs);
	
	memcpy(outData,&re,4);
	*outLen = outDataLen;
	return re;
}


#endif
