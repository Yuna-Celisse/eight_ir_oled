#ifndef _APP_IRTRACKING_H_
#define _APP_IRTRACKING_H_


#include "AllHeader.h"
#include "get_mpu6050.h"  // 新增：引入MPU6050头文件

#define BLACK       1        //黑线black
#define WHITE       0        //白线white


#define u8 uint8_t
#define u16 uint16_t

// 新增：陀螺仪增稳相关定义
// 如果代码大小超限，可以设置为0禁用MPU6050增稳功能
#define YAW_COMPENSATION_ENABLE  1  // 1=启用yaw补偿，0=禁用
#define YAW_KP  2.0f                // yaw角P系数
#define YAW_KD  4.0f                // yaw角D系数

void LineWalking(void);
int LineCheck(void);
u8 TurnControl(u8 x1, u8 x2, u8 x7, u8 x8);

// 新增：陀螺仪增稳相关函数
void IRTracking_MPU_Init(void);     // MPU6050初始化
float Get_Yaw_Compensation(void);   // 获取yaw补偿值

#endif

