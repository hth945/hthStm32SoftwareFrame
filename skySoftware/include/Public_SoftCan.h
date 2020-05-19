#ifndef _PUBLIC_SOFTCAN_H__
#define	_PUBLIC_SOFTCAN_H__

#include "stdint.h"
#include "commonIOInterface.h"


/*Bus总线中各种模块类型定义*/
#define BUS_MAIN        1 //测试机主机
#define BUS_POWER       2 //电源板
#define BUS_KEYBOARD    3 //按键盒
#define BUS_MONITOR     4 //按键盒上的显示器
#define BUS_CCD         5 //镜^
#define BUS_RS232       6 //Bus转串口(RS232)
#define BUS_MOTOR       7 //电机驱动
#define BUS_WAVE        8
#define BUS_LED         9
#define BUS_VOL         10
#define BUS_S10POWER    11 //电源板
#define BUS_CFL    		12 //新镜头
#define BUS_SCAN    	13 //扫描枪
#define BUS_KEY    	    14 //一拖五按键盒子
#define BUS_FORWARD	    15 //一拖十五抓板
#define BUS_CE48CORE    16 //ce48核心板
#define BUS_CE48KEY	    17 //ce48按键板
#define BUS_CE48C8T6    18 //ce48FPAGc8t6
#define BUS_DHCP        19 //自动分配ID总通信板
#define BUS_SD250       20 //自动分配ID总通信板


//注意 都使用扩展帧，id有29位， 同时禁止高7位连续为1

struct _Tcan_node;      //声明
struct _Tcan_adapter;    //声明


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
	//用户设置
	//要接收哪些id的数据 匹配成功的数据会放在 dev中
	uint32_t maskId;
	uint32_t mask;  
	
	uint32_t sendId; //发送数据使用的id
	
	
	//add时自动设置设置
	//此节点的通用输入输出(初始化之后 外部只需使用它) 
	//ps:userData为当前节点的地址
	//  :write2为发送函数 指向 S_CAN_write
	sky_comDriver dev; 
	struct _Tcan_adapter *adap;
	struct _Tcan_node *nextNode;  //节点要组成链表
	
}can_node; 


typedef struct _Tcan_adapter
{
	//用户设置
    int (*sendMsg)(uint32_t id, uint8_t *data, int len);  //can总线发送数据 bsp实现
	int (*addFilter)(int filterN, uint32_t id, uint32_t mask);      //增加一个过滤器  bsp实现
	
	//初始化时自动设置设置
	int filterN;       //已经添加的filter个数
	can_node *srcNode; //节点链表起始指针，之后每添加一个节点会添加到此链表中
	
}can_adapter;

typedef struct
{
//	union _data{
//		uint32_t id;
//		struct {
		uint32_t resev:3;   //低3位固定位b100， 位2表示扩展帧  位1表示数据帧， 位0为控制发送位硬件自动清零 
		uint32_t index:8;    //8bit 功能索引
		uint32_t src_id:3;   //3bit 源模块ID
		uint32_t src_type:6; //6bit 源模块类型
		uint32_t dst_id:3;   //3bit 目标模块ID
		uint32_t dst_type:6; //6bit 目标模块类型
		uint32_t resev2:3;   //高3位固定为b001，因为高7位不能全为1，兼容之前的程序
//		}bits;
//	}data;
	
	uint8_t filt_src;  
	uint8_t filt_dst;  // =0 不过滤dst_type、dst_id，=1都过滤  =2只过滤type =3只过滤id
	
}bus_config;   //sky内部定义节点


int P_CAN_DeInit(can_adapter *adap);
int P_CAN_Init(can_adapter *adap);

int P_CAN_NodeDeInit(can_node *node);
int P_CAN_AddNode(can_adapter *adap,can_node *node);

int P_CAN_SetNodeByBus(can_node *node, bus_config *bc);

//int P_CAN_AddFilter(can_adapter *adap, uint32_t id, uint32_t mask);



//功能：根据msg的id，遍历匹配srcNode链表中的节点，匹配上了就把数据输入到dev中的接收中
//void P_CAN_IRQHandler(can_adapter *adap,can_msg *msg); //接收中断中调用此函数，把数据发送到协议中  
int P_CAN_IRQHandler(can_adapter *adap,uint32_t id, uint8_t *data, int len); //接收中断中调用此函数，把数据发送到协议中  

//内部 不对外暴露(static声明)
//ps:userData为当前节点的地址
//需要考虑线程同步
//static int S_CAN_write(can_node *userData,uint8_t *data, int len);   //给节点实现发送数据函数(通用输入输出功能)

#endif
