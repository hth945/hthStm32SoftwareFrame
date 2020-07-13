#include "SysTick.h"


static u32 sg_systick_1ms_con=0;

static uint32_t _20ms_cnt;
unsigned int _1ms_ok = 0;
unsigned int _20ms_ok = 0;

unsigned int task_time_cnt[MAX_TASK]; 

typedef struct
{
    unsigned int td; //
    void (*fp)(void);
}TaskStruct;

static TaskStruct sg_task[MAX_TASK];


/********************************************************************************
*函得：SysTick_Init
*描述  ：酉到yTickr
*参数  ：无
*返回  ：o
********************************************************************************/
void SysTick_Init(void)
{
	unsigned int i;
    
    for(i=0; i<MAX_TASK; i++)
    {
        sg_task[i].td = 0;
    }
        
    if (SysTick_Config(SystemCoreClock / SYSTICK_FRE_HZ))
    {
        while (1);
    }
}


/********************************************************************************
*函得：TimingDelay_Decrement
*描述  ：Tickr的中嗪担在SysTick_Handler()调用
*参数  ：o
*返回  ：o
********************************************************************************/ 
void SysTick_Handler(void)
{
	unsigned int i;
	
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; //PSysTick中
    
    ++sg_systick_1ms_con;
    
    _1ms_ok = 1;


    //多任杖rgr
    for(i = 0; i < MAX_TASK; i++)
    {
        if(task_time_cnt[i] > 0)
        {
            task_time_cnt[i]--;
        }
        else
        {
            if(sg_task[i].td != 0)
            {
                task_time_cnt[i] = sg_task[i].td;
                sg_task[i].fp(); //绦谐绦
                break;     //每次只绦幸程序
            }
        }
    }


	if(++_20ms_cnt >= 20)
	{
		_20ms_cnt = 0;
		_20ms_ok = 1;
	}
    
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //_SysTick中
}

/********************************************************************************
*函得：MeasureTimeStart_ms
*描述  ：@取初始rg，_始y量rg
*参数  ：o
*返回  ：前rg算值 单位ms
********************************************************************************/
unsigned int MeasureTimeStart_ms(void)
{
	return sg_systick_1ms_con;
}


/********************************************************************************
*函得：GetMeasureTime_ms
*描述  ：@取 {用MeasureTimeStart_ms()r到F在rgL度 
*参数  ：ts-是初rg，就是{用MeasureTimeStart_ms()函r的返回值。
            本函狄根@初始rg碛算rg差
*返回  ：{用MeasureTimeStart_ms()r到F在rgL度 单位ms
********************************************************************************/
unsigned int GetMeasureTime_ms(unsigned int ts)
{
	u32 tmp;
	tmp = sg_systick_1ms_con;
    if(tmp < ts)
	{
		tmp = 0xffffffff - ts + tmp;
	}
	else
	{
		tmp = tmp - ts;
	}
	return tmp;
}


/********************************************************************************
*函得：Sys_CreateTask
*描述  ：添加需要定r绦械暮
*参数  ：task_num - 任站
         fp       - 函抵羔
         td       - g隔rg，即每隔tdrg 绦幸淮fp函
*返回  ：{用MeasureTimeStart()r到F在rgL度 单位1个SYSTICK (1ms)
********************************************************************************/
void Sys_CreateTask(u8 task_num, void (*fp)(void), unsigned int td)
{
    sg_task[task_num].fp = fp;
    sg_task[task_num].td = td;
    task_time_cnt[task_num] = td;
}

/********************************* SKYCODE ************************************/
