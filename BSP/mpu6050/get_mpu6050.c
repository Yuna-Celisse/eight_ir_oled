#include "get_mpu6050.h"
#include "Filter.h"

#define Pi 3.14159265
volatile short angle[3];
volatile short accel[3];
volatile float pitch,roll,yaw;   //欧拉角 Euler Angles

volatile short gyrox,gyroy,gyroz;				//陀螺仪--角速度

volatile short aacx,aacy,aacz;				//陀螺仪--角速度

// 新增：全局变量定义
volatile float Filter_out = 0;
volatile float Yaw_Err_Lowout_last = 0;
volatile float Yaw_Err_Lowout = 0;

void Get_EulerAngles(void)
{
    //获取欧拉角 Get Euler angles
    mpu_dmp_get_data(&pitch,&roll,&yaw);
	// 更新滤波后的yaw值
	Filter_out = get_Filter(yaw);
}

// 新增：低通滤波函数
float get_Filter(float Yaw_Err)	
{
	float a=0.630;
	Yaw_Err_Lowout=(1-a)*Yaw_Err+a*Yaw_Err_Lowout_last;//使得波形更加平滑，滤除高频干扰
	Yaw_Err_Lowout_last=Yaw_Err_Lowout;
	return Yaw_Err_Lowout;
}

// 新增：小圆弧算法
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
