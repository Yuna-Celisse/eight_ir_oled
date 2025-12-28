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
volatile uint8_t race_finished = 0;   // 比赛完成标志：0-未完成，1-已完成
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
	
		// 检测K1按键
		uint8_t key_value = KEY_Scan();
		if (key_value)
		{
			if (system_running == 0)
			{
				// 清除OLED显示，显示倒计时
				OLED_Clear();
				OLED_ShowString(0, 0, (uint8_t*)"Starting in 1s", 8, 1);
				OLED_Refresh();
				
				// 延迟1秒后启动
				delay_ms(1000);
				
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
				
			}
			delay_ms(200);  // 按键后延时，避免重复触发
		}
		
		// 如果系统未运行
		if (system_running == 0)
		{
			if (race_finished)
			{
					uint32_t sec = run_time_ms / 1000;
					uint32_t ms = run_time_ms % 1000;
					
					OLED_Clear();
					OLED_ShowNum(0, 0, sec, 2, 8, 1);
					OLED_ShowString(16, 0, (uint8_t*)".", 8, 1);
					OLED_ShowNum(30, 0, ms, 3, 8, 1);
					OLED_ShowString(50, 0, (uint8_t*)"s", 8, 1);
					OLED_Refresh();
				while(1);
			}
		}
		
		if (system_running == 1)
		{
			// 正计时显示（从0开始往上计数）
		uint32_t sec = run_time_ms / 1000;
		uint32_t ms = run_time_ms % 1000;
		
		// 显示时间（不清屏，只更新数字避免闪烁）
		OLED_ShowString(0, 0, (uint8_t*)"T:  .   s", 8, 1);  // 先显示模板清除旧数字
		OLED_ShowNum(16, 0, sec, 2, 8, 1);
		OLED_ShowNum(40, 0, ms, 3, 8, 1);
		
		OLED_Refresh();
		}
	}
}
