#ifndef __BSP_MOTOR_H_
#define __BSP_MOTOR_H_


#include "AllHeader.h"

typedef enum
{
	MOTOR_ID_M1 = 0,
	MOTOR_ID_M2,
	MAX_MOTOR
} Motor_ID;


void Init_Motor_PWM(void);
void Motor_Set(bool left_forward, uint16_t left_speed, bool right_forward, uint16_t right_speed);
//void Motor_Stop(uint8_t brake);
#endif

