//#include "app_motor.h"

//static float speed_lr = 0;
//static float speed_fb = 0;
//static float speed_spin = 0;

//static int speed_L1_setup = 0;
//static int speed_R1_setup = 0;


//static int g_offset_yaw = 0;
//static uint16_t g_speed_setup = 0;

//// 编码器20毫秒前后数据
////Encoder data before and after 20 milliseconds
//int g_Encoder_All_Now[MAX_MOTOR] = {0};
//int g_Encoder_All_Last[MAX_MOTOR] = {0};
//int g_Encoder_All_Offset[MAX_MOTOR] = {0};

//uint8_t g_start_ctrl = 0;

//car_data_t car_data;
//motor_data_t motor_data;

//uint8_t g_yaw_adjust = 0;

//static float Motion_Get_Circle_Pulse(void)
//{
//    return ENCODER_CIRCLE_450;
//}

//// 仅用于添加到调试中显示数据。
////Only used to display data when added to debugging.
//void *Motion_Get_Data(uint8_t index)
//{
//    if (index == 1)
//        return (int *)g_Encoder_All_Now;
//    if (index == 2)
//        return (int *)g_Encoder_All_Last;
//    if (index == 3)
//        return (int *)g_Encoder_All_Offset;
//    return 0;
//}

//// 获取电机速度
////Obtain motor speed
//void Motion_Get_Motor_Speed(float *speed)
//{
//    for (int i = 0; i < MAX_MOTOR; i++)
//    {
//        speed[i] = motor_data.speed_mm_s[i];
//    }
//}

//// 设置偏航角状态，如果使能则刷新target目标角度。
////Set the yaw angle status, and if enabled, refresh the target target angle.
//void Motion_Set_Yaw_Adjust(uint8_t adjust)
//{
//    if (adjust == 0)
//    {
//        g_yaw_adjust = 0;
//    }
//    else
//    {
//        g_yaw_adjust = 1;
//    }
//    if (g_yaw_adjust)
//    {
//        // PID_Yaw_Reset(获取当前IMU偏航角-yaw);
//    }
//}

//// 返回偏航角调节状态。
////Return to yaw angle adjustment status.
//uint8_t Motion_Get_Yaw_Adjust(void)
//{
//    return g_yaw_adjust;
//}


//// Car Stop 小车停止
//void Motion_Stop(uint8_t brake)
//{
//    Motion_Set_Speed(0, 0);
//    g_start_ctrl = 0;
//    g_yaw_adjust = 0;
//    PID_Clear_Motor(MAX_MOTOR);
////    Motor_Stop(brake);
//}

//// speed_mX=[-1000, 1000], 单位为：mm/s
////speed_mX=[-1000,1000],Unit: mm/s
//void Motion_Set_Speed(int16_t speed_m1, int16_t speed_m2)
//{
//    g_start_ctrl = 1;
//    motor_data.speed_set[0] = speed_m1;
//    motor_data.speed_set[1] = speed_m2;
//    
//    for (uint8_t i = 0; i < MAX_MOTOR; i++)
//    {
//        PID_Set_Motor_Target(i, motor_data.speed_set[i] * 1.0); 
//    }
//}



//// 从编码器读取当前各轮子速度，单位mm/s
////Read the current speed of each wheel from the encoder, in mm/s
//void Motion_Get_Speed(car_data_t *car)
//{
//    int i = 0;
//    float speed_mm[MAX_MOTOR] = {0};
//    float circle_mm = Motion_Get_Circle_MM();
//    float circle_pulse = Motion_Get_Circle_Pulse();
//    float robot_APB = Motion_Get_APB();

//    Motion_Get_Encoder();

//    // 计算轮子速度，单位mm/s。
//    //Calculate the wheel speed in mm/s.
//    for (i = 0; i <MAX_MOTOR ; i++)
//    {
//        speed_mm[i] = (g_Encoder_All_Offset[i]) * 50 * circle_mm / circle_pulse;
//    }

//    car->Vx = (speed_mm[0] + speed_mm[1] ) / 2;
//    car->Vy = -(speed_mm[0] - speed_mm[1] ) / 2;
//    car->Vz = -(speed_mm[0] - speed_mm[1]) / 2.0f / robot_APB * 1000;

//    if (g_start_ctrl)
//    {
//        for (i = 0; i < MAX_MOTOR; i++)
//        {
//            motor_data.speed_mm_s[i] = speed_mm[i];
//        }
//        PID_Calc_Motor(&motor_data);
//    }
//}

//// 返回当前小车轮子轴间距和的一半
////Returns half of the sum of the current wheel spacing of the small car
//float Motion_Get_APB(void)
//{
//    return MSPM0Car_APB;
//}

//// 返回当前小车轮子转一圈的多少毫米
////Returns the number of millimeters the current car wheel has rotated once
//float Motion_Get_Circle_MM(void)
//{
//    return MECANUM_CIRCLE_MM;
//}

//// 获取编码器数据，并计算偏差脉冲数
////Obtain encoder data and calculate the number of deviation pulses
//void Motion_Get_Encoder(void)
//{
//    Encoder_Get_ALL(g_Encoder_All_Now);

//    for (uint8_t i = 0; i < MAX_MOTOR; i++)
//    {
//        // 记录两次测试时间差的脉冲数
//    	//Record the number of pulses between two test times
//        g_Encoder_All_Offset[i] = g_Encoder_All_Now[i] - g_Encoder_All_Last[i];
//        // 记录上次编码器数据
//        //Record Last Encoder Data
//        g_Encoder_All_Last[i] = g_Encoder_All_Now[i];
//    }
//}

//// 控制小车运动
////Control the movement of the car
//void Motion_Ctrl(int16_t V_x, int16_t V_y, int16_t V_z)
//{
//    wheel_Ctrl(V_x, V_y, V_z);
//}

//void Motion_Ctrl_State(uint8_t state, uint16_t speed, uint8_t adjust)
//{
//    uint16_t input_speed = speed * 10;
//    wheel_State_YAW(state, input_speed, adjust);
//}

//// 控制小车运动状态
//// 速度控制：speed=0~1000。
//// 偏航角调节运动：adjust=1开启，=0不开启。
////Control the movement status of the car.
////Speed control: speed=0-1000.
////Yaw angle adjustment motion: adjust=1 on,=0 not on.
//void wheel_State_YAW(uint8_t state, uint16_t speed, uint8_t adjust)
//{
//    Motion_Set_Yaw_Adjust(adjust);
//    g_speed_setup = speed;
//    switch (state)
//    {
//    case MOTION_STOP:
//        g_speed_setup = 0;
//        Motion_Stop(speed == 0 ? STOP_FREE : STOP_BRAKE);
//        break;
//    case MOTION_RUN:
//        wheel_Ctrl(speed, 0, 0);
//        break;
//    case MOTION_BACK:
//        wheel_Ctrl(-speed, 0, 0);
//        break;
//    case MOTION_LEFT:
//    	wheel_Ctrl(speed/2, 0, -speed*2);
//        break;
//    case MOTION_RIGHT:
//    	wheel_Ctrl(speed/2, 0, speed*2);

//        break;
//    case MOTION_SPIN_LEFT:
//        Motion_Set_Yaw_Adjust(0);
//        wheel_Ctrl(0, 0, -speed * 5);
//        break;
//    case MOTION_SPIN_RIGHT:
//        Motion_Set_Yaw_Adjust(0);
//        wheel_Ctrl(0, 0, speed * 5);
//        break;
//    case MOTION_BRAKE:
//        Motion_Stop(STOP_BRAKE);
//        break;
//    default:
//        break;
//    }
//}

//// 控制小车运动状态
//// 速度控制：speed=0~1000。
////Control the movement status of the car.
////Speed control: speed=0-1000.
//void wheel_State(uint8_t state, uint16_t speed)
//{
//    g_speed_setup = speed;
//    switch (state)
//    {
//    case MOTION_STOP:
//        g_speed_setup = 0;
//        Motion_Stop(speed == 0 ? STOP_FREE : STOP_BRAKE);
//        break;
//    case MOTION_RUN:
//        wheel_Ctrl(speed, 0, 0);
//        break;
//    case MOTION_BACK:
//        wheel_Ctrl(-speed, 0, 0);
//        break;
//    case MOTION_LEFT:
//    	wheel_Ctrl(speed/2, 0, -speed*2);
//        break;
//    case MOTION_RIGHT:
//    	wheel_Ctrl(speed/2, 0, speed*2);

//        break;
//    case MOTION_SPIN_LEFT:
//        Motion_Set_Yaw_Adjust(0);
//        wheel_Ctrl(0, 0, -speed * 5);
//        break;
//    case MOTION_SPIN_RIGHT:
//        Motion_Set_Yaw_Adjust(0);
//        wheel_Ctrl(0, 0, speed * 5);
//        break;
//    case MOTION_BRAKE:
//        Motion_Stop(STOP_BRAKE);
//        break;
//    default:
//        break;
//    }
//}


//void wheel_Ctrl(int16_t V_x, int16_t V_y, int16_t V_z)
//{
//    float robot_APB = Motion_Get_APB();
////    speed_lr = -V_y;
//    speed_lr = 0;
//    speed_fb = V_x;
//    speed_spin = (V_z / 1000.0f) * robot_APB;
//    if (V_x == 0 && V_y == 0 && V_z == 0)
//    {
//        Motion_Stop(STOP_BRAKE);
//        return;
//    }

//    speed_L1_setup = speed_fb + speed_lr + speed_spin;
//    speed_R1_setup = speed_fb - speed_lr - speed_spin;

//    if (speed_L1_setup > 1000)
//        speed_L1_setup = 1000;
//    if (speed_L1_setup < -1000)
//        speed_L1_setup = -1000;
//   
//    if (speed_R1_setup > 1000)
//        speed_R1_setup = 1000;
//    if (speed_R1_setup < -1000)
//        speed_R1_setup = -1000;
//    
//    
////    printf("%d,%d\r\n",speed_L1_setup, speed_R1_setup);
//    //调加pid的控制，反应慢就直接pwm控制
//    Motion_Set_Speed(speed_L1_setup, speed_R1_setup); //pid
//    
////    PWM_Control_Car(speed_L1_setup,speed_R1_setup);//直接pwm控
//}

//// 运动控制句柄，每20ms调用一次，主要处理速度相关的数据
////Motion control handle, called every 20ms, mainly processing speed related data
//extern PID_t pid_motor[2];
//void Motion_Handle(void)
//{
//    
//    Motion_Get_Speed(&car_data);

//    if (g_start_ctrl)
//    {
//        //这样效果更好
//        if(motor_data.speed_set[0]==0)
//        {
//            motor_data.speed_pwm[0] = 0;
//            pid_motor[0].pwm_output = 0;
//        }
//            
//        if(motor_data.speed_set[1]==0)
//        {
//            motor_data.speed_pwm[1] = 0;
//            pid_motor[1].pwm_output = 0;
//        }
//        
//        // 根据PWM值设置电机方向和速度
//        bool left_forward = (motor_data.speed_pwm[0] >= 0);
//        bool right_forward = (motor_data.speed_pwm[1] >= 0);
//        uint16_t left_speed = (uint16_t)(left_forward ? motor_data.speed_pwm[0] : -motor_data.speed_pwm[0]);
//        uint16_t right_speed = (uint16_t)(right_forward ? motor_data.speed_pwm[1] : -motor_data.speed_pwm[1]);
//        Motor_Set(left_forward, left_speed, right_forward, right_speed);
//    }
//    
//}

//void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z)
//{
//	float robot_APB = Motion_Get_APB();
//	speed_lr = 0;
//    speed_fb = V_x;
//    speed_spin = (V_z / 1000.0f) * robot_APB;
//    if (V_x == 0 && V_y == 0 && V_z == 0)
//    {
//        Motion_Set_Speed(0,0);
//        return;
//    }

//    speed_L1_setup = speed_fb + speed_spin;

//    speed_R1_setup = speed_fb  - speed_spin;

//		
//    if (speed_L1_setup > 1000) speed_L1_setup = 1000;
//    if (speed_L1_setup < -1000) speed_L1_setup = -1000;

//    if (speed_R1_setup > 1000) speed_R1_setup = 1000;
//    if (speed_R1_setup < -1000) speed_R1_setup = -1000;

//    
//    
//    Motion_Set_Speed(speed_L1_setup,speed_R1_setup);
//		
//}



////      Motion_Set_Speed(speed_L1_setup,speed_R1_setup);
////}


// 简化的电机控制函数实现
#include "app_motor.h"

void Motor_Run(uint16_t left_speed, uint16_t right_speed)
{
    Motor_Set(true, left_speed, true, right_speed);
}

void Motor_Left(uint16_t left_speed, uint16_t right_speed)
{
    Motor_Set(false, left_speed, true, right_speed);
}

void Motor_Right(uint16_t left_speed, uint16_t right_speed)
{
    Motor_Set(true, left_speed, false, right_speed);
}

void Motor_Back(uint16_t left_speed, uint16_t right_speed)
{
    Motor_Set(false, left_speed, false, right_speed);
}

// 红外接收相关变量和函数
static uint8_t infrared_command = 0xFF;

uint8_t get_infrared_command(void)
{
    return infrared_command;
}

void clear_infrared_command(void)
{
    infrared_command = 0xFF;
}

// UART3发送函数
void uart3_send_char(char ch)
{
    // 如果UART3未配置，这里是空实现
    // 实际项目中需要根据硬件配置实现
    (void)ch;
}

// Motion_Ctrl函数实现
void Motion_Ctrl(int16_t V_x, int16_t V_y, int16_t V_z)
{
    // 简化实现：V_x是前进速度，V_z是转向调整
    int16_t left_speed = V_x + V_z;
    int16_t right_speed = V_x - V_z;
    
    // 限幅
    if (left_speed > 1000) left_speed = 1000;
    if (left_speed < -1000) left_speed = -1000;
    if (right_speed > 1000) right_speed = 1000;
    if (right_speed < -1000) right_speed = -1000;
    
    // 根据速度正负设置方向和速度
    if (left_speed >= 0 && right_speed >= 0)
        Motor_Set(true, (uint16_t)left_speed, true, (uint16_t)right_speed);
    else if (left_speed < 0 && right_speed < 0)
        Motor_Set(false, (uint16_t)(-left_speed), false, (uint16_t)(-right_speed));
    else if (left_speed >= 0 && right_speed < 0)
        Motor_Set(true, (uint16_t)left_speed, false, (uint16_t)(-right_speed));
    else
        Motor_Set(false, (uint16_t)(-left_speed), true, (uint16_t)right_speed);
}

// wheel_State函数实现
void wheel_State(uint8_t state, uint16_t speed)
{
    switch(state)
    {
        case MOTION_RUN:
            Motor_Run(speed, speed);
            break;
        case MOTION_BACK:
            Motor_Back(speed, speed);
            break;
        case MOTION_LEFT:
            Motor_Left(speed, speed);
            break;
        case MOTION_RIGHT:
            Motor_Right(speed, speed);
            break;
        case MOTION_SPIN_LEFT:
            Motor_Set(false, speed, true, speed);
            break;
        case MOTION_SPIN_RIGHT:
            Motor_Set(true, speed, false, speed);
            break;
        case MOTION_STOP:
        default:
            Motor_Set(false, 0, false, 0);
            break;
    }
}

// Motion_Stop函数实现
void Motion_Stop(uint8_t brake)
{
    Motor_Set(false, 0, false, 0);
}

// Motor_Stop函数实现（与Motion_Stop功能相同）
void Motor_Stop(uint8_t brake)
{
    Motor_Set(false, 0, false, 0);
}





