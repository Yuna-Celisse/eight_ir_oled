#ifndef __BSP_TIMER_H_
#define __BSP_TIMER_H_

#include "AllHeader.h"

void Timer_10ms_Init(void);

extern volatile uint32_t run_time_ms;  // 运行时间（毫秒）
extern int16_t track_err;
extern float correction;
extern volatile int target_L;
extern volatile int last_target_L;
extern volatile int target_R;
extern volatile int last_target_R;

#endif
