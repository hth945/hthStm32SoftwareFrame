#include "bsp_ADS1259.h"
#include "skySoft_ADS1259.h"
#include "sysio.h" 
#include "delay.h"

static void ADS12xx_Rst_Set(uint8_t level){PBout(14)=level;}
static void ADS12xx_Start_Set(uint8_t level){PBout(15)=level;}
static uint8_t ADS12xx_DRDY1_Read(void){return PCin(13);}
static uint8_t ADS12xx_DRDY2_Read(void){return PCin(1);}
static uint8_t ADS12xx_DRDY3_Read(void){return PCin(3);}
static uint8_t ADS12xx_DRDY4_Read(void){return PCin(5);}
static uint8_t ADS12xx_DRDY5_Read(void){return PBin(1);}
static void ADS12xx_MOSI_Set(uint8_t level){PCout(7)=level;}
static uint8_t ADS12xx_MISO_Set(){return PAin(11);}
static void ADS12xx_SCK_Set(uint8_t level){PDout(13)=level;}
static void ADS12xx_CS1_Set(uint8_t level){PCout(0)=level;}
static void ADS12xx_CS2_Set(uint8_t level){PCout(2)=level;}
static void ADS12xx_CS3_Set(uint8_t level){PAout(1)=level;}
static void ADS12xx_CS4_Set(uint8_t level){PAout(4)=level;}
static void ADS12xx_CS5_Set(uint8_t level){PBout(0)=level;}

static void ADS12xx_GPIO_Init(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_0;//ADS_RESET  ADS_START ADS_CS5
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_0|GPIO_Pin_2;//ADS_MOSI  ADS_CS1 ADS_CS2
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;//ADS_SCLK
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_4;//ADS_CS3  ADS_CS4
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_5;//ADS_DRDY_1 ADS_DRDY2 ADS_DRDY3 ADS_DRDY4
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
    
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//ADS_MISO
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOA,&GPIO_InitStructure);     
        
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//ADS_DRDY5
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_Init(GPIOB,&GPIO_InitStructure);           
        
    ADS12xx_CS1_Set(1);    
    ADS12xx_CS2_Set(1);   
    ADS12xx_CS3_Set(1);   
    ADS12xx_CS4_Set(1);   
    ADS12xx_CS5_Set(1);   
}

static void ADS12xx_SPI_Delay(){
    Delay_us(10);
}

static void ADS12xx_Delay_ms(uint16_t ms){
    Delay_ms(ms);
}

ads12xx_drv ads12xx[5];
void ADS12xx_Init(){
    uint8_t i;
    
    ADS12xx_GPIO_Init();

    for(i=0;i<5;i++){
        skySoft_ADS12xx_DeInit(&ads12xx[i]);
        ads12xx[i].adap.mosi_set=ADS12xx_MOSI_Set;
        ads12xx[i].adap.miso_read=ADS12xx_MISO_Set;
        ads12xx[i].adap.sck_set=ADS12xx_SCK_Set;
        ads12xx[i].adap.delay=ADS12xx_SPI_Delay;        
        
    }

    ads12xx[0].rst_set=ADS12xx_Rst_Set;
    ads12xx[0].start_set=ADS12xx_Start_Set;
    ads12xx[0].delayms=ADS12xx_Delay_ms;
    
    ads12xx[0].adap.cs_set=ADS12xx_CS1_Set;
    ads12xx[1].adap.cs_set=ADS12xx_CS2_Set;
    ads12xx[2].adap.cs_set=ADS12xx_CS3_Set;
    ads12xx[3].adap.cs_set=ADS12xx_CS4_Set;
    ads12xx[4].adap.cs_set=ADS12xx_CS5_Set;

    ads12xx[0].drdy_read=ADS12xx_DRDY1_Read;
    ads12xx[1].drdy_read=ADS12xx_DRDY2_Read;
    ads12xx[2].drdy_read=ADS12xx_DRDY3_Read;
    ads12xx[3].drdy_read=ADS12xx_DRDY4_Read;
    ads12xx[4].drdy_read=ADS12xx_DRDY5_Read;

    for(i=0;i<5;i++){
        skySoft_ADS12xx_Init(&ads12xx[i]);
		
    }  
    
}

void ADS1259_RdADC(uint8_t index,uint32_t *ADC_dat){
    skySoft_ADS1259_RdADC(&ads12xx[index],ADC_dat);
}
