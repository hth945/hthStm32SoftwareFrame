#ifndef _PUBLIC_LCD_H__
#define	_PUBLIC_LCD_H__

#include "stdint.h"


typedef struct{
    uint16_t real_width;//��ʵ���
    uint16_t real_high; //��ʵ�߶� 

    void (* lcd_setpoint)(void*,uint16_t,uint16_t,uint32_t);//Ҫʵ�ֵĴ�㺯��  ��Ҫ һ�к���ʵ�ֵ���Դ
    uint32_t (* lcd_getpoint)(void*,uint16_t,uint16_t);//��ȡ��ɫ �Ǳ�Ҫ
    void (* lcd_setwindow)(void*,uint16_t,uint16_t,uint16_t,uint16_t);//�Ǳ�Ҫ
    void (* lcd_rgbdat)(void*,uint32_t);//Ҫʹ���������ʵ��lcd_setwindow����  �Ǳ�Ҫ
}lcd_interface;

typedef enum{
    Normal_Mode  = 0x00,//���ñ���ɫ���Ǵ��ֵ�����
    Cover_Mode   = 0x01,//�ص�ģʽ ����ԭ�е�ͼ�ϴ��
    NumZero_Mode = 0x02,//���ֲ�0ģʽ
}Show_Mode;

typedef enum{
    Font_12  = 12,
    Font_16  = 16,
    Font_24  = 24,
}Font_Size;

typedef struct{
    uint16_t virtual_width;//�����Ĵ�С
    uint16_t virtual_high;
    uint16_t x_offset;//����ʵ��Ļ�ϵ�λ��
    uint16_t y_offset;
    
    uint32_t front_color;//ǰ��ɫ
    uint32_t back_color;//����ɫ
    Font_Size fontsize;
    Show_Mode mode;
    
    lcd_interface *drive;//��ʵ��Ļ�Ľӿ�
}lcd_adapter;



//RGB565 ������ɫ
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
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY       	  	 0X8430 //��ɫ

//��ʼ��
void P_LCD_Drv_DeInit(lcd_interface *drv);
void P_LCD_Drv(lcd_interface *drv);
void P_LCD_Adap_DeInit(lcd_adapter *adap);
void P_LCD_Adap(lcd_adapter *adap);

//��������
int P_LCD_Point(lcd_adapter *adap,uint16_t x,uint16_t y,uint32_t color);
int P_LCD_SetWindow(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
int P_LCD_RGBDat(lcd_adapter *adap,uint32_t color);

//��ͼ����
void P_LCD_Clr(lcd_adapter *adap,uint32_t color);//����
void P_LCD_Box(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t color);//����
void P_LCD_Line(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t color);//����
void P_LCD_DrawCircle(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t r,uint32_t rgb);//��԰
void P_LCD_DrawRectangle(lcd_adapter *adap,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint32_t rgb);//������
void P_LCD_Pic(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t XSize,uint16_t YSize,uint16_t offset,uint8_t *pic);//��ͼ

//�ַ���ʾ����
void P_LCD_OneChar(lcd_adapter *adap,uint16_t x,uint16_t y,uint16_t chr);
void P_LCD_String(lcd_adapter *adap,uint16_t x,uint16_t y,char *str);//��ʾ�ַ���	  
void P_LCD_ShowHex(lcd_adapter *adap,uint16_t x,uint16_t y,uint8_t len,uint32_t val);//��ʾHex
void P_LCD_Num(lcd_adapter *adap,uint16_t x,uint16_t y,uint8_t len,uint32_t num);//��ʾ����



#endif
