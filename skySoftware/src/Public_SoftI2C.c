//V0.0.4
//1.加了常用的寄存器读写函数
//V0.0.3
//1.添加新函数,判断器件地址是否存在
//V0.0.2
//1.修复了读sda有问题的bug
//V0.0.1
//1.建立

#include "Public_SoftI2C.h"

__weak uint8_t P_I2C_Void_Read(void);
__weak void P_I2C_Void_Set(uint8_t level);
__weak void P_I2C_Void(void);
__weak void P_I2C_Delay(void);

void P_I2C_Start(i2c_adapter *adap);
void P_I2C_Stop(i2c_adapter *adap);
void P_I2C_SendByte(i2c_adapter *adap,uint8_t dat);
int P_I2C_WaitAck(i2c_adapter *adap);
uint8_t P_I2C_ReadByte(i2c_adapter *adap);
void P_I2C_ReplyAck(i2c_adapter *adap,uint8_t ack);

void P_I2C_DeInit(i2c_adapter *adap){
    adap->sda_set=P_I2C_Void_Set;
    adap->sda_read=P_I2C_Void_Read;
    adap->sda_outmode=P_I2C_Void;
    adap->sda_inmode=P_I2C_Void;
    adap->scl_set=P_I2C_Void_Set;
    adap->delay=P_I2C_Delay;
    adap->wait_overtime_cnt=250;
}

void P_I2C_Init(i2c_adapter *adap){
    adap->sda_outmode();
    adap->sda_set(1);
    adap->scl_set(1);
}

__weak uint8_t P_I2C_Void_Read(){
    return 0;
}

__weak void P_I2C_Void_Set(uint8_t level){
}

__weak void P_I2C_Void(){
}

__weak void P_I2C_Delay(){
    uint8_t i;
    for(i=0;i<168;i++);
}

int P_I2C_transfer(i2c_adapter *adap,i2c_msg *msgs,uint16_t msgs_num){
    uint16_t i,j;
    uint16_t addr,len=0;
    uint8_t *buf;
    uint8_t tail_stop=0;
    uint16_t type;
    
    for(i=0;i<msgs_num;i++){
        addr=msgs[i].addr<<1;
        len=msgs[i].len;
        buf=msgs[i].buf;
        type=msgs[i].type&0x7fff;
        
        switch(type){
            case I2C_S_W:
                P_I2C_Start(adap);//start信号
                P_I2C_SendByte(adap,addr+0);//发送写地址
                P_I2C_WaitAck(adap);
                for(j=0;j<len;j++){
                    P_I2C_SendByte(adap,buf[j]);//发送数据  
                    if(P_I2C_WaitAck(adap)!=0){
                        P_I2C_Stop(adap);
                        return -1;
                    }
                }
            break;
                
            case I2C_W:
                for(j=0;j<len;j++){
                    P_I2C_SendByte(adap,buf[j]);//发送数据  
                    if(P_I2C_WaitAck(adap)!=0){
                        P_I2C_Stop(adap);
                        return -1;
                    }
                }
            break;
                
            case I2C_S_R:
                P_I2C_Start(adap);//start信号
                P_I2C_SendByte(adap,addr+1);//发送读地址
                if(P_I2C_WaitAck(adap)!=0){//等待从机回应
                    P_I2C_Stop(adap);
                    return -1;
                }   
                for(j=0;j<len;j++){
                    buf[j]=P_I2C_ReadByte(adap);//读取数据  
                    P_I2C_ReplyAck(adap,j<(len-1)?1:0);//最后的一个数据不回ack    
                }
            break; 
            
            case I2C_R:
                for(j=0;j<len;j++){
                    buf[j]=P_I2C_ReadByte(adap);//读取数据  
                    P_I2C_ReplyAck(adap,j<(len-1)?1:0);//最后的一个数据不回ack    
                }
            break;
        }
        
        if(msgs[i].type&0x8000){//函数中间要发送停止位
            P_I2C_Stop(adap);
            tail_stop=0;//函数结束不需要停止位
        }else{
            tail_stop=1;//函数结束需要停止位
        }
    }
    
    if(tail_stop){
        P_I2C_Stop(adap);
    }
    return 0;
    
}

//判断器件是否存在
int P_I2C_DevExist(i2c_adapter *adap,uint8_t dev_addr){
    int ret;
    
    P_I2C_Start(adap);
    P_I2C_SendByte(adap,dev_addr<<1);//发送器件地址
    ret=P_I2C_WaitAck(adap);
    P_I2C_Stop(adap);
    
    return ret;
}

//该函数的组成成分 Start+WSlaveAddr+Reg+WDat+Stop
int P_I2C_WriteReg(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t dat){
    i2c_msg msg[1];
    uint8_t buf[2];
    
    buf[0]=reg;
    buf[1]=dat;
    msg[0].type=I2C_S_W;
    msg[0].addr=slave_addr;
    msg[0].len=2;
    msg[0].buf=buf;
    
    return P_I2C_transfer(adap,msg,1);
}

//该函数的组成成分 Start+WSlaveAddr+Reg+WDat...+Stop
int P_I2C_WriteRegBuf(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint16_t len,uint8_t *dat){
    i2c_msg msg[2];
    uint8_t buf[2];
    
    buf[0]=reg;
    msg[0].type=I2C_S_W;
    msg[0].addr=slave_addr;
    msg[0].len=1;
    msg[0].buf=buf;
    
    msg[1].type=I2C_W;
    msg[1].addr=slave_addr;
    msg[1].len=len;
    msg[1].buf=dat;    
    
    return P_I2C_transfer(adap,msg,2);
}

//该函数的组成成分 [Start+WSlaveAddr+Reg]+[Start+RAddr+RDat+Stop]
int P_I2C_ReadReg(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t *dat){
    i2c_msg msg[2];
    uint8_t buf[2];
    
    buf[0]=reg;
    msg[0].type=I2C_S_W;
    msg[0].addr=slave_addr;
    msg[0].len=1;
    msg[0].buf=buf;
    
    msg[1].type=I2C_S_R;
    msg[1].addr=slave_addr;
    msg[1].len=1;
    msg[1].buf=dat;
    
    return P_I2C_transfer(adap,msg,2);
}

//该函数的组成成分 [Start+WSlaveAddr+Reg]+[Start+RAddr+RDat...+Stop]
int P_I2C_ReadRegBuf(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint16_t len,uint8_t *dat){
    i2c_msg msg[2];
    uint8_t buf[2];
    
    buf[0]=reg;
    msg[0].type=I2C_S_W;
    msg[0].addr=slave_addr;
    msg[0].len=1;
    msg[0].buf=buf;
    
    msg[1].type=I2C_S_R;
    msg[1].addr=slave_addr;
    msg[1].len=len;
    msg[1].buf=dat;
    
    return P_I2C_transfer(adap,msg,2);
}

void P_I2C_WriteRegMask(i2c_adapter *adap,uint8_t slave_addr,uint8_t reg,uint8_t mask,uint8_t dat){
    uint8_t temp_dat;
    
    P_I2C_ReadReg(adap,slave_addr,reg,&temp_dat);//先读取防止改到其他值
    
    temp_dat&=~mask;//掩码的内的数据先清除
    temp_dat|=(mask&dat);//赋值
    
    P_I2C_WriteReg(adap,slave_addr,reg,temp_dat);
}

/*-----------------------------------*/

void P_I2C_Start(i2c_adapter *adap){
    adap->sda_outmode();
    adap->sda_set(1);
    adap->scl_set(1);
    adap->delay();
    adap->sda_set(0);
    adap->delay();
    adap->scl_set(0);
    adap->delay();
}

void P_I2C_Stop(i2c_adapter *adap){
    adap->sda_outmode();
    adap->sda_set(0);
    adap->scl_set(0);
    adap->delay();
    adap->scl_set(1);
    adap->delay();
    adap->sda_set(1);
    adap->delay();
}

void P_I2C_SendByte(i2c_adapter *adap,uint8_t dat){
    uint8_t i=8;
    adap->sda_outmode();
    adap->scl_set(0);
    while(i--){
        adap->sda_set(dat&0x80?1:0);
        dat<<=1;
        adap->delay();
        adap->scl_set(1);
        adap->delay();
        adap->scl_set(0);
        adap->delay();
        adap->sda_set(0);
        adap->delay();
    }
}

uint8_t P_I2C_ReadByte(i2c_adapter *adap){
    uint8_t i=8;
    uint8_t byte=0;	

    adap->sda_inmode();
    adap->sda_set(1); //使内部od断开
    adap->scl_set(0);
    while(i--){
        byte<<=1;
        adap->delay();
        adap->scl_set(1);
        adap->delay();
        if(adap->sda_read()){//scl下降之前读取
            byte|=0x01;
        }
        adap->scl_set(0);
        adap->delay();
    }
    return byte;
}
        
int P_I2C_WaitAck(i2c_adapter *adap){
    uint16_t cnt=0;
    adap->sda_inmode();
    adap->sda_set(1); //使内部od断开
    adap->delay();
    adap->scl_set(1);
    adap->delay();
	while(adap->sda_read()){
		cnt++;
		if(cnt>adap->wait_overtime_cnt){
            adap->scl_set(0);
            adap->delay();
			return -1;
		}
	}
    adap->scl_set(0);
    adap->delay();
    return 0;
}

void P_I2C_ReplyAck(i2c_adapter *adap,uint8_t ack){
    adap->scl_set(0);
    adap->sda_outmode();
    adap->sda_set(ack?0:1);
    adap->delay();
    adap->scl_set(1);
    adap->delay();
    adap->scl_set(0);
    adap->delay();
    adap->sda_set(0);
    adap->delay();
}

