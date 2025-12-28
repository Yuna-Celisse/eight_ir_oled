#ifndef __CONGIF_H_
#define __CONGIF_H_


#include "AllHeader.h"


#define COMPILE_ENT 0  //编译环境,1:CCS 0:keil  此处和printf相关环境不对,无法打印

#define IRRMOTE (0) //红外遥控打开开关

// IR Track GPIO定义 (根据实际硬件配置修改)
#define IR_Track_PORT       GPIOB
#define IR_Track_X1_PIN     DL_GPIO_PIN_0
#define IR_Track_X2_PIN     DL_GPIO_PIN_1
#define IR_Track_X3_PIN     DL_GPIO_PIN_2
#define IR_Track_X4_PIN     DL_GPIO_PIN_3





#endif

