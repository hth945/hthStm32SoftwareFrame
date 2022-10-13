#ifndef _PUBLIC_SOFTCAN_H__
#define	_PUBLIC_SOFTCAN_H__

#include "stdint.h"
#include "commonIOInterface.h"


/*Bus�����и���ģ�����Ͷ���*/
#define BUS_MAIN        1 //���Ի�����
#define BUS_POWER       2 //��Դ��
#define BUS_KEYBOARD    3 //������
#define BUS_MONITOR     4 //�������ϵ���ʾ��
#define BUS_CCD         5 //���^
#define BUS_RS232       6 //Busת����(RS232)
#define BUS_MOTOR       7 //�������
#define BUS_WAVE        8
#define BUS_LED         9
#define BUS_VOL         10
#define BUS_S10POWER    11 //��Դ��
#define BUS_CFL    		12 //�¾�ͷ
#define BUS_SCAN    	13 //ɨ��ǹ
#define BUS_KEY    	    14 //һ���尴������
#define BUS_FORWARD	    15 //һ��ʮ��ץ��
#define BUS_CFL2    	16 //�¾�ͷ
#define BUS_CE48KEY	    17 //ce48������
#define BUS_CE48C8T6    18 //ce48FPAGc8t6
#define BUS_DHCP        19 //�Զ�����ID��ͨ�Ű�
#define BUS_SD250       20 //ʱ��ģ��
#define BUS_MC_100      21 //����Ļ��Ե��ɫ
#define BUS_ML100       22 //��©���
#define BUS_BL20        23 //50·����
#define BUS_BL20MAIN    24 //50·������İ�
#define BUS_LX          25 //���а�
#define BUS_CE48CORE    26 //ce48 ���İ�
#define BUS_3DPOWER     27 //�߾��ȵ�Դģ����İ�
#define BUS_SMALLBL     28//�߾��ȱ�����������
#define BUS_CE48AMMETER 29 //ce48���
#define BUS_A30EXP      30 //A30��Դ��


//ע�� ��ʹ����չ֡��id��29λ�� ͬʱ��ֹ��7λ����Ϊ1

struct _Tcan_node;      //����
struct _Tcan_adapter;    //����


//typedef struct
//{
//  uint32_t StdId; 
//  uint32_t ExtId;
//  uint8_t IDE;
//  uint8_t RTR;
//  uint8_t DLC;
//  uint8_t Data[8];
//  uint8_t FMI;
//}can_msg;


typedef struct _Tcan_node
{
	//�û�����
	//Ҫ������Щid������ ƥ��ɹ������ݻ���� dev��
	uint32_t maskId;
	uint32_t mask;  
	
	uint32_t sendId; //��������ʹ�õ�id
	
	
	//addʱ�Զ���������
	//�˽ڵ��ͨ���������(��ʼ��֮�� �ⲿֻ��ʹ����) 
	//ps:userDataΪ��ǰ�ڵ�ĵ�ַ
	//  :write2Ϊ���ͺ��� ָ�� S_CAN_write
	sky_comDriver dev; 
	struct _Tcan_adapter *adap;
	struct _Tcan_node *nextNode;  //�ڵ�Ҫ�������
	
}can_node; 


typedef struct _Tcan_adapter
{
	//�û�����
    int (*sendMsg)(uint32_t id, uint8_t *data, int len);  //can���߷������� bspʵ��
	int (*addFilter)(int filterN, uint32_t id, uint32_t mask);      //����һ��������  bspʵ��
	
	//��ʼ��ʱ�Զ���������
	int filterN;       //�Ѿ���ӵ�filter����
	can_node *srcNode; //�ڵ�������ʼָ�룬֮��ÿ���һ���ڵ����ӵ���������
	
}can_adapter;

typedef struct
{
//	union _data{
//		uint32_t id;
//		struct {
		uint32_t resev:3;   //��3λ�̶�λb100�� λ2��ʾ��չ֡  λ1��ʾ����֡�� λ0Ϊ���Ʒ���λӲ���Զ����� 
		uint32_t index:8;    //8bit ��������
		uint32_t src_id:3;   //3bit Դģ��ID
		uint32_t src_type:6; //6bit Դģ������
		uint32_t dst_id:3;   //3bit Ŀ��ģ��ID
		uint32_t dst_type:6; //6bit Ŀ��ģ������
		uint32_t resev2:3;   //��3λ�̶�Ϊb001����Ϊ��7λ����ȫΪ1������֮ǰ�ĳ���
//		}bits;
//	}data;
	
	uint8_t filt_src;  
	uint8_t filt_dst;  // =0 ������dst_type��dst_id��=1������  =2ֻ����type =3ֻ����id
	
}bus_config;   //sky�ڲ�����ڵ�


int P_CAN_DeInit(can_adapter *adap);
int P_CAN_Init(can_adapter *adap);

int P_CAN_NodeDeInit(can_node *node);
int P_CAN_AddNode(can_adapter *adap,can_node *node);

int P_CAN_SetNodeByBus(can_node *node, bus_config *bc);

//int P_CAN_AddFilter(can_adapter *adap, uint32_t id, uint32_t mask);



//���ܣ�����msg��id������ƥ��srcNode�����еĽڵ㣬ƥ�����˾Ͱ��������뵽dev�еĽ�����
//void P_CAN_IRQHandler(can_adapter *adap,can_msg *msg); //�����ж��е��ô˺����������ݷ��͵�Э����  
int P_CAN_IRQHandler(can_adapter *adap,uint32_t id, uint8_t *data, int len); //�����ж��е��ô˺����������ݷ��͵�Э����  

//�ڲ� �����Ⱪ¶(static����)
//ps:userDataΪ��ǰ�ڵ�ĵ�ַ
//��Ҫ�����߳�ͬ��
//static int S_CAN_write(can_node *userData,uint8_t *data, int len);   //���ڵ�ʵ�ַ������ݺ���(ͨ�������������)

#endif
