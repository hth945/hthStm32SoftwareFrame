#ifndef _SKYSOFT_SD250x_H__
#define _SKYSOFT_SD250x_H__	 

#include "stdint.h" 
#include "Public_SoftI2C.h"

#define		RTC_Address     0x32//0x64 

/******************** Alarm register ********************/
#define		Alarm_SC				0x07
#define		Alarm_MN				0x08
#define		Alarm_HR				0x09
#define		Alarm_WK				0x0A
#define		Alarm_DY				0x0B
#define		Alarm_MO				0x0C
#define		Alarm_YR				0x0D
#define		Alarm_EN				0x0E

/******************** Control Register *******************/
#define		CTR1            0x0F
#define		CTR2            0x10
#define		CTR3            0x11

/******************** Battery Register ********************/
#define		Chg_MG          0x18		//充电管理寄存器地址
#define		Bat_H8          0x1A		//电量最高位寄存器地址
#define		Bat_L8          0x1B		//电量低八位寄存器地址

/*********************** ID Register **********************/
#define		ID_Address			0x72		//ID号起始地址

/********************************************************/
#define		Chg_enable			0x82
#define		Chg_disable			0


enum Freq{F_0Hz, F32KHz, F4096Hz, F1024Hz, F64Hz, F32Hz, F16Hz, F8Hz, \
		  F4Hz, F2Hz, F1Hz, F1_2Hz, F1_4Hz, F1_8Hz, F1_16Hz, F_1s};

enum clk_Souce{S_4096Hz, S_1024Hz, S_1s, S_1min};

/*此结构体定义了时间信息包括年、月、日、星期、时、分、秒*/
typedef	struct{
	uint8_t	second;
	uint8_t	minute;
	uint8_t	hour;
	uint8_t	week;
	uint8_t	day;
	uint8_t	month;
	uint8_t	year;
} Time_Def;

/*此结构体定义了倒计时中断可供配置的频率源、IM和初值主要参数*/
typedef	struct
{
	enum clk_Souce d_clk;
	uint8_t   IM;	//IM=1:周期性中断
	uint32_t  init_val;
} CountDown_Def;

int skySoft_SD250X_WriteRTC_Enable(i2c_adapter *adap);
int skySoft_SD250X_WriteRTC_Disable(i2c_adapter *adap);
int skySoft_SD250X_I2CWriteSerial(i2c_adapter *adap,uint8_t Address, uint8_t length, uint8_t *ps);
uint8_t skySoft_SD250X_I2CReadSerial(i2c_adapter *adap,uint8_t Address, uint8_t length, uint8_t *ps);
void skySoft_SD250X_Set_CountDown(i2c_adapter *adap,CountDown_Def *CountDown_Init)		;
void skySoft_SD250X_Set_Alarm(i2c_adapter *adap,uint8_t Enable_config, Time_Def *psRTC)	;
void skySoft_SD250X_SetFrq(i2c_adapter *adap,enum Freq F_Out)	;
void skySoft_SD250X_ClrINT(i2c_adapter *adap,uint8_t int_EN) ;

void skySoft_SD250X_Init(i2c_adapter *adap);
int skySoft_SD250X_RTC_WriteDate(i2c_adapter *adap,Time_Def *psRTC);
int skySoft_SD250X_RTC_ReadDate(i2c_adapter *adap,Time_Def *psRTC);

#endif


