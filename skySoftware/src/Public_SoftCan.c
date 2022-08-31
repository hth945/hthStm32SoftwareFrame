#include "Public_SoftCan.h"
#include "Public_util.h"

int P_CAN_DeInit(can_adapter *adap)
{
	memset(adap,0,sizeof(can_adapter));
	return 0;
}

int P_CAN_Init(can_adapter *adap)
{
	return 0;
}

int P_CAN_NodeDeInit(can_node *node)
{
	memset(node,0,sizeof(can_node));
	return 0;
}

static int P_CAN_AddFilter(can_adapter *adap,uint32_t id, uint32_t mask)
{
	int re;
	
	//������Ҫ��֤�����������ظ����
	
	re = adap->addFilter(adap->filterN, id, mask);
	if (re >= 0)
	{
		adap->filterN++;
		return adap->filterN;
	}
	
	return re;
}

static int S_CAN_write(sky_comDriver *self,uint8_t *data, int len)
{
	can_node *cn =(can_node *) ((uint8_t*)self - OFFSETOF(can_node, dev));
	
	while (len>0)
	{
		if (len >= 8)
		{
			cn->adap->sendMsg(cn->sendId >> 3, data, 8);
			len -= 8;
			data+= 8;
		}else
		{
			cn->adap->sendMsg(cn->sendId >> 3, data, len);
			len -= len;
		}
	}
	return 0;
	//return ((can_node *)self->userData)->adap->sendMsg(((can_node *)self->userData)->sendId, data, len);
}

int P_CAN_AddNode(can_adapter *adap,can_node *node)
{
	can_node *p; 
	if (adap->srcNode != 0)
	{
		p = adap->srcNode;
		while(p->nextNode != 0)
		{
			p = p->nextNode;
		}
		p->nextNode = node;
	}else
	{
		adap->srcNode = node;
	}
	
	node->nextNode = 0;
	node->adap = adap;
//	node->dev.userData = node;
	node->dev.write = S_CAN_write;
	
	P_CAN_AddFilter(adap, node->maskId, node->mask);
	
	return 0;
}


int P_CAN_SetNodeByBus(can_node *node, bus_config *bc)
{
	//��������յĸ�3λ�̶�Ϊb001��
	//��3λ�̶�λb100�� λ2��ʾ��չ֡  λ1��ʾ����֡�� λ0Ϊ���Ʒ���λӲ���Զ�����
	//uint32_t mask=0xE0000006, maskId=0x20000004, sendId=0x20000004;
	//maskId |=  ((bc->dst_type&0x3f)<<(7+16)) | ((bc->dst_id&0x07)<<(4+16))| ((bc->src_id&0x07)<<14)| ((bc->src_id&0x07)<<11);
	uint32_t tem;
	bus_config bcTem;
	
	memcpy(&bcTem, bc, sizeof(bus_config));
	bc = &bcTem;
	
	/******������id****/
	bc->resev = 4;
	bc->resev2 = 1;
	node->maskId = *(uint32_t *)(bc);
	
	/******����id****/
	tem = bc->dst_id;
	bc->dst_id = bc->src_id;
	bc->src_id = tem;
	tem = bc->dst_type;
	bc->dst_type = bc->src_type;
	bc->src_type = tem;
	node->sendId = *(uint32_t *)(bc);
	
	/******����****/
	*(uint32_t *)(bc) = 0xffffffff;
	bc->resev = 6;
	bc->resev2 = 1;
	
	if (bc->filt_dst == 1)  //������
	{
		
	}else if (bc->filt_dst == 0) //��������
	{
		bc->dst_id = 0;
		bc->dst_type = 0;
	}else if (bc->filt_dst == 2) //ֻ�������� ����id
	{
		bc->dst_id = 0;
	}else if (bc->filt_dst == 3) //ֻ����id
	{
		bc->dst_type = 0;
	}else 
	{
		while(1);
	}
	
	if (bc->filt_src == 1)  //������
	{
		
	}else if (bc->filt_src == 0) //��������
	{
		bc->src_id = 0;
		bc->src_type = 0;
	}else if (bc->filt_src == 2) //ֻ�������� ����id
	{
		bc->src_id = 0;
	}else if (bc->filt_src == 3) //ֻ����id
	{
		bc->src_type = 0;
	}else 
	{
		while(1);
	}
	bc->index = 0;
	node->mask = *(uint32_t *)(bc);
	
	return 0;
}

int P_CAN_IRQHandler(can_adapter *adap,uint32_t id, uint8_t *data, int len)
{
	can_node *p = adap->srcNode;
	
	while(p != 0)
	{
		if (((p->maskId ^ (id<<3)) & p->mask& 0xffffff8) == 0) //�Ա�ͨ�� �����id��3λ���������������λ
		{
			Write2dev(&p->dev, data, len);
		}
		
		p = p->nextNode;
	}
	
	return 0;
}



