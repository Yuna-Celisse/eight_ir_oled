#include "AllHeader.h"

// 全局变量定义
void Beep_Times(uint8_t times);
void Display_Time(void);
void Display_Countdown(uint8_t seconds);
void Check_Wide_Line(void);
void RGB_Running_Effect(void);

int direct;
volatile bool left_forward = 0;
volatile bool right_forward = 0;
int left_speed = 0;
int right_speed = 0;

int main(void)
{
	// 系统配置初始化  System configuration initialization
	SYSCFG_DL_init();       
	 // OLED显示屏初始化  OLED display initialization
	OLED_Init();            
	 // 串口通信初始化  USART communication initialization	
	USART_Init();           
     // 电机PWM初始化  Motor PWM initialization
	Init_Motor_PWM();
   //初始化电机编码器
  encoder_init(); 
//  电机控速pid2初始化
 // PID_Param_Init();
  
  // 初始显示
  OLED_Clear();
 // OLED_ShowString(0, 0, (uint8_t*)"Track Monitor", 8, 1);
  OLED_Refresh();

	uart0_send_string("$0,0,1#");
	
	// 启动电机控制 - 现在使用PID控制，注释掉直接控制
	// 参数：左轮方向(true=前进), 左轮速度(0-10000), 右轮方向, 右轮速度
	// Motor_Set(true, 5000, true, 5000);  // 两轮都以50%速度前进
	
	while (1)                
	{
		
		// 读取红外传感器数据
		static u8 x1,x2,x3,x4,x5,x6,x7,x8;
		deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
		 
//		track_err = track_read(x1,x2,x3,x4,x5,x6,x7,x8); 
		direct = Direct_Read(x1,x2,x3,x4,x5,x6,x7,x8);
		
		// 显示循迹传感器状态（第一行：传感器1-4）
		OLED_ShowString(0, 16, (uint8_t*)"IR:", 8, 1);
		OLED_ShowNum(24, 16, x1, 1, 8, 1);
		OLED_ShowNum(32, 16, x2, 1, 8, 1);
		OLED_ShowNum(40, 16, x3, 1, 8, 1);
		OLED_ShowNum(48, 16, x4, 1, 8, 1);
		OLED_ShowNum(56, 16, x5, 1, 8, 1);
		OLED_ShowNum(64, 16, x6, 1, 8, 1);
		OLED_ShowNum(72, 16, x7, 1, 8, 1);
		OLED_ShowNum(80, 16, x8, 1, 8, 1);
		
//		// 显示循迹误差值
//		OLED_ShowString(0, 32, (uint8_t*)"Err:", 8, 1);
//		if(track_err >= 0) {
//			OLED_ShowString(32, 32, (uint8_t*)"+", 8, 1);
//			OLED_ShowNum(40, 32, track_err, 3, 8, 1);
//		} else {
//			OLED_ShowString(32, 32, (uint8_t*)"-", 8, 1);
//			OLED_ShowNum(40, 32, -track_err, 3, 8, 1);
//		}
		
		// 显示 correction 和 direct
//		OLED_ShowString(0, 0, (uint8_t*)"Cor:", 8, 1);
//		OLED_ShowNum(32, 0, (int16_t)correction, 3, 8, 1);
//		OLED_ShowString(64, 0, (uint8_t*)"D:", 8, 1);
//		OLED_ShowNum(80, 0, direct, 2, 8, 1);
		
		
		// 显示编码器值
		OLED_ShowString(88, 32, (uint8_t*)"L:", 8, 1);
		OLED_ShowNum(104, 32, motorL_encoder.count, 3, 8, 1);
		OLED_ShowString(88, 48, (uint8_t*)"R:", 8, 1);
		OLED_ShowNum(104, 48, motorR_encoder.count, 3, 8, 1);
		
		// 刷新OLED显示
		OLED_Refresh();
		
		if (direct == 0)
 		{
 			// direct为0：根据上一次的趋势继续转向
 			if (last_target_L > last_target_R)  // 左转趋势
 			{
				Motor_Set(left_forward = 0, 2000, right_forward = 1, 2000);
 			}
 			else if(last_target_L < last_target_R)// 右转趋势
 			{
				Motor_Set(left_forward = 1, 2000, right_forward = 0, 2000);
 			}
//			else  // 没有趋势时，直行
//			{
//				Motor_Set(1, 2500, 1, 2500);
//			}
 		}
 		else
 		{ 
		 setspeed_pid(&left_speed, &right_speed);
		 // 使用PID计算出的速度驱动电机
		 Motor_Set(1, left_speed, 1, right_speed);
		}
		last_target_L = left_speed;
		last_target_R = right_speed;
		 
			
//		 Motor_Set(left_forward = 1, SPEED, right_forward = 1, SPEED);
//		 target_L = SPEED - correction;
//     target_R = SPEED + correction;
//		 // 限幅
//		 if (target_L < 0) target_L = 0;
//		 if (target_R < 0) target_R = 0;
//		 if (target_L > 68) target_L = 68;
//		 if (target_R > 68) target_R = 68;		
//    }
//		last_target_L = target_L;
//		last_target_R = target_R;
	}
}
