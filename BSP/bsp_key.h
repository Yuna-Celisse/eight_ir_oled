#ifndef __BSP_KEY_H_
#define __BSP_KEY_H_

#include "AllHeader.h"

// 按键状态定义
#define KEY_PRESSED     0  // 按下状态（低电平）
#define KEY_RELEASED    1  // 释放状态（高电平）

// 按键检测函数（检测按键是否被按下）
uint8_t KEY1_IsPressed(void);
uint8_t KEY2_IsPressed(void);
uint8_t KEY3_IsPressed(void);

// 按键扫描函数（轮询方式）
uint8_t KEY_Scan(void);

// 按键读取函数
uint8_t KEY_Read(void);

// LED翻转函数
void LED_Toggle(void);

// 按键中断处理函数（在GROUP1_IRQHandler中调用）
void KEY_IRQ_Handler(void);

#endif
