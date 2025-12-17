#include "AllHeader.h"

// 全局变量定义
void Beep_Times(uint8_t times);
void Display_Time(void);
void Display_Countdown(uint8_t seconds);
void Check_Wide_Line(void);
void RGB_Running_Effect(void);

// 系统状态变量
typedef enum {
    STATE_COUNTDOWN,     // 倒计时等待启动
    STATE_RUNNING,       // 运行中
    STATE_FINISHED       // 已完成
} SystemState_t;

SystemState_t system_state = STATE_COUNTDOWN;
volatile uint32_t run_time_ms = 0;      // 运行时间(毫秒)
volatile uint8_t beep_point_count = 0;  // 蜂鸣器点位计数
volatile bool is_on_wide_line = false;  // 是否在宽线上
volatile bool wide_line_detected = false; // 宽线检测标志
volatile uint8_t wide_line_type = 0;    // 宽线类型: 1=起点, 2=蜂鸣器点, 3=终点
volatile uint32_t rgb_effect_counter = 0;
volatile uint32_t countdown_ms = 0;     // 倒计时(毫秒)
uint8_t countdown_seconds = 3;          // 倒计时秒数（可修改：3秒/5秒/10秒）

int main(void)
{
	// 系统配置初始化  System configuration initialization
	SYSCFG_DL_init();       
	 // OLED显示屏初始化  OLED display initialization
	OLED_Init();            
	 // 串口通信初始化  USART communication initialization	
	USART_Init();           
	 
	Init_Motor_PWM();
   //初始化电机编码器
  encoder_init(); 
//  电机控速pid2初始化
  PID_Param_Init();
  
  // 初始显示
  OLED_Clear();
  OLED_ShowString(0, 0, "Smart Car Race", 8, 1);
  OLED_ShowString(0, 16, "Auto Start in:", 8, 1);
  OLED_Refresh();
  
  // 初始化倒计时
  countdown_ms = countdown_seconds * 1000;

	uart0_send_string("$0,0,1#");
	
	while (1)                
	{
		// 状态机处理
		switch(system_state) {
			case STATE_COUNTDOWN:
				// 倒计时显示
				Display_Countdown((countdown_ms + 999) / 1000);  // 向上取整显示秒数
				
				// 倒计时结束，自动启动
				if(countdown_ms == 0) {
					system_state = STATE_RUNNING;
					run_time_ms = 0;
					beep_point_count = 0;
					OLED_Clear();
					Beep_Times(2);  // 启动提示音（响2次）
					delay_ms(200);  // 短暂延时
				}
				break;
				
			case STATE_RUNNING:
				// 检测宽线并处理
				Check_Wide_Line();
				
				// 巡线控制
				LineWalking();
				
				// RGB灯效
				RGB_Running_Effect();
				
				// 显示运行时间和状态
				Display_Time();
				
				// 显示红外传感器数据
				if(IR_recv_complete_flag) {    
					OLED_ShowString(0, 40, "IR:", 8, 1);
					for (int i = 0; i < 8; i++) 
						OLED_ShowNum(20 + i * 10, 40, IR_Data_number[i], 1, 8, 1);        
				}
				break;
				
			case STATE_FINISHED:
				// 停车状态
				Motion_Ctrl(0, 0, 0);  // 停止电机
				OLED_ShowString(0, 48, "FINISHED!", 8, 1);
				break;
		}
		
		OLED_Refresh();
	}	

}

// 显示倒计时
void Display_Countdown(uint8_t seconds) {
    OLED_ShowString(0, 32, "    ", 8, 1);  // 清除之前的数字区域
    OLED_ShowNum(48, 32, seconds, 2, 8, 1);
    OLED_ShowString(72, 32, "s", 8, 1);
    
    // 添加提示信息
    if(seconds > 0) {
        OLED_ShowString(0, 48, "Waiting...", 8, 1);
    } else {
        OLED_ShowString(0, 48, "GO! GO! GO!", 8, 1);
    }
}

// 蜂鸣器响N次
void Beep_Times(uint8_t times) {
    for(uint8_t i = 0; i < times; i++) {
        Buzzer_open_state();
        delay_ms(150);  // 响150ms
        Buzzer_close_state();
        if(i < times - 1) {
            delay_ms(150);  // 间隔150ms
        }
    }
}

// 显示运行时间
void Display_Time(void) {
    uint32_t seconds = run_time_ms / 1000;
    uint32_t milliseconds = run_time_ms % 1000;
    uint32_t centiseconds = milliseconds / 10;  // 厘秒(0.01秒)
    
    OLED_ShowString(0, 0, "Time:", 8, 1);
    OLED_ShowNum(40, 0, seconds, 3, 8, 1);
    OLED_ShowString(64, 0, ".", 8, 1);
    OLED_ShowNum(72, 0, centiseconds, 2, 8, 1);
    OLED_ShowString(96, 0, "s", 8, 1);
    
    OLED_ShowString(0, 16, "Beep:", 8, 1);
    OLED_ShowNum(40, 16, beep_point_count, 1, 8, 1);
}

// 检测宽线（20cm宽度）
void Check_Wide_Line(void) {
    static uint8_t wide_line_hold = 0;
    
    // 读取8路循迹传感器
    uint8_t all_white = 1;
    for(int i = 0; i < 8; i++) {
        if(IR_Data_number[i] == 1) {  // 1表示检测到白线
            all_white = 0;
            break;
        }
    }
    
    // 如果所有传感器都检测到白线，说明遇到宽线
    if(all_white == 0) {
        // 检查是否8个传感器大部分都是白色（检测宽线）
        uint8_t white_count = 0;
        for(int i = 0; i < 8; i++) {
            if(IR_Data_number[i] == 1) white_count++;
        }
        
        if(white_count >= 6) {  // 至少6个传感器检测到白线
            if(!is_on_wide_line) {
                is_on_wide_line = true;
                beep_point_count++;
                
                // 判断是否为终点（第9个宽线是终点D）
                if(beep_point_count >= 9) {  // A B B B C C C B D，第9个宽线是终点
                    system_state = STATE_FINISHED;
                    Beep_Times(2);  // 终点提示音
                } else if(beep_point_count == 2 || beep_point_count == 3 || beep_point_count == 4 || beep_point_count == 8) {
                    // B点位：响应次数对应点位顺序
                    uint8_t beep_times = 0;
                    if(beep_point_count == 2) beep_times = 1;       // 第一个B
                    else if(beep_point_count == 3) beep_times = 2;  // 第二个B
                    else if(beep_point_count == 4) beep_times = 3;  // 第三个B
                    else if(beep_point_count == 8) beep_times = 4;  // 第四个B
                    
                    Beep_Times(beep_times);
                }
                
                wide_line_hold = 20;  // 保持一段时间防止重复触发
            }
        } else {
            if(wide_line_hold > 0) {
                wide_line_hold--;
            } else {
                is_on_wide_line = false;
            }
        }
    } else {
        if(wide_line_hold > 0) {
            wide_line_hold--;
        } else {
            is_on_wide_line = false;
        }
    }
}

// RGB跑马灯效果
void RGB_Running_Effect(void) {
    #if defined(Control_RGB_ALL)
    rgb_effect_counter++;
    if(rgb_effect_counter % 50 == 0) {  // 每50个循环改变一次
        uint8_t pattern = (rgb_effect_counter / 50) % 6;
        switch(pattern) {
            case 0: Control_RGB_ALL(Red_RGB); break;
            case 1: Control_RGB_ALL(Green_RGB); break;
            case 2: Control_RGB_ALL(Blue_RGB); break;
            case 3: Control_RGB_ALL(Yellow_RGB); break;
            case 4: Control_RGB_ALL(Purple_RGB); break;
            case 5: Control_RGB_ALL(Cyan_RGB); break;
        }
    }
    #endif
}