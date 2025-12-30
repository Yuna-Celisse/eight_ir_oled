#include "bsp_timer.h"
#include "bsp_RGB.h"

// ============ 外部变量声明 ============
extern volatile ENCODER_RES motorL_encoder;  // 左轮编码器数据结构
extern volatile ENCODER_RES motorR_encoder;  // 右轮编码器数据结构
extern int direct;                           // 传感器检测到白色的数量（0=完全丢线，全在黑底上）
extern volatile uint8_t system_running;      // 系统运行状态：0=停止，1=运行
extern volatile uint8_t race_finished;       // 比赛完成标志：0=未完成，1=已完成
extern unsigned char LedsArray[];            // RGB LED颜色数组

// ============ 全局变量定义 ============
volatile uint8_t g_rgb_line_detected = 0;    // 水平白线检测标志：0=窄线循迹中，1=检测到水平白线
volatile int last_target_L;                  // 上一次左轮目标速度（用于丢线判断）
volatile int last_target_R;                  // 上一次右轮目标速度（用于丢线判断）
int direct;                                  // 当前检测到白色的传感器数量
volatile bool left_forward = 0;              // 左轮方向标志（暂未使用）
volatile bool right_forward = 0;             // 右轮方向标志（暂未使用）
int left_speed = 0;                          // 左轮目标速度（PID输出）
int right_speed = 0;                         // 右轮目标速度（PID输出）
volatile uint32_t run_time_ms = 0;           // 系统运行时间计数器（毫秒）

// ============ 定时器初始化函数 ============
// 功能：启动10ms定时器用于循迹控制
// 说明：定时器名称虽为TIMER_20ms但实际配置为10ms周期
void Timer_10ms_Init(void)
{
    // 清除中断挂起标志并使能定时器中断
    NVIC_ClearPendingIRQ(TIMER_20ms_INST_INT_IRQN);
	  NVIC_EnableIRQ(TIMER_20ms_INST_INT_IRQN);
		DL_TimerA_startCounter(TIMER_20ms_INST);
}

u8 gled_cnt = 0;                // 绿灯计数器（未使用）
int encoder_left = 0;           // 左编码器增量值（每10ms）
int encoder_right = 0;          // 右编码器增量值（每10ms）

// ============ 10ms定时器中断处理函数 ============
// 功能：核心循迹控制逻辑，每10ms执行一次
// 包含：时间计数、蜂鸣器控制、RGB显示、传感器采集、循迹PID、丢线处理
void TIMER_20ms_INST_IRQHandler(void)
{
    // 检查是否为定时器归零中断
	if( DL_TimerA_getPendingInterrupt(TIMER_20ms_INST) == DL_TIMER_IIDX_ZERO )
	{
		// -------- 运行时间累加 --------
		if (system_running) {
			run_time_ms += 10;  // 每次中断增加10ms
		}
		
		// -------- 系统状态检查 --------
		if (system_running == 0) {
			return;  // 系统未运行时直接退出，不执行循迹
		}
		
		// -------- 蜂鸣器非阻塞控制状态机 --------
		static u8 beep_state = 0;      // 蜂鸣器状态：0=空闲，1=响声中，2=间隔中
		static u8 beep_count = 0;      // 当前已响次数
		static u8 beep_target = 0;     // 目标响声次数（与白线数量对应）
		static u8 beep_timer = 0;      // 状态持续时间计数（10ms单位）
		static u8 white_line_count = 0;  // 已通过的白线条数
		static u8 last_line_detected = 0;  // 上一次白线检测状态（边沿检测）
		
		// 蜂鸣器状态机处理（非阻塞）
		if (beep_state != 0)
		{
			beep_timer++;
			if (beep_state == 1 && beep_timer >= 5)  // 响声持续50ms
			{
				DL_GPIO_clearPins(BEEP_PORT, BEEP_Buzzer_PIN);  // 停止蜂鸣器
				beep_count++;
				if (beep_count >= beep_target)
				{
					beep_state = 0;  // 所有响声完成，返回空闲
				}
				else
				{
					beep_state = 2;  // 进入间隔状态
					beep_timer = 0;
				}
			}
			else if (beep_state == 2 && beep_timer >= 10)  // 间隔持续100ms
			{
				DL_GPIO_setPins(BEEP_PORT, BEEP_Buzzer_PIN);  // 开始下一次响声
				beep_state = 1;
				beep_timer = 0;
			}
		}
		
		// -------- RGB LED显示控制（100ms更新一次）--------
		static uint8_t rgb_tick = 0;
		if(++rgb_tick >= 10) {  // 10次中断 = 100ms
			rgb_tick = 0;
			
			// 重新初始化RGB引脚（确保GPIO配置正确）
			DL_GPIO_initDigitalOutput(IOMUX_PINCM27);
			DL_GPIO_enableOutput(RGB_PORT, RGB_WQ2812_PIN);
			
			// 根据水平白线检测状态设置RGB颜色
			if(g_rgb_line_detected) {
				// 检测到水平白线（所有传感器返回1，全检测到白色）：红色
				LedsArray[0] = 0x00; LedsArray[1] = 0xff; LedsArray[2] = 0x00;  // LED1红色
				LedsArray[3] = 0x00; LedsArray[4] = 0xff; LedsArray[5] = 0x00;  // LED2红色
			} else {
				// 正常循迹（部分传感器在白线上返回1，部分在黑底上返回0）：蓝色
				LedsArray[0] = 0x00; LedsArray[1] = 0x00; LedsArray[2] = 0xff;  // LED1蓝色
				LedsArray[3] = 0x00; LedsArray[4] = 0x00; LedsArray[5] = 0xff;  // LED2蓝色
			}
			rgb_SendArray();  // 发送RGB数据到LED
		}
		
		// -------- 红外传感器数据采集与解析 --------
		IRDataAnalysis();  // 读取并解析8路红外传感器数据

		// 获取8个传感器的状态（地图：黑底白线）
		// 传感器值：1=检测到白线，0=检测到黑底（偏离白线）
		static u8 x1,x2,x3,x4,x5,x6,x7,x8;
		deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
		direct = Direct_Read(x1,x2,x3,x4,x5,x6,x7,x8);  // 计算检测到白色的传感器数量
		
		// -------- 水平白线检测（所有传感器都为1=全部检测到白色，说明遇到水平白线）--------
		g_rgb_line_detected = (x1 && x2 && x3 && x4 && x5 && x6 && x7 && x8);
		
		// 水平白线边沿检测：从普通赛道进入水平白线时触发
		if (g_rgb_line_detected && !last_line_detected)
		{
			// 检测到新的水平白线，计数增加
			white_line_count++;
			beep_target = white_line_count;  // 蜂鸣次数 = 水平白线数量
			beep_count = 0;
			beep_state = 1;                  // 启动蜂鸣器状态机
			beep_timer = 0;
			DL_GPIO_setPins(BEEP_PORT, BEEP_Buzzer_PIN);  // 立即开始第一次响声
			
			// 第5条水平白线为终点停车线
			if (white_line_count >= 5)
			{
				Motor_Set(0, 0, 0, 0);  // 停止电机
				DL_GPIO_clearPins(BEEP_PORT, BEEP_Buzzer_PIN);  // 关闭蜂鸣器
				beep_state = 0;         // 清除蜂鸣器状态
				
				race_finished = 1;      // 标记比赛完成
				system_running = 0;     // 停止系统运行
				return;                 // 退出中断
			}
		}
		last_line_detected = g_rgb_line_detected;  // 保存当前状态用于下次边沿检测
		
		// -------- 丢线处理（direct=0表示所有传感器都在黑底上，完全丢线）--------
		if (direct == 0)
		{
			// 注意：此时所有传感器都返回0，完全在黑底上，无法判断白线位置
			// 策略：根据上一次的速度差判断小车之前的转向趋势，继续该方向转弯寻找白线
			if (last_target_L < last_target_R)  // 上次是左转（左轮慢，右轮快）
			{
				Motor_Set(0, 2000, 1, 2000);  // 继续左转：左轮后退，右轮前进
			}
			else if (last_target_L > last_target_R)  // 上次是右转（左轮快，右轮慢）
			{
				Motor_Set(1, 2000, 0, 2000);  // 继续右转：左轮前进，右轮后退
			}
		}
		// -------- 正常循迹模式（部分传感器检测到白线，可判断白线位置）--------
		else
		{
			// 使用PID算法根据传感器偏差计算左右轮速度
			// 偏离白线中心时：部分传感器在白线上（返回1），部分在黑底上（返回0）
			setspeed_pid(&left_speed, &right_speed);
			// 驱动电机（参数：左轮方向，左轮速度，右轮方向，右轮速度）
			Motor_Set(1, left_speed, 1, right_speed);
		}
		
		// 保存当前速度用于下次丢线判断
		last_target_L = left_speed;
		last_target_R = right_speed;
	}
}
