#include "app_rgb.h"
#include "bsp_RGB.h"

// 外部数组声明
extern unsigned char LedsArray[];

// 定义ProtocolString数组
uint8_t ProtocolString[80] = {0};

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
        case OFF:
            for(int i = 0; i < WS2812_MAX * 3; i++) {
                LedsArray[i] = 0;
            }
            break;
        case Red_RGB:     set_ALL_RGB_COLOR(RED);break;
        case Green_RGB:   set_ALL_RGB_COLOR(GREEN);break;
        case Blue_RGB:    set_ALL_RGB_COLOR(BLUE);break;
        case Yellow_RGB:  set_ALL_RGB_COLOR(YELLOW);break;
        case Purple_RGB:  set_ALL_RGB_COLOR(PURPLE);break;  
        case Cyan_RGB:    set_ALL_RGB_COLOR(CYAN);break;
        case White_RGB:   set_ALL_RGB_COLOR(RGB_WHITE);break;
        default: return;
    }
    rgb_SendArray();
}

void RGB_LineTracking_Control(uint8_t line_detected)
{
    Control_RGB_ALL(line_detected ? Red_RGB : Blue_RGB);
}

