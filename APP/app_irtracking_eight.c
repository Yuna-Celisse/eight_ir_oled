#include "app_irtracking_eight.h"

#define IRTrack_Trun_KP (200)
#define IRTrack_Trun_KI (0.02) 
#define IRTrack_Trun_KD (0) 

int pid_output_IRR = 0;
u8 trun_flag = 0;


#define IRR_SPEED 			  200  //巡线速度	Line patrol speed
#define TURN_SPEED            300  //转弯速度 Turn speed

// 转弯控制函数 Turn control function
// 返回值：1表示正在转弯，0表示不需要转弯
u8 TurnControl(u8 x1, u8 x2, u8 x7, u8 x8)
{
	// 右转条件：右侧传感器检测到线，左侧未检测到
	// Right turn: right sensor detects line, left side clear
	if((x1 == 1 || x2 == 1) && x8 == 0) 
	{
		// 右转：左电机正转，右电机停止
		// Right turn: left motor forward, right motor stop
		Motion_Set_Speed(TURN_SPEED, 0);
		return 1;
	}
	// 左转条件：左侧传感器检测到线，右侧未检测到
	// Left turn: left sensor detects line, right side clear
	else if((x7 == 1 || x8 == 1) && x1 == 0) 
	{
		// 左转：右电机正转，左电机停止
		// Left turn: right motor forward, left motor stop
		Motion_Set_Speed(0, TURN_SPEED);
		return 1;
	}
	
	return 0; // 不需要转弯 No turn needed
}

float APP_IR_PID_Calc(int8_t actual_value)
{

	float IRTrackTurn = 0;
	int8_t error;
	static int8_t error_last=0;
	static float IRTrack_Integral;//积分	integral
	
	error=actual_value;
	
	IRTrack_Integral +=error;
	
	//位置式pid	Position pid
	IRTrackTurn=error*IRTrack_Trun_KP
							+IRTrack_Trun_KI*IRTrack_Integral
							+(error - error_last)*IRTrack_Trun_KD;
	


	if (IRTrackTurn > (MAX_SPEED - MOTOR_DEAD_ZONE))
			IRTrackTurn = (MAX_SPEED - MOTOR_DEAD_ZONE);
	if (IRTrackTurn < (MOTOR_DEAD_ZONE - MAX_SPEED))
			IRTrackTurn = (MOTOR_DEAD_ZONE - MAX_SPEED);
	return IRTrackTurn;
}

 
 

void deal_IRdata(u8 *x1,u8 *x2,u8 *x3,u8 *x4,u8 *x5,u8 *x6,u8 *x7,u8 *x8)
{

	
*x1 = IR_Data_number[0];
*x2 = IR_Data_number[1];
*x3 = IR_Data_number[2];
*x4 = IR_Data_number[3];
*x5 = IR_Data_number[4];
*x6 = IR_Data_number[5];
*x7 = IR_Data_number[6];
*x8 = IR_Data_number[7];
}


void LineWalking(void)
{
	static int8_t err = 0;
	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	
	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
	
	// 优先处理转弯 Priority: handle turns first
	if(TurnControl(x1, x2, x7, x8))
	{
		return; // 正在转弯，直接返回 Turning, return directly
	}
		
	if(x3 == 0 &&  x4 == 0  && x5 == 0 && 6 == 0 ) //俩边都亮，直跑	Both sides are bright, run straight
	{
		err = 0;
		if(trun_flag == 1)
		{
			trun_flag = 0;//走到圈了	Got into the circle
		}
	}
	
	//优先判断	Priority judgment
 else if(x1 == 0 &&  x3 == 0 && x4 == 0 && x5 == 0 && x8 == 0 )
	{
		err = 0;
	}
	else if(x1 == 1 && x2 == 1 &&x3 == 1 &&  x4 == 1  && x5 == 1 && x6  == 1 && x7 == 1 && x8 == 1 ) //过锐角	Over sharp angle
	{
		if(trun_flag == 0) //出线了	Qualified
		{
			err = 0; 
			trun_flag = 1;
		}
		//其它情况保持上个状态	Stay in the previous state in other situations
	}
	
	// 转弯判断已在函数开头处理 Turn conditions handled at function start

	// else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
	// {
	// 	err = -10;
	// }
	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
	{
		err = -20;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1101 1111
	{
		err = -30;  // L3触发修正
	}
	
	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
	{
		err = -40;  // L2触发更大修正
	}
	// else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
	// {
	// 	err = -50;  // L1触发更大修正
	// }

	// else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
	// {
	// 	err = 10;
	// } 
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
	{
		err = 20;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 1011
	{
		err =30;  // L6触发修正
	}
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
	{
		err = 40;  // L7触发更大修正
	}
	// else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
	// {
	// 	err = 50;  // L7触发更大修正
	// }
	
 
	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && (x4 == 0 || x5 == 0) && x6 == 1 && x7 == 1&& x8 == 1) //直走	Go straight
	{
		err = 0;
	}
	
	
	
	//剩下的就保持上一个状态	The rest will remain in the previous state
	pid_output_IRR = (int)(APP_IR_PID_Calc(err));

	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);

}

//检测现在位于黑线还是在白线上	Detection is now on the black line or on the white line
int LineCheck(void)
{
	int if_have = 0;
    static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
	//Traversing the grayscale sensor
	if(!x1)
	{
		if_have = 1;
	}
    if(!x2)
	{
		if_have = 1;
	}
    if(!x3)
	{
		if_have = 1;
	}
	if(!x4)
	{
		if_have = 1;
	}
	if(!x5)
	{
		if_have = 1;
	}
	if(!x6)
	{
		if_have = 1;
	}
    if(!x7)
	{
		if_have = 1;
	}
    if(!x8)
	{
		if_have = 1;
	}
		
	//If a black line is detected
	if(if_have >= 1) 
	{
		  return BLACK;
	}
	else
	{
		  return WHITE;
	}
}




