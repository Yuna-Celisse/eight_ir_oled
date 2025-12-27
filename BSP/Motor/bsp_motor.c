#include "bsp_motor.h"

void Init_Motor_PWM(void)
{
    SYSCFG_DL_motor_PWM_init();
    DL_TimerA_startCounter(motor_PWM_INST);  // 启动PWM定时器
}

/**
 * @brief 电机控制函数 - 设置左右轮的方向和速度
 * @param left_forward  左轮方向: true=前进, false=后退
 * @param left_speed    左轮速度: 0-10000 (0=停止, 10000=满转)
 * @param right_forward 右轮方向: true=前进, false=后退
 * @param right_speed   右轮速度: 0-10000 (0=停止, 10000=满转)
 */
void Motor_Set(bool left_forward, uint16_t left_speed, bool right_forward, uint16_t right_speed)
{
    //限制最大速度为85%，避免100%占空比可能的问题
    if(left_speed > 8500) left_speed = 8500;
    if(right_speed > 8500) right_speed = 8500;
    
    // 左电机控制 (通道2和通道3)
    if (left_forward)
    {
        // 正转：通道3输出PWM，通道2输出0（低电平）
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, left_speed, DL_TIMER_CC_3_INDEX);
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_2_INDEX);
    }
    else
    {
        // 反转：通道3输出0（低电平），通道2输出PWM
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_3_INDEX);
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, left_speed, DL_TIMER_CC_2_INDEX);
    }
    
    // 右电机控制 (通道0和通道1)
    if (right_forward)
    {
        // 正转：通道0输出PWM，通道1输出0（低电平）
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, right_speed, DL_TIMER_CC_0_INDEX);
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_1_INDEX);
    }
    else
    {
        // 反转：通道0输出0（低电平），通道1输出PWM
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_0_INDEX);
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, right_speed, DL_TIMER_CC_1_INDEX);
    }
}


// //小车停止 参数刹车和不刹车
// void Motor_Stop(uint8_t brake)
// {
//    if(brake == 1)//刹车 - 所有通道输出高电平（设置10000）
//    {
//         DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 8500, DL_TIMER_CC_3_INDEX);
//         DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 8500, DL_TIMER_CC_2_INDEX);
       
//         DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 8500, DL_TIMER_CC_0_INDEX);
//         DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 8500, DL_TIMER_CC_1_INDEX);
//    }
//    else  //不刹车 - 所有通道输出低电平（设置0）
//    {
//         DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_3_INDEX);
//         DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_2_INDEX);
       
//         DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_0_INDEX);
//         DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_1_INDEX);
//    }
// }
