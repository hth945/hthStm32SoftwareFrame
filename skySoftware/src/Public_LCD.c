//V0.0.2
//1.修复打字函数会多打边框的bug
//2.添加打字函数以画框的形式实现
//V0.0.1
//1.建立

#include "public_lcd.h"
#include "stdio.h"
#include "font.h"

__weak void PP_LCD_SetPoint(lcd_interface *drv,uint16_t x,uint16_t y,uint32_t color);
__weak uint32_t PP_LCD_GetPoint(lcd_interface *drv,uint16_t x,uint16_t y){return 0;};
__weak void PP_LCD_SetWindow(lcd_interface *drv,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
__weak void PP_LCD_RGBDat(lcd_interface *drv,uint32_t color);

void P_LCD_Adap_DeInit(lcd_adapter *adap){
    adap->virtual_width=100;
    adap->virtual_high=100;
    adap->x_offset=0;
    adap->y_offset=0;
    
    adap->front_color=BLACK;
    adap->back_color=WHITE;
    adap->fontsize=Font_16;
    adap->mode=Normal_Mode;
}

void P_LCD_Drv_DeInit(lcd_interface *drv){
    drv->real_width=100;
    drv->real_high=100;    
    drv->lcd_setpoint=(void (*)(void*,uint16_t,uint16_t,uint32_t))PP_LCD_SetPoint;
    drv->lcd_getpoint=(uint32_t (*)(void*,uint16_t,uint16_t))PP_LCD_GetPoint;
    drv->lcd_setwindow=(void (*)(void*,uint16_t,uint16_t,uint16_t,uint16_t))PP_LCD_SetWindow;
    drv->lcd_rgbdat=(void (*)(void*,uint32_t))PP_LCD_RGBDat;
}

void P_LCD_Adap(lcd_adapter *adap){
    
}

void P_LCD_Drv(lcd_interface *drv){

}

//------------------------
__weak void PP_LCD_SetPoint(lcd_interface *drv,uint16_t x,uint16_t y,uint32_t color){

}

uint16_t pp_x=0,pp_y=0;
uint16_t pp_x2=0;
uint16_t pp_x1=0;
__weak void PP_LCD_SetWindow(lcd_interface *drv,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2){
    pp_x1=x1;
    pp_x2=x2;
    pp_x=x1;
    pp_y=y1;
}

__weak void PP_LCD_RGBDat(lcd_interface *drv,uint32_t color){
    drv->lcd_setpoint(drv,pp_x,pp_y,color);
    pp_x++;
    if(pp_x>pp_x2){
        pp_y++;
        pp_x=pp_x1;
    }
}

//------------------------
//基本函数
static uint8_t SetWindow_flag=0;
int P_LCD_Point(lcd_adapter *adap,uint16_t x,uint16_t y,uint32_t color){
    uint16_t p_x,p_y;
    
    if(x>(adap->virtual_width-1)) return -1;
    if(y>(adap->virtual_high-1)) return -1;
    
    p_x=x+adap->x_offset;
    p_y=y+adap->y_offset;
    
    if(p_x>(adap->drive->real_width-1)) return -1;
    if(p_y>(adap->drive->real_high-1)) return -1;
    
    adap->drive->lcd_setpoint(adap->drive,p_x,p_y,color);
    
    return 0;
}

int P_LCD_SetWindow(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2){
    uint16_t p_x1,p_y1;
    uint16_t p_x2,p_y2;
    
    SetWindow_flag=0;
    
    if(x1>x2) return -1;
    if(y1>y2) return -1;
    
    if(x1>(adap->virtual_width-1)) return -1;
    if(y1>(adap->virtual_high-1)) return -1;
    if(x2>(adap->virtual_width-1)) return -1;
    if(y2>(adap->virtual_high-1)) return -1;
    
    p_x1=x1+adap->x_offset;
    p_y1=y1+adap->y_offset;
    p_x2=x2+adap->x_offset;
    p_y2=y2+adap->y_offset;
    
    if(p_x1>(adap->drive->real_width-1)) return -1;
    if(p_y1>(adap->drive->real_high-1)) return -1;
    if(p_x2>(adap->drive->real_width-1)) return -1;
    if(p_y2>(adap->drive->real_high-1)) return -1; 
    
    SetWindow_flag=1;
    
    adap->drive->lcd_setwindow(adap->drive,p_x1,p_y1,p_x2,p_y2);
    
    return 0;
}

int P_LCD_RGBDat(lcd_adapter *adap,uint32_t color){
    if(SetWindow_flag){//开窗成功才能发送数据
        adap->drive->lcd_rgbdat(adap->drive,color);
        return 0;
    }else{
        return -1;
    }
}

//-------------------------------------------------------------

void P_LCD_Box(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t color){
    uint32_t i;
    
	P_LCD_SetWindow(adap,x1,y1,x2,y2);
	for (i=0; i<(x2-x1+1)*(y2-y1+1); i++){
		P_LCD_RGBDat(adap,color);
	}  
}

void P_LCD_Clr(lcd_adapter *adap,uint32_t color){
    P_LCD_Box(adap,0,0,adap->virtual_width-1,adap->virtual_high-1,color);
}

void P_LCD_Line(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t color){
	uint32_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)
		incx=1; //设置单步方向 
	else 
		if(delta_x==0)incx=0;//垂直线 
	else 
		{incx=-1;delta_x=-delta_x;} 
		
	if(delta_y>0)
		incy=1; 
	else 
		if(delta_y==0)incy=0;//水平线 
	else
		{incy=-1;delta_y=-delta_y;} 
		
	if(delta_x>delta_y)
		distance=delta_x; //选取基本增量坐标轴 
	else 
		distance=delta_y; 
	
	for(t=0;t<=distance+1;t++ ){ //画线输出 
        P_LCD_Point(adap,uRow,uCol,color);//画点
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void P_LCD_DrawCircle(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t r,uint32_t rgb)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b){
        P_LCD_Point(adap,x+b,y-a,rgb);//0  
        P_LCD_Point(adap,x-b,y+a,rgb);//4
        P_LCD_Point(adap,x+a,y-b,rgb);//1   
        P_LCD_Point(adap,x-a,y+b,rgb);//5  
        P_LCD_Point(adap,x-a,y-b,rgb);//2    
        P_LCD_Point(adap,x+a,y+b,rgb);//6 
		P_LCD_Point(adap,x-b,y-a,rgb);//3          
		P_LCD_Point(adap,x+b,y+a,rgb);//7 

		a++;
		//使用Bresenham算法画圆     
		if(di<0)
            di +=4*a+6;	  
		else{
			di+=10+4*(a-b);   
			b--;
		} 
	}
}		

//画矩形
void P_LCD_DrawRectangle(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t rgb){
	P_LCD_Line(adap,x1,y1,x2,y1,rgb);
	P_LCD_Line(adap,x1,y1,x1,y2,rgb);
	P_LCD_Line(adap,x1,y2,x2,y2,rgb);
	P_LCD_Line(adap,x2,y1,x2,y2,rgb);
}

//显示图片
void P_LCD_Pic(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t XSize,uint16_t YSize,uint16_t offset,uint8_t *pic){
    uint32_t i;

    i=XSize*YSize;
    pic+=offset;
    P_LCD_SetWindow(adap,x,y,x+XSize-1,y+YSize-1);

    do{
        P_LCD_RGBDat(adap,0xffff&(*(uint16_t *)pic));
        pic+=2;//RGB565两个字节
    }while(i--);

}

//显示一个字符
void P_LCD_OneChar(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t chr){
    uint8_t i,x1,y1;
    uint8_t x_pixsum;
    uint8_t temp;
    uint16_t index;
    uint8_t t=0;
    unsigned char *msk=NULL;
    struct typFNT_GB16 *type16;
    struct typFNT_GB24 *type24;
    
    uint8_t x_bsize=0;
    
    if(chr>=0x20 && chr<=0x7e){//ascii
        x_bsize=adap->fontsize/16+(((adap->fontsize%16)==0)?0:1);//不能被16整除+1
        x_pixsum=adap->fontsize/2;
        
        chr=chr-' ';//' '表示0x20,即是字库的开头
        chr=x_bsize*(adap->fontsize)*chr;//得到实际起始
        if(adap->fontsize==Font_12)msk=(unsigned char *)&asc2_12[chr]; 	//调用12字体
        else if(adap->fontsize==Font_16)msk=(unsigned char *)&asc2_16[chr];	//调用16字体
        else if(adap->fontsize==Font_24)msk=(unsigned char *)&asc2_24[chr];	//调用24字体
        else return;//没有的字库        
    }else{//汉字
        x_bsize=adap->fontsize/8+(((adap->fontsize%8)==0)?0:1);//不能被8整除+1
        x_pixsum=adap->fontsize;
        
        if(adap->fontsize==Font_16){
            type16=(struct typFNT_GB16*)GB_16;
            for(i=0;i<sizeof(GB_16)/sizeof(struct typFNT_GB16);i++){//搜索有无该字
                index=type16->Index[0];index<<=8;//拼接index
                index|=0xff&type16->Index[1];                
                if(chr==index){
                    msk=(unsigned char *)type16->Msk;//获取到字体位置
                    break;
                }
                type16++;
            }
        }else if(adap->fontsize==Font_24){
            type24=(struct typFNT_GB24*)GB_24;
            for(i=0;i<sizeof(GB_24)/sizeof(struct typFNT_GB24);i++){
                index=type24->Index[0];index<<=8;
                index|=0xff&type24->Index[1];                    
                if(chr==index){
                    msk=(unsigned char *)type24->Msk;
                    break;
                }
                type24++;
            }        
        }
    }
    
    if((adap->mode&0x01)==Normal_Mode){//普通模式 能画框
        P_LCD_SetWindow(adap,x,y,x+x_pixsum-1,y+adap->fontsize-1);
    }
    for(y1=0;y1<adap->fontsize;y1++){
        for(x1=0;x1<x_pixsum;x1++){
            if(x1%8==0){
                if(msk!=NULL){
                    temp=msk[t];
                }else{
                    temp=0x55;
                } 
                t++;
            }
            
            if(temp&0x80){   
                if((adap->mode&0x01)==Normal_Mode){//普通模式 画框形式写数据
                    P_LCD_RGBDat(adap,adap->front_color);
                }else{//覆盖模式  只能以打点的形式实现
                    P_LCD_Point(adap,x+x1,y+y1,adap->front_color);
                }    
            }else{
                if((adap->mode&0x01)==Normal_Mode){//普通模式 画框形式写数据
                    P_LCD_RGBDat(adap,adap->back_color);
                }
            }
            temp<<=1;
        }
    }   
}

//显示字符串	  
void P_LCD_String(lcd_adapter *adap,uint16_t x,uint16_t y,char *str){         
    while(*str!=0){
        if((uint16_t)*str>0x80){
            P_LCD_OneChar(adap,x,y,((uint16_t)*str<<8)|*(str+1));
            x+=adap->fontsize;
            str+=2; 
        }else{//ascii
            P_LCD_OneChar(adap,x,y,*str);
            x+=adap->fontsize/2;
            str++; 
        }
    }    
}

static const unsigned char p_ascii[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46};
static void P_LCD_AddHex(uint32_t v, uint8_t Len, char**ps) {
    char *s = *ps;
    
    if (Len > 8) {
        return;
    }
    
    (*ps) += Len;
    **ps   = '\0';    
    while(Len--) {
        *(s + Len) = p_ascii[v&0xf];
        v >>= 4;
    }
}

void P_LCD_ShowHex(lcd_adapter *adap,uint16_t x,uint16_t y,uint8_t len,uint32_t val){
    char ac[9];
    char* s = ac;
    
    P_LCD_AddHex(val, len, &s);
    P_LCD_String(adap,x,y,(char *)ac);
}

//m^n函数
//返回值:m^n次方.
static uint32_t P_LCD_Pow(uint8_t m,uint8_t n){
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	

void P_LCD_Num(lcd_adapter *adap,uint16_t x,uint16_t y,uint8_t len,uint32_t num){         	
	uint8_t i,temp;
	uint8_t enshow=0;	
    
	for(i=0;i<len;i++){
		temp=(num/P_LCD_Pow(10,len-i-1))%10;
		if(enshow==0 && i<(len-1)){
			if(temp==0){
                if((adap->mode&0x02)==NumZero_Mode){
                    P_LCD_OneChar(adap,x+(adap->fontsize/2)*i,y,'0');
				}else{
                    P_LCD_OneChar(adap,x+(adap->fontsize/2)*i,y,' ');
                }
                continue;
			}else{
                enshow=1; 
            }
		}
	 	P_LCD_OneChar(adap,x+(adap->fontsize/2)*i,y,temp+'0'); 
	}
} 


