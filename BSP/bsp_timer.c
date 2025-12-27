#include "bsp_timer.h"

// 引入编码器结构体
extern volatile ENCODER_RES motorL_encoder;
extern volatile ENCODER_RES motorR_encoder;
// 引入转弯状态
extern int direct;

float correction = 0;
int16_t track_err;
volatile int target_L;
volatile int last_target_L;
volatile int target_R;
volatile int last_target_R;

//10ms定时器
void Timer_10ms_Init(void)
{
    //打开10ms定时器
    NVIC_ClearPendingIRQ(TIMER_20ms_INST_INT_IRQN);
	  NVIC_EnableIRQ(TIMER_20ms_INST_INT_IRQN);
		DL_TimerA_startCounter(TIMER_20ms_INST);
}

u8 gled_cnt = 0;
int encoder_left = 0;    // 左编码器每10ms增量值
int encoder_right = 0;   // 右编码器每10ms增量值

//10ms定时器中断
void TIMER_20ms_INST_IRQHandler(void)
{
    //10ms归零中断触发
	if( DL_TimerA_getPendingInterrupt(TIMER_20ms_INST) == DL_TIMER_IIDX_ZERO )
	{
////		//编码器更新
		IRDataAnalysis();
////		encoder_update();
////		
////		// 读取编码器值（每10ms读取一次）
////		encoder_left = motorL_encoder.count;   // 本周期左电机脉冲数
////		encoder_right = motorR_encoder.count;  // 本周期右电机脉冲数

//		// 转弯或失线状态下，清空积分，避免带入直行控制
//		if (direct == 0) {
////			TRACK_PID.sum_error = 0;
////  		TRACK_PID.pre_error = 0;
//		}

////		correction = pid_control_track(track_err, &TRACK_PID);

//        // 只有在非转弯状态(direct != 0)时才使用PID控制
//        // direct == 0时由主循环的Motor_Set()控制转弯
//        if (direct != 0)
//        {
////            pid_control_speed(encoder_right, target_R , &SPEED_PID_R);
////            pid_control_speed(encoder_left, target_L, &SPEED_PID_L);
//        }
		
	}

}
