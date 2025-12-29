#include "get_mpu6050.h"
#include "Filter.h"
#define Pi 3.14159265




int encoder_odometry_flag = 0;
extern volatile float gyro_x,gyro_y,gyro_z,accel_z;
extern volatile float Accel_Angle_X,Accel_Angle_Y;
//uint8_t mode = NO_SELECT_MODE;
extern volatile float kal_mpu_out;
volatile int balance_yaw;
          
float object_yaw = 0; 



uint8_t yaw_flag =0;



float 
	Turn_Kd=1.00,//转向环KP、KD1.0
	Turn_Kp=1.00;
int	yaw_out  ;//-150

int yaw_out2;
//H题mode1

volatile short angle[3];
volatile short accel[3];
volatile float pitch,roll,yaw;   //欧拉角 Euler Angles

volatile float kal_mpu_out;

volatile float Filter_out;

volatile float Yaw_Err_Lowout_last;
volatile float Yaw_Err_Lowout;

volatile float erro_sumr;
volatile float error;

volatile float gyro_x,gyro_y,gyro_z,accel_z;
volatile float Accel_Angle_X,Accel_Angle_Y;
uint8_t Way_Angle=2; 
Bias_t Angle;

const int CALIB_SAMPLES = 30; // 采样次数  Number of samples
float Angle_Balance,Gyro_Balance,Gyro_Turn; //平衡倾角 平衡陀螺仪 转向陀螺仪
float Acceleration_Z;
// 计算偏移量   Number of samples
volatile float yawBias = 0, pitchBias = 0, rollBias = 0;

volatile float calibratedYaw = 0, calibratedPitch = 0, calibratedRoll = 0;

float AngleOffsetCalc(void)
{
    for (int i = 0; i < CALIB_SAMPLES; i++)
    {
        Get_EulerAngles();
//        printf("yaw:%.2f, pitch:%.2f, roll:%.2f\r\n", yaw, pitch, roll);
        calibratedYaw += calibratedYaw;

       
//        delay_ms(20);
    }
//    printf("yawsum:%d\r\n", (int)yawSum);
    // 计算偏移量   Calculate offset
    calibratedYaw = calibratedYaw / CALIB_SAMPLES;

    
//    printf("yawBias:%.2f, pitchBias:%.2f, rollBias:%.2f", yawBias, pitchBias, rollBias);
}
//获取已校准的角度  Get the calibrated angle
void Get_CalibratedAngles(void)
{
	
	
	
	
    if(Filter_out < -2)
    {
        calibratedYaw = -Filter_out;
    }
    else if(Filter_out >= 2)
    {
        calibratedYaw = 360 - Filter_out; //车头朝前yaw顺时针为负数  逆时针为正数 yaw clockwise is negative
    }
    else
    {
        calibratedYaw = 0; 	//过滤掉0和360附近的数据  Filter out data near 0 and 360
			
    }

}

float get_Filter(float 	Yaw_Err)	
{
	
	
	float a=0.630;

	Yaw_Err_Lowout=(1-a)*Yaw_Err+a*Yaw_Err_Lowout_last;//使得波形更加平滑，滤除高频干扰
	Yaw_Err_Lowout_last=Yaw_Err_Lowout;

	return Yaw_Err_Lowout;
}




void Get_EulerAngles(void)
{
    //获取欧拉角 Get Euler angles
    mpu_dmp_get_data(&pitch,&roll,&yaw);
	  
	

		kal_mpu_out=KalmanFilter(&kfp_mpu,yaw);
		Filter_out=get_Filter(yaw);
//		calibratedYaw=Filter_out+180;

//	  delay_ms(10);//根据设置的采样率，不可设置延时过大 According to the set sampling rate, the delay cannot be set too large
//	printf("pitch :%3.2f roll :%3.2f yaw:%3.2f\r\n",pitch,roll,Filter_out);

}

//角度环PID控制 Angle ring PID control
float dir_kp = 2.0,dir_ki=0.000,dir_kd = 5.00;
int Integral_Max = 300; //300
int pid_max = 1000; //3000
float Dir_PID(float error)
{ //5*20/1000*188
    float result = 0;
    static int16_t err_last = 0; //上次的误差初始为0  Last error
    static float Integral = 0; // 初始化积分项 Initialize integral term

//   if(error == 0)          
//   {
//       Integral = 0;          //积分清零   Integral cleared
//   }
    Integral += error;           // 更新积分项，并进行限幅 Update the integral term and limit it
    if (Integral > Integral_Max) Integral = Integral_Max;               //积分限幅 Integral limiting
    if (Integral < -Integral_Max) Integral = -Integral_Max;             //积分限幅 Integral limiting

    // 位置式 PID
    result = dir_kp * error +dir_ki*Integral+ dir_kd * (error - err_last);
  
	 err_last = error;       // 更新积分项，并进行限幅 Update the integral term and limit it

    // 对输出进行限幅Output limiting value
    if (result > Integral_Max) result = pid_max;  
    if (result < -Integral_Max) result = -pid_max;
	printf("result:%3.2f\r\n",result);
    return -result;
}
//将航向角限制为 0-360 度（防止因加减运算导致航向角范围超过 0-360 度）
//Limit the heading_angle to 0-360 degrees(to prevent the range of heading_angle over 0-360 degrees beacuse of Addition or subtraction operations )
float navigetion_0_360_limit(float angle)
{
		float temp = 0;
		 if(angle > 360)
		{
			temp = angle - 360;
		}
		else
		{
			temp = angle;
		}
		return temp;
}

//计算 0-360 导航坐标系中的小圆弧偏差（逆时针方向的负圆弧角度为正，顺时针方向的为负）
//Calculate the minor arc deviation in the 0-360 Navigation Coordinate System (Counterclockwise negative arc Angle is Positive, Clockwise is Negative)  
//计算 0-360 导航坐标系中的小圆弧偏差（逆时针方向的负圆弧角度为正，顺时针方向的为负）
//Calculate the minor arc deviation in the 0-360 Navigation Coordinate System (Counterclockwise negative arc Angle is Positive, Clockwise is Negative)  
float get_minor_arc(float azimuth,float headingAngle)  
{
    float angle_err = 0.0; 
    if(azimuth >= 180 + headingAngle)  
    {
        angle_err = azimuth - headingAngle - 360;  
    }
    else if(headingAngle > 180 + azimuth)
    {
        angle_err = azimuth - headingAngle + 360;
    }
    else
    {
        angle_err =  azimuth - headingAngle;
    }
    return -angle_err;
}


 /**************************************************************************
Function: Get angle
Input   : way：The algorithm of getting angle 1：DMP  2：kalman  3：Complementary filtering
Output  : none
函数功能：获取角度	
入口参数：way：获取角度的算法 1：DMP  2：卡尔曼 3：互补滤波
返回  值：无
**************************************************************************/	
void Get_Angle(uint8_t way)
{ 

	if(way==1)                           //DMP的读取在	数据采集中断读取，严格遵循时序要求
	{	
		if( mpu_dmp_get_data(&pitch,&roll,&yaw) == 0 )
        { 
          //  printf("\r\npitch =%d\r\n", (int)pitch);
           // printf("\r\nroll =%d\r\n", (int)roll);
//            printf("\r\nyaw =%3.2f\r\n", yaw);
        }      
        delay_ms(10);//根据设置的采样率，不可设置延时过大
	}			
	else
	{
		
		MPU6050ReadGyro(angle);
		MPU6050ReadAcc(accel);
		Accel_Angle_X=atan2(accel[0],accel[2])*180/Pi;     //计算倾角，转换单位为度	
		Accel_Angle_Y=atan2(accel[1],accel[2])*180/Pi;     //计算倾角，转换单位为度
		accel_z=accel[2]*1.962/32768;
		gyro_z = angle[2] * 2000 / 32768; // 陀螺仪量程转换
		delay_ms(10);
		if(Way_Angle==2)		  	
		{
			pitch= -Kalman_Filter_x(Accel_Angle_X,gyro_x);//卡尔曼滤波
			roll = -Kalman_Filter_y(Accel_Angle_Y,gyro_y);


		}
		else if(Way_Angle==3) 
		{  
			 pitch = -Complementary_Filter_x(Accel_Angle_X,gyro_x);//互补滤波
			 roll = -Complementary_Filter_y(Accel_Angle_Y,gyro_y);
		}
		Angle_Balance=pitch;                              //更新平衡倾角
		Gyro_Turn=angle[2];                                 //更新转向角速度
		Acceleration_Z=accel[2];                           //更新Z轴加速度计
		
	}

} 
//PID结合mpu6050控制走直线 PID control combined with MPU6050 for straight-line control
// 注意：此函数需要根据当前项目的电机控制接口进行适配
// Note: This function needs to be adapted according to the motor control interface of the current project
void mpu6050_Line()
{
	static float first_yaw;
	if(yaw_flag==0)
	{
		first_yaw =calibratedYaw;
		object_yaw  = navigetion_0_360_limit(first_yaw);	
		balance_yaw	=get_minor_arc(object_yaw,calibratedYaw);
		yaw_flag=1;
	}
	if(yaw_flag==1){
		yaw_out=Dir_PID(balance_yaw);
		// TODO: 根据当前项目的电机控制接口调用电机控制函数
		// 原接口: Set_PID_Motor(53 ,53,yaw_out);
		// 可以使用Motor_Set函数或其他电机控制函数替代
		// Motor_Set(true, base_speed + yaw_out, true, base_speed - yaw_out);
	}
}
// 此函数提供了一个使用按键控制走直线的示例
// This function provides an example of using the button to control straight-line movement
// 注意：此函数需要根据当前项目的按键接口进行适配
// Note: This function needs to be adapted according to the button interface of the current project
void BSP_Loop(void)
{
	static uint8_t send_key1 = 0;

	// 使用按键，按一下开启走直线和停止之间切换
	// Use the button to toggle between straight-line movement and stop.
	// TODO: 使用当前项目的按键函数替换 Key1_State
	// 示例: if (KEY_Scan())
	/*
	if (KEY_Scan())
	{
		switch (send_key1)
		{
		case 1:
			mpu6050_Line();
			break;
		case 0:
		default:
			break;
		}
		send_key1 = (send_key1 + 1) % 2;
	}
	*/
}

