#ifndef __BSP_KEY_H_
#define __BSP_KEY_H_

#include "AllHeader.h"

#define KEY_PRESSED     0
#define KEY_RELEASED    1

uint8_t KEY1_IsPressed(void);
uint8_t KEY2_IsPressed(void);
uint8_t KEY3_IsPressed(void);

uint8_t KEY_Scan(void);

uint8_t KEY_Read(void);

void LED_Toggle(void);

void KEY_IRQ_Handler(void);

#endif
