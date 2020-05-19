#include "Public_Soft_24cxx.h" 
#include "Public_SoftI2C.h"
#include "delay.h"
#include "bsp_24Cxx.h"


static void BSP_AT24Cxx_Init_GPIO(void);
static void bsp_24Cxx_SCL_Set(uint8_t level){
	if(level)
	{
		GPIO_SetBits(GPIOB ,GPIO_Pin_10);
	}
	else
	{
		GPIO_ResetBits(GPIOB ,GPIO_Pin_10);
	}
	
}
static void bsp_24Cxx_SDA_Set(uint8_t level){
	if(level)
	{
		GPIO_SetBits(GPIOB ,GPIO_Pin_11);
	}
	else
	{
		GPIO_ResetBits(GPIOB ,GPIO_Pin_11);
	}
}
static uint8_t 	bsp_24Cxx_SDA_Read(){return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);}
//static void 	bsp_24Cxx_Delay(){Delay_us(10);}


//定义控制块结构体
i2c_adapter AT24Cxx_adap;
AT24Cxx_drv AT24Cxx[2];




void BSP_AT24Cxx_Init(){
	
    BSP_AT24Cxx_Init_GPIO();

    P_I2C_DeInit(&AT24Cxx_adap);
    AT24Cxx_adap.scl_set=bsp_24Cxx_SCL_Set;
    AT24Cxx_adap.sda_set=bsp_24Cxx_SDA_Set;
    AT24Cxx_adap.sda_read=bsp_24Cxx_SDA_Read;    
    P_I2C_Init(&AT24Cxx_adap);
	
	AT24Cxx[0].AT24C08_TYP=AT24C08;
	AT24Cxx[0].AT24CXX_Address=ADDR000;
}

void BSP_AT24Cxx_Init_GPIO(){
 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10); 	//PB6,PB7 输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_11); 	//PB6,PB7 输出高
	
}

int AT24CXX_ReadDate(uint8_t index,uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead){
    return AT24CXX_Read(&AT24Cxx_adap,&AT24Cxx[index],ReadAddr,pBuffer,NumToRead);
}

int AT24CXX_WriteDate(uint8_t index,uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite){
    return AT24CXX_Write(&AT24Cxx_adap,&AT24Cxx[index],WriteAddr,pBuffer,NumToWrite);
}





