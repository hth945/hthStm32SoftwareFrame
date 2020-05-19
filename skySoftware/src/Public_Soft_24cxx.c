#include "Public_Soft_24cxx.h" 
#include "Public_SoftI2C.h"


//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数

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

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
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




