#ifndef __ALLHEADER_H_
#define __ALLHEADER_H_

#define Motor_Switch 1
#define u8  uint8_t
#define u16 uint16_t 
#define u32 uint32_t 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "usart0.h"

#include "bsp_beep_led.h"
#include "bsp_ir.h"
#include "bsp_key.h"
#include "oled.h"
#include "config.h"

#include "bsp_encoder.h"
#include "bsp_timer.h"
#include "bsp_motor.h"
#include "bsp_RGB.h"

#include "app_motor.h"
#include "bsp_PID_motor.h"
#include "app_irtracking_eight.h"
#include "app_rgb.h"

// MPU6050陀螺仪相关
#include "Kalman.h"
#include "Filter.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "bsp_mpu6050.h"
#include "get_mpu6050.h"


#endif

