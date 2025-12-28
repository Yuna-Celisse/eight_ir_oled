#include "AllHeader.h"

// 外部变量声明
extern unsigned char LedsArray[];

// 全局变量定义
void Beep_Times(uint8_t times);
void Display_Time(void);
void Display_Countdown(uint8_t seconds);
void Check_Wide_Line(void);
void RGB_Running_Effect(void);

// 按键启动控制相关变量
volatile uint8_t system_running = 0;  // 系统运行标志：0-停止，1-运行
// 移除了运行时间限制，可以无限制运行
extern volatile uint32_t run_time_ms; // 在bsp_timer.c中定义

int main(void)
{
	// 系统配置初始化  System configuration initialization
	SYSCFG_DL_init();
	
	// RGB引脚初始化 - 配置为输出模式（强推挽输出）
	// 初始化GPIOB PIN_10为输出
	DL_GPIO_initDigitalOutput(IOMUX_PINCM27);  // PIN_10对应PINCM27
	DL_GPIO_enableOutput(RGB_PORT, RGB_WQ2812_PIN);
	DL_GPIO_clearPins(RGB_PORT, RGB_WQ2812_PIN);
	
	// 简单延时等待稳定
	for(volatile int i=0; i<100000; i++);
	
	// RGB初始化测试 - 红绿测试
	Control_RGB_ALL(Red_RGB);
	delay_ms(200);
	Control_RGB_ALL(Green_RGB);
	delay_ms(200);
	Control_RGB_ALL(OFF);
	delay_ms(10);
	       
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
  OLED_ShowString(0, 0, (uint8_t*)"Press K1 Start", 8, 1);
  OLED_ShowString(0, 16, (uint8_t*)"Status: STOP", 8, 1);
  OLED_ShowString(0, 32, (uint8_t*)"GPIO:", 8, 1);
  OLED_ShowString(0, 48, (uint8_t*)"K1:", 8, 1);
  OLED_Refresh();

	uart0_send_string("$0,0,1#");
	
	// 启动电机控制 - 现在使用PID控制，注释掉直接控制
	// 参数：左轮方向(true=前进), 左轮速度(0-10000), 右轮方向, 右轮速度
	// Motor_Set(true, 5000, true, 5000);  // 两轮都以50%速度前进
	
	uint32_t loop_count = 0;
	
	while (1)                
	{
		loop_count++;
		
		// 显示GPIO原始值和按键状态用于调试
		uint32_t gpio_raw = DL_GPIO_readPins(KEY_PORT, KEY_K1_PIN);
		uint8_t raw_key = KEY_Read();
		
		if (system_running == 0)  // 只在停止状态刷新调试信息
		{
			OLED_ShowString(40, 32, (uint8_t*)"      ", 8, 1);
			OLED_ShowNum(40, 32, gpio_raw, 5, 8, 1);
			OLED_ShowString(32, 48, (uint8_t*)"   ", 8, 1);
			OLED_ShowNum(32, 48, raw_key, 1, 8, 1);
			OLED_Refresh();
			
			// 未启动时保持熄灭
			// RGB在按K1后才启动
		}
		
		// 检测K1按键
		uint8_t key_value = KEY_Scan();
		if (key_value)
		{
			if (system_running == 0)
			{
				// 启动系统
				system_running = 1;
				run_time_ms = 0;  // 重置计时器
				
				// 清除OLED显示
				OLED_Clear();
				OLED_ShowString(0, 0, (uint8_t*)"Running...", 8, 1);
				OLED_Refresh();
				
				// 蜂鸣器提示启动
				DL_GPIO_setPins(BEEP_PORT, BEEP_Buzzer_PIN);
				delay_ms(100);
				DL_GPIO_clearPins(BEEP_PORT, BEEP_Buzzer_PIN);
				
				// RGB启动提示 - 快速闪烁3次表示开始工作
				for(int i=0; i<3; i++) {
					Control_RGB_ALL(Red_RGB);
					delay_ms(100);
					Control_RGB_ALL(OFF);
					delay_ms(100);
				}
			}
			else
			{
				// 停止系统
				system_running = 0;
				Motor_Set(0, 0, 0, 0);  // 停止电机
				
				// 关闭RGB
				Control_RGB_ALL(OFF);
				
				OLED_Clear();
				OLED_ShowString(0, 0, (uint8_t*)"Press K1 Start", 8, 1);
				OLED_ShowString(0, 16, (uint8_t*)"Status: STOP", 8, 1);
				OLED_Refresh();
			}
			delay_ms(200);  // 按键后延时，避免重复触发
		}
		
		// 如果系统未运行，跳过后续处理
		if (system_running == 0)
		{
			delay_ms(10);  // 短暂延时，避免CPU占用过高
			continue;
		}
		
		// 读取红外传感器数据
		static u8 x1,x2,x3,x4,x5,x6,x7,x8;
		static u8 white_detected[8] = {0};  // 记录每个传感器是否检测到白线
		static u8 tick_count = 0;  // tick计数器
		static u8 is_on_white = 0;  // 当前是否在白线上
		static u8 white_line_count = 0;  // 检测到的白线条数
		static u8 line_detected = 0;  // 是否检测到黑线
		deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
//		track_err = track_read(x1,x2,x3,x4,x5,x6,x7,x8); 
//		direct = Direct_Read(x1,x2,x3,x4,x5,x6,x7,x8);
		
		// 黑底白线：检测是否全白（全白=1,1,1,1,1,1,1,1）
		// 全白时亮红灯，其他时候亮蓝灯
		line_detected = (x1 && x2 && x3 && x4 && x5 && x6 && x7 && x8);
		
		// 更新全局RGB状态，由定时器中断独立处理
		g_rgb_line_detected = line_detected;
		
		OLED_Refresh();
		
		// 在3个tick内累计检测白线
		if (x1 == 1) white_detected[0] = 1;
		if (x2 == 1) white_detected[1] = 1;
		if (x3 == 1) white_detected[2] = 1;
		if (x4 == 1) white_detected[3] = 1;
		if (x5 == 1) white_detected[4] = 1;
		if (x6 == 1) white_detected[5] = 1;
		if (x7 == 1) white_detected[6] = 1;
		if (x8 == 1) white_detected[7] = 1;
		
		tick_count++;
		
		// 检查是否所有传感器都检测到白线
		if (tick_count >= 3)
		{
			if (white_detected[0] && white_detected[1] && white_detected[2] && white_detected[3] &&
			    white_detected[4] && white_detected[5] && white_detected[6] && white_detected[7])
			{
				// 所有传感器都检测到白线
				if (is_on_white == 0)
				{
					// 从黑线到白线的转换，增加计数并触发蜂鸣器
					white_line_count++;
					// 根据白线条数响相应次数的蜂鸣器
					for (u8 i = 0; i < white_line_count; i++)
					{
						DL_GPIO_setPins(BEEP_PORT, BEEP_Buzzer_PIN);
						delay_ms(50);
						DL_GPIO_clearPins(BEEP_PORT, BEEP_Buzzer_PIN);
						if (i < white_line_count - 1)  // 不是最后一次时加间隔
						{
							delay_ms(100);  // 两次蜂鸣之间的间隔
						}
					}
				}
				is_on_white = 1;  // 标记为在白线上
			}
			else
			{
				// 检测到黑线，更新状态但不触发蜂鸣器
				is_on_white = 0;
			}
			// 重置检测状态
			tick_count = 0;
			for (int i = 0; i < 8; i++) white_detected[i] = 0;
		}
		
		// 正计时显示（从0开始往上计数）
		uint32_t sec = run_time_ms / 1000;
		uint32_t ms = run_time_ms % 1000;
		
		// 显示时间
		OLED_ShowString(0, 0, (uint8_t*)"T:", 8, 1);
		OLED_ShowNum(16, 0, sec, 2, 8, 1);
		OLED_ShowString(32, 0, (uint8_t*)".", 8, 1);
		OLED_ShowNum(40, 0, ms, 3, 8, 1);
		OLED_ShowString(64, 0, (uint8_t*)"s", 8, 1);
		
		// 显示循迹传感器状态
		OLED_ShowString(0, 16, (uint8_t*)"IR:", 8, 1);
		OLED_ShowNum(24, 16, x1, 1, 8, 1);
		OLED_ShowNum(32, 16, x2, 1, 8, 1);
		OLED_ShowNum(40, 16, x3, 1, 8, 1);
		OLED_ShowNum(48, 16, x4, 1, 8, 1);
		OLED_ShowNum(56, 16, x5, 1, 8, 1);
		OLED_ShowNum(64, 16, x6, 1, 8, 1);
		OLED_ShowNum(72, 16, x7, 1, 8, 1);
		OLED_ShowNum(80, 16, x8, 1, 8, 1);
		
		// 显示RGB状态
		OLED_ShowString(0, 32, (uint8_t*)"RGB:", 8, 1);
		if(line_detected) {
			OLED_ShowString(32, 32, (uint8_t*)"RED ", 8, 1);
		} else {
			OLED_ShowString(32, 32, (uint8_t*)"BLU ", 8, 1);
		}
		
		// 显示编码器值
		OLED_ShowString(88, 32, (uint8_t*)"L:", 8, 1);
		OLED_ShowNum(104, 32, motorL_encoder.count, 3, 8, 1);
		OLED_ShowString(88, 48, (uint8_t*)"R:", 8, 1);
		OLED_ShowNum(104, 48, motorR_encoder.count, 3, 8, 1);
		
		OLED_Refresh();
		
/*		
		// 显示时间（秒.毫秒）
		OLED_ShowString(0, 0, (uint8_t*)"T:", 8, 1);
		OLED_ShowNum(16, 0, sec, 2, 8, 1);
		OLED_ShowString(32, 0, (uint8_t*)".", 8, 1);
		OLED_ShowNum(40, 0, ms, 3, 8, 1);
		// RGB数据
		OLED_ShowString(72, 0, (uint8_t*)"G", 8, 1);
		OLED_ShowNum(80, 0, LedsArray[0], 2, 8, 1);
		OLED_ShowString(96, 0, (uint8_t*)"R", 8, 1);
		OLED_ShowNum(104, 0, LedsArray[1], 2, 8, 1);
		
		// 显示循迹传感器状态
		OLED_ShowString(0, 16, (uint8_t*)"IR:", 8, 1);
		OLED_ShowNum(24, 16, x1, 1, 8, 1);
		OLED_ShowNum(32, 16, x2, 1, 8, 1);
		OLED_ShowNum(40, 16, x3, 1, 8, 1);
		OLED_ShowNum(48, 16, x4, 1, 8, 1);
		OLED_ShowNum(56, 16, x5, 1, 8, 1);
		OLED_ShowNum(64, 16, x6, 1, 8, 1);
		OLED_ShowNum(72, 16, x7, 1, 8, 1);
		OLED_ShowNum(80, 16, x8, 1, 8, 1);
		// B值
		OLED_ShowString(96, 16, (uint8_t*)"B", 8, 1);
		OLED_ShowNum(104, 16, LedsArray[2], 2, 8, 1);
		
		// 显示RGB状态
		OLED_ShowString(0, 32, (uint8_t*)"RGB:", 8, 1);
		if(line_detected) {
			OLED_ShowString(32, 32, (uint8_t*)"RED ", 8, 1);
		} else {
			OLED_ShowString(32, 32, (uint8_t*)"GRN ", 8, 1);
		};
		
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
		
		// 显示编码器值 - 暂时关闭
//		OLED_ShowString(88, 32, (uint8_t*)"       ", 8, 1);  // 清除
//		OLED_ShowString(88, 32, (uint8_t*)"L:", 8, 1);
//		OLED_ShowNum(104, 32, motorL_encoder.count, 3, 8, 1);
//		OLED_ShowString(88, 48, (uint8_t*)"       ", 8, 1);  // 清除
//		OLED_ShowString(88, 48, (uint8_t*)"R:", 8, 1);
//		OLED_ShowNum(104, 32, motorL_encoder.count, 3, 8, 1);
//		OLED_ShowString(88, 48, (uint8_t*)"R:", 8, 1);
//		OLED_ShowNum(104, 48, motorR_encoder.count, 3, 8, 1);
		
		// 刷新OLED显示
		OLED_Refresh();
*/


		 
			
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
