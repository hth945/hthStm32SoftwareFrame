#ifndef _PUBLIC_LCD_H__
#define	_PUBLIC_LCD_H__

#include "stdint.h"


typedef struct{
    uint16_t real_width;//真实宽度
    uint16_t real_high; //真实高度 

    void (* lcd_setpoint)(void*,uint16_t,uint16_t,uint32_t);//要实现的打点函数  必要 一切函数实现的来源
    uint32_t (* lcd_getpoint)(void*,uint16_t,uint16_t);//换取颜色 非必要
    void (* lcd_setwindow)(void*,uint16_t,uint16_t,uint16_t,uint16_t);//非必要
    void (* lcd_rgbdat)(void*,uint32_t);//要使用这个必须实现lcd_setwindow函数  非必要
}lcd_interface;

typedef enum{
    Normal_Mode  = 0x00,//会用背景色覆盖打字的区域
    Cover_Mode   = 0x01,//重叠模式 会在原有的图上打点
    NumZero_Mode = 0x02,//数字补0模式
}Show_Mode;

typedef enum{
    Font_12  = 12,
    Font_16  = 16,
    Font_24  = 24,
}Font_Size;

typedef struct{
    uint16_t virtual_width;//虚拟框的大小
    uint16_t virtual_high;
    uint16_t x_offset;//在真实屏幕上的位置
    uint16_t y_offset;
    
    uint32_t front_color;//前景色
    uint32_t back_color;//背景色
    Font_Size fontsize;
    Show_Mode mode;
    
    lcd_interface *drive;//真实屏幕的接口
}lcd_adapter;



//RGB565 常用颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	 
#define RED           	 0xF800
#define GREEN         	 0x07E0
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define MAGENTA       	 0xF81F
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY       	  	 0X8430 //灰色

//初始化
void P_LCD_Drv_DeInit(lcd_interface *drv);
void P_LCD_Drv(lcd_interface *drv);
void P_LCD_Adap_DeInit(lcd_adapter *adap);
void P_LCD_Adap(lcd_adapter *adap);

//基础函数
int P_LCD_Point(lcd_adapter *adap,uint16_t x,uint16_t y,uint32_t color);
int P_LCD_SetWindow(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
int P_LCD_RGBDat(lcd_adapter *adap,uint32_t color);

//绘图函数
void P_LCD_Clr(lcd_adapter *adap,uint32_t color);//清屏
void P_LCD_Box(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t color);//画块
void P_LCD_Line(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t color);//划线
void P_LCD_DrawCircle(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t r,uint32_t rgb);//画园
void P_LCD_DrawRectangle(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t rgb);//画矩形
void P_LCD_Pic(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t XSize,uint16_t YSize,uint16_t offset,uint8_t *pic);//画图

//字符显示函数
void P_LCD_OneChar(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t chr);
void P_LCD_String(lcd_adapter *adap,uint16_t x,uint16_t y,char *str);//显示字符串	  
void P_LCD_ShowHex(lcd_adapter *adap,uint16_t x,uint16_t y,uint8_t len,uint32_t val);//显示Hex
void P_LCD_Num(lcd_adapter *adap,uint16_t x,uint16_t y,uint8_t len,uint32_t num);//显示数字



#endif
