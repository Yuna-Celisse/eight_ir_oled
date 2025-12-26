#include "bsp_PID_motor.h"

// PID_t pid_motor[2];

// // YAW偏航角
// //YAW yaw angle
// PID pid_Yaw = {0, 0.4, 0, 0.1, 0, 0, 0};

// // 初始化PID参数
// //Initialize PID parameters
// void PID_Param_Init(void)
// {
//     /* 速度相关初始化参数 */
// 	//Speed dependent initialization parameters
//     for (int i = 0; i < MAX_MOTOR; i++)
//     {
//         pid_motor[i].target_val = 0.0;
//         pid_motor[i].pwm_output = 0.0;
//         pid_motor[i].err = 0.0;
//         pid_motor[i].err_last = 0.0;
//         pid_motor[i].err_next = 0.0;
//         pid_motor[i].integral = 0.0;

//         pid_motor[i].Kp = PID_DEF_KP;
//         pid_motor[i].Ki = PID_DEF_KI;
//         pid_motor[i].Kd = PID_DEF_KD;
//     }
// 		PID_Set_Motor_Parm(0,1,0,0.01);   //0是左电机
// 		PID_Set_Motor_Parm(1,1,0,0.01);  //1是右电机
//     pid_Yaw.Proportion = PID_YAW_DEF_KP;
//     pid_Yaw.Integral = PID_YAW_DEF_KI;
//     pid_Yaw.Derivative = PID_YAW_DEF_KD;
// }

// // Set PID parameters 设置PID参数
// void PID_Set_Parm(PID_t *pid, float p, float i, float d)
// {
//     pid->Kp = p; // Set Scale Factor 设置比例系数 P
//     pid->Ki = i; // Set integration coefficient 设置积分系数 I
//     pid->Kd = d; // Set differential coefficient 设置微分系数 D
// }

// // Set the target value of PID 设置PID的目标值
// void PID_Set_Target(PID_t *pid, float temp_val)
// {
//     pid->target_val = temp_val; // Set the current target value 设置当前的目标值
// }

// // Obtain PID target value 获取PID目标值
// float PID_Get_Target(PID_t *pid)
// {
//     return pid->target_val; // Set the current target value 设置当前的目标值
// }

// // Incremental PID calculation formula 增量式PID计算公式
// float PID_Incre_Calc(PID_t *pid, float actual_val)
// {
//     /*计算目标值与实际值的误差*/
// 	//Calculate the error between the target value and the actual value
//     pid->err = pid->target_val - actual_val;
//     /*PID算法实现*/
//     //PID algorithm implementation
//     pid->pwm_output += pid->Kp * (pid->err - pid->err_next) + pid->Ki * pid->err + pid->Kd * (pid->err - 2 * pid->err_next + pid->err_last);
//     /*传递误差*/
//     //transmission error
//     pid->err_last = pid->err_next;
//     pid->err_next = pid->err;

//     /*返回PWM输出值*/
//     /*Return PWM output value*/

// //    if (pid->pwm_output > (MAX_SPEED - MOTOR_DEAD_ZONE))
// //        pid->pwm_output = (MAX_SPEED - MOTOR_DEAD_ZONE);
// //    if (pid->pwm_output < (MOTOR_DEAD_ZONE - MAX_SPEED))
// //        pid->pwm_output = (MOTOR_DEAD_ZONE - MAX_SPEED);

//     return pid->pwm_output;
// }

// // 位置式PID计算方式
// //Position PID calculation method
// float PID_Location_Calc(PID_t *pid, float actual_val)
// {
//     /*计算目标值与实际值的误差*/
// 	/*Calculate the error between the target value and the actual value*/
//     pid->err = pid->target_val - actual_val;

//     /* 限定闭环死区 */
//     /*Limited closed-loop dead zone*/
//     if ((pid->err >= -40) && (pid->err <= 40))
//     {
//         pid->err = 0;
//         pid->integral = 0;
//     }

//     /* 积分分离，偏差较大时去掉积分作用 */
//     /*Integral separation, removing the integral effect when the deviation is large*/
//     if (pid->err > -1500 && pid->err < 1500)
//     {
//         pid->integral += pid->err; // error accumulation 误差累积

//         /* Limit the integration range to prevent integration saturation 限定积分范围，防止积分饱和 */
//         if (pid->integral > 4000)
//             pid->integral = 4000;
//         else if (pid->integral < -4000)
//             pid->integral = -4000;
//     }

//     /*PID算法实现*/ /*PID algorithm implementation*/
//     pid->output_val = pid->Kp * pid->err +
//                       pid->Ki * pid->integral +
//                       pid->Kd * (pid->err - pid->err_last);

//     /*误差传递*/ /*Error transmission*/
//     pid->err_last = pid->err;

//     /*返回pid输出值*/ 
//     return pid->output_val;
// }

// // PID计算输出值 PID calculation output value
// void PID_Calc_Motor(motor_data_t *motor)
// {
//     int i;
//     // float pid_out[4] = {0};
//     // for (i = 0; i < MAX_MOTOR; i++)
//     // {
//     //     pid_out[i] = PID_Location_Calc(&pid_motor[i], 0);
//     //     PID_Set_Motor_Target(i, pid_out[i]);
//     // }

//     for (i = 0; i < MAX_MOTOR; i++)
//     {
//         motor->speed_pwm[i] = PID_Incre_Calc(&pid_motor[i], motor->speed_mm_s[i]);
//     }
// }

// // PID单独计算一条通道 PID calculates one channel separately
// float PID_Calc_One_Motor(uint8_t motor_id, float now_speed)
// {
//     if (motor_id >= MAX_MOTOR)
//         return 0;
//     return PID_Incre_Calc(&pid_motor[motor_id], now_speed);
// }

// // 设置PID参数，motor_id=4设置所有，=0123设置对应电机的PID参数。
// //Set PID parameters, motor_ Id=4 Set all,=0123 Set the PID parameters of the corresponding motor.
// void PID_Set_Motor_Parm(uint8_t motor_id, float kp, float ki, float kd)
// {
//     if (motor_id > MAX_MOTOR)
//         return;

//     if (motor_id == MAX_MOTOR)
//     {
//         for (int i = 0; i < MAX_MOTOR; i++)
//         {
//             pid_motor[i].Kp = kp;
//             pid_motor[i].Ki = ki;
//             pid_motor[i].Kd = kd;
//         }
//     }
//     else
//     {
//         pid_motor[motor_id].Kp = kp;
//         pid_motor[motor_id].Ki = ki;
//         pid_motor[motor_id].Kd = kd;
//     }
// }

// // 清除PID数据
// //Clear PID data
// void PID_Clear_Motor(uint8_t motor_id)
// {
//     if (motor_id > MAX_MOTOR)
//         return;

//     if (motor_id == MAX_MOTOR)
//     {
//         for (int i = 0; i < MAX_MOTOR; i++)
//         {
//             pid_motor[i].pwm_output = 0.0;
//             pid_motor[i].err = 0.0;
//             pid_motor[i].err_last = 0.0;
//             pid_motor[i].err_next = 0.0;
//             pid_motor[i].integral = 0.0;
//         }
//     }
//     else
//     {
//         pid_motor[motor_id].pwm_output = 0.0;
//         pid_motor[motor_id].err = 0.0;
//         pid_motor[motor_id].err_last = 0.0;
//         pid_motor[motor_id].err_next = 0.0;
//         pid_motor[motor_id].integral = 0.0;
//     }
// }

// // 设置PID目标速度，单位为：mm/s
// //Set PID target speed in mm/s
// void PID_Set_Motor_Target(uint8_t motor_id, float target)
// {
//     if (motor_id > MAX_MOTOR)
//         return;

//     if (motor_id == MAX_MOTOR)
//     {
//         for (int i = 0; i < MAX_MOTOR; i++)
//         {
//             pid_motor[i].target_val = target;
//         }
//     }
//     else
//     {
//         pid_motor[motor_id].target_val = target;
//     }
// }

// // 返回PID结构体数组
// //Returns an array of PID structures
// PID_t *Pid_Get_Motor(void)
// {
//     return pid_motor;
// }

int8_t SPEED = 24;

PID SPEED_PID_L = {
    .L_or_R = true,
    .KP = 5,
    .KI = 0.4,
    .KD = 0,
    .output = 0,
    .error = 0,
    .sum_error = 0,
    .pre_error = 0,
    .limit_sum_error = 100,
    .limit_output = 68
};

PID SPEED_PID_R = {
	.L_or_R = false,
    .KP = 5,
    .KI = 0.4,
    .KD = 0,
    .output = 0,
    .error = 0,
    .sum_error = 0,
    .pre_error = 0,
    .limit_sum_error = 100,
    .limit_output = 68
};

PID TRACK_PID = {
    .KP = 0.08,
    .KI = 0.0025,
    .KD = 0.2,
	  .output = 0,
	  .error = 0,
	  .sum_error = 0,
    .pre_error = 0,
    .limit_sum_error = 100,
    .limit_output = 68
};
                      
void pid_control_speed(int current, float target, PID *pid)
{
    pid->error = target - current;
    pid->sum_error += pid->error;

    if (pid->sum_error > pid->limit_sum_error)
        pid->sum_error = pid->limit_sum_error;
    else if (pid->sum_error < -pid->limit_sum_error)
        pid->sum_error = -pid->limit_sum_error;

    pid->output = pid->KP * pid->error + pid->KI * pid->sum_error;

    if (pid->output > pid->limit_output)
        pid->output = pid->limit_output;
    else if (pid->output < -pid->limit_output)
        pid->output = -pid->limit_output;

    int pwm = (SPEED + pid->output)*125;

    // 限制PWM值范围
    if (pwm < 0) pwm = 0;
    if (pwm > 8500) pwm = 8500;

    if (pid->L_or_R)
    {
        // 左电机控制 - 使用通道2和3
        // 正转：通道3输出PWM，通道2输出0
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, pwm, DL_TIMER_CC_3_INDEX);
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_2_INDEX);
    }
    else
    {
        // 右电机控制 - 使用通道0和1  
        // 正转：通道0输出PWM，通道1输出0
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, pwm, DL_TIMER_CC_0_INDEX);
        DL_TimerA_setCaptureCompareValue(motor_PWM_INST, 0, DL_TIMER_CC_1_INDEX);
    }
}

float pid_control_track(int track_error, PID *pid)
{
    pid->error = track_error;
    pid->sum_error += pid->error;
    if (pid->sum_error > pid->limit_sum_error)
        pid->sum_error = pid->limit_sum_error;
    if (pid->sum_error < -pid->limit_sum_error)
        pid->sum_error = -pid->limit_sum_error;

//    float derivative = pid->error - pid->pre_error;
//    pid->pre_error = pid->error;

    float output = pid->KP * pid->error +
                   pid->KI * pid->sum_error;
		//+pid->KD * derivative;

    if (output > pid->limit_output) output = pid->limit_output;
    if (output < -pid->limit_output) output = -pid->limit_output;

    pid->output = output;
    return output;
}
