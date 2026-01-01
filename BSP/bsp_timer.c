#include "bsp_timer.h"
#include "bsp_RGB.h"

// 引入编码器结构体
extern volatile ENCODER_RES motorL_encoder;
extern volatile ENCODER_RES motorR_encoder;
// 引入转弯状态
extern int direct;
// 引入系统运行状态
extern volatile uint8_t system_running;
extern volatile uint8_t race_finished;
// RGB状态变量
volatile uint8_t g_rgb_line_detected = 0;
extern unsigned char LedsArray[];

//float correction = 0;
//int16_t track_err;
//volatile int target_L;
volatile int last_target_L;
//volatile int target_R;
volatile int last_target_R;

int direct;
volatile bool left_forward = 0;
volatile bool right_forward = 0;
int left_speed = 0;
int right_speed = 0;

// 运行时间计时（毫秒）
volatile uint32_t run_time_ms = 0;

//10ms定时器
void Timer_10ms_Init(void)
{
    //打开10ms定时器
    NVIC_ClearPendingIRQ(TIMER_10ms_INST_INT_IRQN);
	  NVIC_EnableIRQ(TIMER_10ms_INST_INT_IRQN);
		DL_TimerA_startCounter(TIMER_10ms_INST);
}

u8 gled_cnt = 0;
int encoder_left = 0;    // 左编码器每10ms增量值
int encoder_right = 0;   // 右编码器每10ms增量值

//10ms定时器中断
void TIMER_10ms_INST_IRQHandler(void)
{
    //10ms归零中断触发
	if( DL_TimerA_getPendingInterrupt(TIMER_10ms_INST) == DL_TIMER_IIDX_ZERO )
	{
		// 更新运行时间（10ms增量）
		if (system_running) {
			run_time_ms += 10;
		}
		
		// 只有系统运行时才执行循迹控制
		if (system_running == 0) {
			return;  // 系统未运行，快速退出中断
		}
		
		// 蜂鸣器状态机变量
		static u8 beep_state = 0;      // 0:空闲 1:响声 2:间隔
		static u8 beep_count = 0;      // 当前已响次数
		static u8 beep_target = 0;     // 目标响声次数
		static u8 beep_timer = 0;      // 蜂鸣器计时器（10ms单位）
		static u8 white_line_count = 0;  // 白线条数
		static u8 last_line_detected = 0;  // 上一次白线检测状态
		
		// 蜂鸣器状态机处理（非阻塞）
		if (beep_state != 0)
		{
			beep_timer++;
			if (beep_state == 1 && beep_timer >= 5)  // 响声50ms
			{
				DL_GPIO_clearPins(BEEP_PORT, BEEP_Buzzer_PIN);  // 停止响声
				beep_count++;
				if (beep_count >= beep_target)
				{
					beep_state = 0;  // 完成所有响声
				}
				else
				{
					beep_state = 2;  // 进入间隔状态
					beep_timer = 0;
				}
			}
			else if (beep_state == 2 && beep_timer >= 10)  // 间隔100ms
			{
				DL_GPIO_setPins(BEEP_PORT, BEEP_Buzzer_PIN);  // 开始下一次响声
				beep_state = 1;
				beep_timer = 0;
			}
		}
		
		// RGB控制 - 每100ms更新一次
		static uint8_t rgb_tick = 0;
		if(++rgb_tick >= 10) {  // 10*10ms = 100ms
			rgb_tick = 0;
			
			// 保存GPIO配置并重新初始化RGB引脚
			DL_GPIO_initDigitalOutput(IOMUX_PINCM27);
			DL_GPIO_enableOutput(RGB_PORT, RGB_WQ2812_PIN);
			
			// 根据全局状态更新RGB
			if(g_rgb_line_detected) {
				LedsArray[0] = 0x00; LedsArray[1] = 0xff; LedsArray[2] = 0x00;
				LedsArray[3] = 0x00; LedsArray[4] = 0xff; LedsArray[5] = 0x00;
			} else {
				LedsArray[0] = 0x00; LedsArray[1] = 0x00; LedsArray[2] = 0xff;
				LedsArray[3] = 0x00; LedsArray[4] = 0x00; LedsArray[5] = 0xff;
			}
			rgb_SendArray();
		}
		
		// 红外数据采集
		IRDataAnalysis();

		static u8 x1,x2,x3,x4,x5,x6,x7,x8;
		deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
		direct = Direct_Read(x1,x2,x3,x4,x5,x6,x7,x8);
		
		// 更新白线检测状态
		g_rgb_line_detected = (x1 && x2 && x3 && x4 && x5 && x6 && x7 && x8);
		
		// 检测白线边沿（从无到有），触发蜂鸣器
		if (g_rgb_line_detected && !last_line_detected)
		{
			// 刚进入白线，增加计数并启动蜂鸣器
			white_line_count++;
			beep_target = white_line_count;
			beep_count = 0;
			beep_state = 1;
			beep_timer = 0;
			DL_GPIO_setPins(BEEP_PORT, BEEP_Buzzer_PIN);  // 立即开始第一次响声
			
			// 第五条线为停车线
			if (white_line_count >= 5)
			{
				Motor_Set(0, 0, 0, 0);  // 停止电机
				DL_GPIO_clearPins(BEEP_PORT, BEEP_Buzzer_PIN);  // 关闭蜂鸣器
				beep_state = 0;        // 清除蜂鸣器状态
				
				race_finished = 1;     // 标记比赛完成
				system_running = 0;    // 停止系统
				return;                // 退出中断
			}
		}
		last_line_detected = g_rgb_line_detected;
		
		if (direct == 0)
		{
			// 根据上次速度差判断转弯方向，并设置标志位
			if (last_target_L < last_target_R)  // 左转
			{
				Motor_Set(0, 2000, 1, 2000);
			}
			else if (last_target_L > last_target_R)  // 右转
			{
				Motor_Set(1, 2000, 0, 2000);
			}
		}
		// 正常循迹
		else
		{
			setspeed_pid(&left_speed, &right_speed);
			// 使用PID计算出的速度驱动电机
			Motor_Set(1, left_speed, 1, right_speed);
		}
		last_target_L = left_speed;
		last_target_R = right_speed;
	}
}
