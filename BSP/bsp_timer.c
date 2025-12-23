#include "bsp_timer.h"

// 外部变量声明
extern volatile uint32_t run_time_ms;
extern volatile uint32_t countdown_ms;
extern uint8_t system_state;

// 引入编码器结构体
extern volatile ENCODER_RES motorL_encoder;
extern volatile ENCODER_RES motorR_encoder;

//20ms定时器
void Timer_20ms_Init(void)
{
    //打开20ms定时器
    NVIC_ClearPendingIRQ(TIMER_20ms_INST_INT_IRQN);
	  NVIC_EnableIRQ(TIMER_20ms_INST_INT_IRQN);
		DL_TimerA_startCounter(TIMER_20ms_INST);
}

u8 gled_cnt = 0;
int encoder_left = 0;    // 左编码器每20ms增量值
int encoder_right = 0;   // 右编码器每20ms增量值

//20ms定时器中断
void TIMER_20ms_INST_IRQHandler(void)
{
    //20ms归零中断触发
	if( DL_TimerA_getPendingInterrupt(TIMER_20ms_INST) == DL_TIMER_IIDX_ZERO )
	{
		//编码器更新
				IRDataAnalysis();
				encoder_update();
        Motion_Handle(); //小车测速
        
        // 读取编码器值（每20ms读取一次）
        encoder_left = motorL_encoder.count;   // 本周期左电机脉冲数
        encoder_right = motorR_encoder.count;  // 本周期右电机脉冲数
        
        // 倒计时处理 (STATE_COUNTDOWN = 0)
        if(system_state == 0) {
            if(countdown_ms >= 20) {
                countdown_ms -= 20;  // 每次中断减少10ms
            } else {
                countdown_ms = 0;
            }
        }
        
        // 系统运行时累计时间 (STATE_RUNNING = 1)
        if(system_state == 1) {
            run_time_ms += 20;  // 每次中断增加20ms
        }
        
        // 每20ms更新一次OLED显示编码器增量值
        OLED_ShowString(0, 0, "L:", 8, 1);
        OLED_ShowNum(16, 0, encoder_left >= 0 ? encoder_left : -encoder_left, 4, 8, 1);
        if(encoder_left < 0) OLED_ShowString(12, 0, "-", 8, 1);
        else OLED_ShowString(12, 0, " ", 8, 1);
        
        OLED_ShowString(64, 0, "R:", 8, 1);
        OLED_ShowNum(80, 0, encoder_right >= 0 ? encoder_right : -encoder_right, 4, 8, 1);
        if(encoder_right < 0) OLED_ShowString(76, 0, "-", 8, 1);
        else OLED_ShowString(76, 0, " ", 8, 1);
        
        // 显示电机PWM输出（调试用）
        extern motor_data_t motor_data;
        OLED_ShowString(0, 16, "PWM_L:", 8, 1);
        OLED_ShowNum(48, 16, (int)motor_data.speed_pwm[0], 4, 8, 1);
        OLED_ShowString(0, 24, "PWM_R:", 8, 1);
        OLED_ShowNum(48, 24, (int)motor_data.speed_pwm[1], 4, 8, 1);
        
        gled_cnt++;
        if(gled_cnt>=10)
        {
            gled_cnt=0;
            DL_GPIO_togglePins(LED_PORT,LED_MCU_PIN);
        }

	}

}
