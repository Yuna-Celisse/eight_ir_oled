#ifndef __APP_RGB_H_
#define __APP_RGB_H_

#include "AllHeader.h"

enum RGB_Light
{
    Left_RGB = 0,
    Right_RGB
};


typedef enum RGB_Color
{
    Red_RGB,
    Green_RGB,
    Blue_RGB,
    Yellow_RGB,
    Purple_RGB,    
    Cyan_RGB,
    White_RGB,
    OFF    
}RGB_Color_t;

//void Control_RGB_ALL();
void Control_RGB_ALL(RGB_Color_t color);
void app_color();
void RGB_LineTracking_Control(uint8_t line_detected);
#endif


