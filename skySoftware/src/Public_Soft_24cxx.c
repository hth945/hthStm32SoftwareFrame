#include "Public_Soft_24cxx.h" 
#include "Public_SoftI2C.h"


//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���

int AT24CXX_Read(i2c_adapter *adap,AT24Cxx_drv *at24cxx,uint16_t Addr,uint8_t *pBuffer,uint16_t NumToRead)
{
    i2c_msg msg[2];
    uint8_t buf1[10];   
    
	if((at24cxx->AT24C08_TYP)>AT24C16)
	{
		buf1[0] = Addr>>8;
		buf1[1] = Addr%256;
		msg[0].type=I2C_S_W;
		msg[0].addr=(at24cxx->AT24CXX_Address);
		msg[0].len=2;
		msg[0].buf=buf1;
 	}else
	{
		buf1[0] = Addr%256;
		msg[0].type=I2C_S_W;
		msg[0].addr=(at24cxx->AT24CXX_Address)+((Addr/256));
		msg[0].len=1;
		msg[0].buf=buf1;
	}
    
    msg[1].type=I2C_S_R;
    msg[1].addr=(at24cxx->AT24CXX_Address);
    msg[1].len=NumToRead;
    msg[1].buf=pBuffer; 
    
    return P_I2C_transfer(adap,msg,2);
}

//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
int AT24CXX_Write(i2c_adapter *adap,AT24Cxx_drv *at24cxx,uint16_t Addr,uint8_t *pBuffer,uint16_t NumToWrite)
{
    int ret;  
    i2c_msg msg[2];
    uint8_t buf1[10]; 
    
	if ((at24cxx->AT24C08_TYP)>AT24C16)
	{
		buf1[0] = Addr>>8;
		buf1[1] = Addr%256;
		msg[0].type=I2C_S_W;
		msg[0].addr=(at24cxx->AT24CXX_Address);
		msg[0].len=2;
		msg[0].buf=buf1;
 	}else
	{
		buf1[0] = Addr%256;
		msg[0].type=I2C_S_W;
		msg[0].addr=(at24cxx->AT24CXX_Address)+((Addr/256));
		msg[0].len=1;
		msg[0].buf=buf1;
	}
	
    msg[1].type=I2C_W;
    msg[1].addr=(at24cxx->AT24CXX_Address);
    msg[1].len=NumToWrite;
    msg[1].buf=pBuffer;
	
    ret=P_I2C_transfer(adap,msg,2);
	
    return	ret;
}




