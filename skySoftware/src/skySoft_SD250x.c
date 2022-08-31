//�汾 V0.0.1

#include "skySoft_sd250x.h"


void skySoft_SD250X_Init(i2c_adapter *adap){
    uint8_t data_Buf[1];
    
    //���ʹ��
	data_Buf[0] = Chg_enable;
	skySoft_SD250X_I2CWriteSerial(adap,Chg_MG,1,data_Buf);    
}


/*********************************************
 * ��������WriteRTC_Enable
 * ��  ����RTCд�������
 * ��  �룺��
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
 ********************************************/
int skySoft_SD250X_WriteRTC_Enable(i2c_adapter *adap)
{
    i2c_msg msg[2];
    uint8_t buf1[2],buf2[2];
    
    buf1[0]=CTR2;
    buf1[1]=0x80;//��WRTC1=1  
    msg[0].type=I2C_S_W|I2C_P;
    msg[0].addr=RTC_Address;
    msg[0].len=2;
    msg[0].buf=buf1;
    
    buf2[0]=CTR1;
    buf2[1]=0x84;//��WRTC2,WRTC3=1    
    msg[1].type=I2C_S_W;
    msg[1].addr=RTC_Address;
    msg[1].len=2;
    msg[1].buf=buf2;    
    
    return P_I2C_transfer(adap,msg,2);
}

/*********************************************
 * ��������WriteRTC_Disable
 * ��  ����RTCд��ֹ����
 * ��  �룺��
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
 ********************************************/
int skySoft_SD250X_WriteRTC_Disable(i2c_adapter *adap)
{
    i2c_msg msg[1];
    uint8_t buf1[3];
    
    buf1[0]=CTR1;//����д��ַ0FH    
    buf1[1]=0x0;//��WRTC2,WRTC3=0 
    buf1[2]=0x0;//��WRTC1=0(10H��ַ) 
    msg[0].type=I2C_S_W;
    msg[0].addr=RTC_Address;
    msg[0].len=3;
    msg[0].buf=buf1;
    
    return P_I2C_transfer(adap,msg,1);
}

/*********************************************
 * ��������RTC_WriteDate
 * ��  ����дRTCʵʱ���ݼĴ���
 * ��  �룺ʱ��ṹ��ָ��
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
 ********************************************/
int skySoft_SD250X_RTC_WriteDate(i2c_adapter *adap,Time_Def *psRTC)	//дʱ�����Ҫ��һ�ζ�ʵʱʱ��Ĵ���(00H~06H)����д�룬
{                               //�����Ե�����7��ʱ�������е�ĳһλ����д����,������ܻ�����ʱ�����ݵĴ����λ. 
                                //Ҫ�޸�����ĳһ������ , Ӧһ����д��ȫ�� 7 ��ʵʱʱ������.
    int ret;    
    i2c_msg msg[1];
    uint8_t buf1[8];        

    buf1[0]=0;//����д��ʼ��ַ   
    buf1[1]=psRTC->second;	 //second 
    buf1[2]=psRTC->minute;	 //minute   
    buf1[3]=psRTC->hour|0x80;//hour ,ͬʱ����Сʱ�Ĵ������λ��0��Ϊ12Сʱ�ƣ�1��Ϊ24Сʱ�ƣ�  
    buf1[4]=psRTC->week;     //week     
    buf1[5]=psRTC->day;		 //day
    buf1[6]=psRTC->month;	 //month      
    buf1[7]=psRTC->year;	 //year
    msg[0].type=I2C_S_W;
    msg[0].addr=RTC_Address;
    msg[0].len=8;
    msg[0].buf=buf1;
    
    skySoft_SD250X_WriteRTC_Enable(adap);		//ʹ�ܣ�����
    ret=P_I2C_transfer(adap,msg,1);
    skySoft_SD250X_WriteRTC_Disable(adap);		//����
    
    return ret;
}

/*********************************************
 * ��������RTC_ReadDate
 * ��  ������RTCʵʱ���ݼĴ���
 * ��  �룺ʱ��ṹ��ָ��
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
 ********************************************/
int skySoft_SD250X_RTC_ReadDate(i2c_adapter *adap,Time_Def *psRTC)
{
    int ret;      
    i2c_msg msg[2];
    uint8_t buf1[1],buf2[7];   
    
    buf1[0]=0;//����д��ʼ��ַ   
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
 * ������     ��I2CWriteSerial
 * ��  ��     ��I2C��ָ����ַдһ�ֽ�����
 * Device_Addr��I2C�豸��ַ
 * Address    ���ڲ���ַ
 * length     ���ֽڳ���
 * ps         ��������ָ��
 * ���       ��TRUE �ɹ���FALSE ʧ��
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
 * ������     ��I2CReadSerial
 * ��  ��     ��I2C��ָ����ַдһ�ֽ�����
 * Device_Addr��I2C�豸��ַ
 * Address    ���ڲ���ַ
 * length     ���ֽڳ���
 * ps         ��������ָ��
 * ���       ��TRUE �ɹ���FALSE ʧ��
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
 * ��������Set_CountDown
 * ��  �������õ���ʱ�ж�
 * ��  �룺CountDown_Init ����ʱ�жϽṹ��ָ�� 
 * ��  ������
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
 * ��������Set_Alarm
 * ��  �������ñ����жϣ����ӹ��ܣ�
 * Enable_config��ʹ������  
 * psRTC������ʱ���ʱ��ṹ��ָ��
 * ��  ������
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
 * ��������SetFrq
 * ��  ��������RTCƵ���жϣ���INT�����Ƶ�ʷ���
 * ��  �룺Ƶ��ֵ
 * ��  ������
 ********************************************/
void skySoft_SD250X_SetFrq(i2c_adapter *adap,enum Freq F_Out)					
{
    uint8_t buf[2];
    buf[0] = 0xA1;
    buf[1] = F_Out;
    skySoft_SD250X_I2CWriteSerial(adap,CTR2,2,buf);
}

/*********************************************
 * ��������ClrINT
 * ��  ������ֹ�ж�
 * int_EN���ж����� INTDE��INTDE��INTDE
 * ��  ������
 ********************************************/
void skySoft_SD250X_ClrINT(i2c_adapter *adap,uint8_t int_EN)
{
    uint8_t buf;
    buf = 0x80 & (~int_EN);
    skySoft_SD250X_I2CWriteSerial(adap,CTR2,1,&buf);
}

