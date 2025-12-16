#include "bsp_timer.h"

// 外部变量声明
extern volatile uint32_t run_time_ms;
extern uint8_t system_state;

//20ms定时器
void Timer_20ms_Init(void)
{
    //打开20ms定时器
    NVIC_ClearPendingIRQ(TIMER_20ms_INST_INT_IRQN);
	  NVIC_EnableIRQ(TIMER_20ms_INST_INT_IRQN);
		DL_TimerA_startCounter(TIMER_20ms_INST);
}

u8 gled_cnt = 0;
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
        
        // 系统运行时累计时间 (STATE_RUNNING = 1)
        if(system_state == 1) {
            run_time_ms += 20;  // 每次中断增加20ms
        }
        
        gled_cnt++;
        if(gled_cnt>=10)
        {
            gled_cnt=0;
            DL_GPIO_togglePins(LED_PORT,LED_MCU_PIN);
        }

	}

}
