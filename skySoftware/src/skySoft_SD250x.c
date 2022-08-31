//版本 V0.0.1

#include "skySoft_sd250x.h"


void skySoft_SD250X_Init(i2c_adapter *adap){
    uint8_t data_Buf[1];
    
    //充电使能
	data_Buf[0] = Chg_enable;
	skySoft_SD250X_I2CWriteSerial(adap,Chg_MG,1,data_Buf);    
}


/*********************************************
 * 函数名：WriteRTC_Enable
 * 描  述：RTC写允许程序
 * 输  入：无
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
int skySoft_SD250X_WriteRTC_Enable(i2c_adapter *adap)
{
    i2c_msg msg[2];
    uint8_t buf1[2],buf2[2];
    
    buf1[0]=CTR2;
    buf1[1]=0x80;//置WRTC1=1  
    msg[0].type=I2C_S_W|I2C_P;
    msg[0].addr=RTC_Address;
    msg[0].len=2;
    msg[0].buf=buf1;
    
    buf2[0]=CTR1;
    buf2[1]=0x84;//置WRTC2,WRTC3=1    
    msg[1].type=I2C_S_W;
    msg[1].addr=RTC_Address;
    msg[1].len=2;
    msg[1].buf=buf2;    
    
    return P_I2C_transfer(adap,msg,2);
}

/*********************************************
 * 函数名：WriteRTC_Disable
 * 描  述：RTC写禁止程序
 * 输  入：无
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
int skySoft_SD250X_WriteRTC_Disable(i2c_adapter *adap)
{
    i2c_msg msg[1];
    uint8_t buf1[3];
    
    buf1[0]=CTR1;//设置写地址0FH    
    buf1[1]=0x0;//置WRTC2,WRTC3=0 
    buf1[2]=0x0;//置WRTC1=0(10H地址) 
    msg[0].type=I2C_S_W;
    msg[0].addr=RTC_Address;
    msg[0].len=3;
    msg[0].buf=buf1;
    
    return P_I2C_transfer(adap,msg,1);
}

/*********************************************
 * 函数名：RTC_WriteDate
 * 描  述：写RTC实时数据寄存器
 * 输  入：时间结构体指针
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
int skySoft_SD250X_RTC_WriteDate(i2c_adapter *adap,Time_Def *psRTC)	//写时间操作要求一次对实时时间寄存器(00H~06H)依次写入，
{                               //不可以单独对7个时间数据中的某一位进行写操作,否则可能会引起时间数据的错误进位. 
                                //要修改其中某一个数据 , 应一次性写入全部 7 个实时时钟数据.
    int ret;    
    i2c_msg msg[1];
    uint8_t buf1[8];        

    buf1[0]=0;//设置写起始地址   
    buf1[1]=psRTC->second;	 //second 
    buf1[2]=psRTC->minute;	 //minute   
    buf1[3]=psRTC->hour|0x80;//hour ,同时设置小时寄存器最高位（0：为12小时制，1：为24小时制）  
    buf1[4]=psRTC->week;     //week     
    buf1[5]=psRTC->day;		 //day
    buf1[6]=psRTC->month;	 //month      
    buf1[7]=psRTC->year;	 //year
    msg[0].type=I2C_S_W;
    msg[0].addr=RTC_Address;
    msg[0].len=8;
    msg[0].buf=buf1;
    
    skySoft_SD250X_WriteRTC_Enable(adap);		//使能，开锁
    ret=P_I2C_transfer(adap,msg,1);
    skySoft_SD250X_WriteRTC_Disable(adap);		//关锁
    
    return ret;
}

/*********************************************
 * 函数名：RTC_ReadDate
 * 描  述：读RTC实时数据寄存器
 * 输  入：时间结构体指针
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
int skySoft_SD250X_RTC_ReadDate(i2c_adapter *adap,Time_Def *psRTC)
{
    int ret;      
    i2c_msg msg[2];
    uint8_t buf1[1],buf2[7];   
    
    buf1[0]=0;//设置写起始地址   
    msg[0].type=I2C_S_W;
    msg[0].addr=RTC_Address;
    msg[0].len=1;
    msg[0].buf=buf1;
    
    msg[1].type=I2C_S_R;
    msg[1].addr=RTC_Address;
    msg[1].len=7;
    msg[1].buf=buf2; 
    
    ret=P_I2C_transfer(adap,msg,2);
    psRTC->second=buf2[0];
    psRTC->minute=buf2[1];
    psRTC->hour=buf2[2] & 0x7F;
    psRTC->week=buf2[3];
    psRTC->day=buf2[4];
    psRTC->month=buf2[5];
    psRTC->year=buf2[6];  
    
    return ret;
}

/*********************************************
 * 函数名     ：I2CWriteSerial
 * 描  述     ：I2C在指定地址写一字节数据
 * Device_Addr：I2C设备地址
 * Address    ：内部地址
 * length     ：字节长度
 * ps         ：缓存区指针
 * 输出       ：TRUE 成功，FALSE 失败
 ********************************************/	
int skySoft_SD250X_I2CWriteSerial(i2c_adapter *adap,uint8_t Address, uint8_t length, uint8_t *ps)
{
    int ret;  
    uint8_t i;
    i2c_msg msg[1];
    uint8_t buf1[10]; 
    
    buf1[0]=Address;
    for(i=0;i<length;i++){
        buf1[1+i]=ps[i];
    }
    msg[0].type=I2C_S_W;
    msg[0].addr=RTC_Address;
    msg[0].len=length+1;
    msg[0].buf=buf1;
    
    skySoft_SD250X_WriteRTC_Enable(adap);
    ret=P_I2C_transfer(adap,msg,1);
    skySoft_SD250X_WriteRTC_Disable(adap);
    
    return	ret;
}

/*********************************************
 * 函数名     ：I2CReadSerial
 * 描  述     ：I2C在指定地址写一字节数据
 * Device_Addr：I2C设备地址
 * Address    ：内部地址
 * length     ：字节长度
 * ps         ：缓存区指针
 * 输出       ：TRUE 成功，FALSE 失败
 ********************************************/	
uint8_t skySoft_SD250X_I2CReadSerial(i2c_adapter *adap,uint8_t Address, uint8_t length, uint8_t *ps)
{
    i2c_msg msg[2];
    uint8_t buf1[1];   
    
    buf1[0]=Address;
    msg[0].type=I2C_S_W;
    msg[0].addr=RTC_Address;
    msg[0].len=1;
    msg[0].buf=buf1;
    
    msg[1].type=I2C_S_R;
    msg[1].addr=RTC_Address;
    msg[1].len=length;
    msg[1].buf=ps; 
    
    return P_I2C_transfer(adap,msg,2);
    
}

/*********************************************
 * 函数名：Set_CountDown
 * 描  述：设置倒计时中断
 * 输  入：CountDown_Init 倒计时中断结构体指针 
 * 输  出：无
 ********************************************/
void skySoft_SD250X_Set_CountDown(i2c_adapter *adap,CountDown_Def *CountDown_Init)
{
    uint8_t buf[6];
    uint8_t tem=0xF0;
    buf[0] = (CountDown_Init->IM<<6)|0xB4;//10H
    buf[1] = CountDown_Init->d_clk<<4;	  //11H
    buf[2] = 0;							  //12H
    buf[3] = CountDown_Init->init_val & 0x0000FF;//13H
    buf[4] = (CountDown_Init->init_val & 0x00FF00) >> 8;//14H
    buf[5] = (CountDown_Init->init_val & 0xFF0000) >> 16;//15H
    skySoft_SD250X_I2CWriteSerial(adap,CTR2,1,&tem);
    skySoft_SD250X_I2CWriteSerial(adap,CTR2,6,buf);
}

/*********************************************
 * 函数名：Set_Alarm
 * 描  述：设置报警中断（闹钟功能）
 * Enable_config：使能设置  
 * psRTC：报警时间的时间结构体指针
 * 输  出：无
 ********************************************/
void skySoft_SD250X_Set_Alarm(i2c_adapter *adap,uint8_t Enable_config, Time_Def *psRTC)
{
    uint8_t buf[10];
    buf[0] = psRTC->second;
    buf[1] = psRTC->minute;
    buf[2] = psRTC->hour;
    buf[3] = 0;
    buf[4] = psRTC->day;
    buf[5] = psRTC->month;
    buf[6] = psRTC->year;
    buf[7] = Enable_config;
    buf[8] = 0xFF;
    buf[9] = 0x92;	
    skySoft_SD250X_I2CWriteSerial(adap,Alarm_SC,10,buf);
}

/*********************************************
 * 函数名：SetFrq
 * 描  述：设置RTC频率中断，从INT脚输出频率方波
 * 输  入：频率值
 * 输  出：无
 ********************************************/
void skySoft_SD250X_SetFrq(i2c_adapter *adap,enum Freq F_Out)					
{
    uint8_t buf[2];
    buf[0] = 0xA1;
    buf[1] = F_Out;
    skySoft_SD250X_I2CWriteSerial(adap,CTR2,2,buf);
}

/*********************************************
 * 函数名：ClrINT
 * 描  述：禁止中断
 * int_EN：中断类型 INTDE、INTDE、INTDE
 * 输  出：无
 ********************************************/
void skySoft_SD250X_ClrINT(i2c_adapter *adap,uint8_t int_EN)
{
    uint8_t buf;
    buf = 0x80 & (~int_EN);
    skySoft_SD250X_I2CWriteSerial(adap,CTR2,1,&buf);
}

