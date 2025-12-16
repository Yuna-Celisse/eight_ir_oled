#include "app_rgb.h"

//RGB简单灯效

// RGB功能需要额外的驱动文件支持
// 如果没有RGB硬件或驱动，可以从项目中移除此文件

// 颜色定义（如果有RGB驱动，这些应该在驱动头文件中定义）
#define RED     0xFF0000
#define GREEN   0x00FF00
#define BLUE    0x0000FF
#define YELLOW  0xFFFF00
#define PURPLE  0xFF00FF
#define CYAN    0x00FFFF
#define BLACK   0x000000

// 弱函数声明 - 如果没有实际的RGB驱动，这些函数不会做任何事
__attribute__((weak)) void rgb_SetColor(unsigned char LedId, unsigned long color) {
    // 空实现，如果没有RGB驱动
    (void)LedId;
    (void)color;
}

__attribute__((weak)) void rgb_SendArray(void) {
    // 空实现，如果没有RGB驱动
}

extern uint8_t ProtocolString[80];//引入备份数据区 Introducing backup data area
static void set_ALL_RGB_COLOR(unsigned long color)
{
    rgb_SetColor(Left_RGB,color);
    rgb_SetColor(Right_RGB,color);
}


void app_color()
{
	
		if (ProtocolString[8] == '2')      //小车左旋 Left rotation of the car
	{
		 Control_RGB_ALL(Red_RGB);
	}
 else if (ProtocolString[8] == '3') //小车右旋 Car turning right
	{
		Control_RGB_ALL(Green_RGB);
	}
	else if (ProtocolString[8] == '4')
	{
		Control_RGB_ALL(Blue_RGB);
	}
else {

	Control_RGB_ALL(OFF);


}



}

void Control_RGB_ALL(RGB_Color_t color)
{
    switch(color)
    {
        case    Red_RGB:     set_ALL_RGB_COLOR(RED);break;
        case    Green_RGB:   set_ALL_RGB_COLOR(GREEN);break;
        case    Blue_RGB:    set_ALL_RGB_COLOR(BLUE);break;
        case    Yellow_RGB:  set_ALL_RGB_COLOR(YELLOW);break;
        case    Purple_RGB:  set_ALL_RGB_COLOR(PURPLE); break;  
        case    Cyan_RGB:    set_ALL_RGB_COLOR(CYAN);break;
        case    OFF  :       set_ALL_RGB_COLOR(BLACK);break;
        
        default : return;
        
    }
    
    rgb_SendArray();//必须发送,才显示
    delay_ms(200);
    
    
}

