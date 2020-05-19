#include "bsp_sd250x.h"
#include "skySoft_sd250x.h"
#include "stm32h7xx_hal.h"
#include "delay.h"

static void bsp_SD250X_Init_GPIO(void);
static void bsp_SD250X_SCL_Set(uint8_t level){HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9,level);}
static void bsp_SD250X_SDA_Set(uint8_t level){HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,level);}
static uint8_t bsp_SD250X_SDA_Read(){return HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8);}
static void bsp_SD250X_Delay(){Delay_us(10);}

i2c_adapter sd250x_adap;
//uint8_t	 data_Buf[8];
//uint8_t	 VBAT_Buf[2];
//uint16_t VBAT_VAL;
//Time_Def time_init={0x01,0x13,0x20,0x05,0x08,0x11,0x19};	//初始化实时时间

void SD250X_Init(){
    bsp_SD250X_Init_GPIO();
    
    P_I2C_DeInit(&sd250x_adap);
    sd250x_adap.scl_set=bsp_SD250X_SCL_Set;
    sd250x_adap.sda_set=bsp_SD250X_SDA_Set;
    sd250x_adap.sda_read=bsp_SD250X_SDA_Read;    
    P_I2C_Init(&sd250x_adap);
    
    skySoft_SD250X_Init(&sd250x_adap);
}

void bsp_SD250X_Init_GPIO(){
	GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_GPIOD_CLK_ENABLE();	
    
    GPIO_InitStructure.Pin=GPIO_PIN_9;
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull=GPIO_PULLUP;         		
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  
	HAL_GPIO_Init(GPIOD,&GPIO_InitStructure); 
    
    GPIO_InitStructure.Pin=GPIO_PIN_8;
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_OD;
	GPIO_InitStructure.Pull=GPIO_PULLUP;         		
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  
	HAL_GPIO_Init(GPIOD,&GPIO_InitStructure);     
}

int SD250X_RTC_ReadDate(Time_Def *psRTC){
    return skySoft_SD250X_RTC_ReadDate(&sd250x_adap,psRTC);
}

int SD250X_RTC_WriteDate(Time_Def *psRTC){
    return skySoft_SD250X_RTC_WriteDate(&sd250x_adap,psRTC);
}

/************* 给RTC写入计时的初始时间 ************/		
//	SD250X_RTC_WriteDate(&sd250x_adap,&time_init);
//	SD250X_RTC_ReadDate(&sd250x_adap,&sysTime);
//	printf("写入的初始时间为： \r\n");
//	printf("%02X-%02X-%02X  %02X:%02X:%02X  星期%02X\r\n\r\n", \
//	sysTime.year, sysTime.month, sysTime.day, sysTime.hour,\
//	sysTime.minute, sysTime.second, sysTime.week);	
//	printf("----------------------------\r\n\r\n");	
    
/*************** 读出芯片的ID号演示 ****************/	
//	SD250X_I2CReadSerial(&sd250x_adap,ID_Address,8,data_Buf);
//	printf("芯片64bit ID号为：\r\n");
//	for (i=0; i<8; i++)
//	printf("0x%02X ", data_Buf[i]);
//	printf("\r\n----------------------------\r\n\r\n");


/************** 读出后备电池电压演示 ***************/	
//	SD250X_I2CReadSerial(&sd250x_adap,Bat_H8,2,VBAT_Buf);
//	VBAT_VAL = (VBAT_Buf[0]>>7)*255 + VBAT_Buf[1];
//	printf("VBAT脚的电池电压为：%d.%d%dV\r\n\r\n", VBAT_VAL/100, VBAT_VAL%100/10, VBAT_VAL%10);

//	SD250X_I2CReadSerial(&sd250x_adap,0x16,1,&aaa);
//	printf("温度为：\r\n");
//	printf("%d ", aaa);
//	printf("\r\n----------------------------\r\n\r\n");
    



