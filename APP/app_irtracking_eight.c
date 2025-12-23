#include "app_irtracking_eight.h"

#define IRTrack_Trun_KP (275)
#define IRTrack_Trun_KI (0.02) 
#define IRTrack_Trun_KD (80) 

int pid_output_IRR = 0;
u8 trun_flag = 0;

// 记录上一时刻的电机速度
static int16_t last_speed_L = 0;
static int16_t last_speed_R = 0;

// 转弯状态标志：0=正常巡线，1=左转中，2=右转中
static u8 turning_state = 0;
// 转弯预检测标志：0=无，1=检测到左侧黑，2=检测到右侧黑
static u8 turn_pre_state = 0;
// 预检测超时计数器，防止预检测状态保持过久
static u8 pre_state_timeout = 0;
#define PRE_STATE_TIMEOUT_MAX 50  // 预检测状态最多保持次数
// 全白检测计数器
static u8 all_white_counter = 0;
#define ALL_WHITE_THRESHOLD 1  // 只需检测1次全白就判定为转弯

#define IRR_SPEED 			 450  //巡线速度	Line patrol speed
#define TURN_SPEED            500  //转弯速度（原地转向）

// 转弯控制函数 Turn control function
// 返回值：1表示正在转弯，0表示不需要转弯
u8 TurnControl_read(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8)
{
	int turn_direact = 0;
	turn_direact = x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8;
	return turn_direact;
}
u8 TurnControl_read_left(u8 x1, u8 x2, u8 x3, u8 x4)
{
	int turn_direact_left = 0;
	turn_direact_left = x1 + x2 + x3 + x4;
	return turn_direact_left;
}

u8 TurnControl_read_right(u8 x5, u8 x6, u8 x7, u8 x8)
{
	int turn_direact_right = 0;
	turn_direact_right = x5 + x6 + x7 + x8;
	return turn_direact_right;
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
	int16_t current_speed_L, current_speed_R;
	
	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
	
	// 检测是否全黑（所有传感器都为0，黑底白线下全黑表示离开赛道）
	u8 all_black = (x1==0 && x2==0 && x3==0 && x4==0 && x5==0 && x6==0 && x7==0 && x8==0);
	
	// 放宽左侧白线检测：左侧3个为白（更容易触发）
	u8 left_white = ((x1==1 || x2==1 || x3==1) && (x1+x2+x3 >= 2) && x7==0 && x8==0);
	
	// 放宽右侧白线检测：右侧3个为白（更容易触发）
	u8 right_white = ((x6==1 || x7==1 || x8==1) && (x6+x7+x8 >= 2) && x1==0 && x2==0);
	
	// 预检测状态超时处理
	if(turn_pre_state != 0)
	{
		pre_state_timeout++;
		if(pre_state_timeout > PRE_STATE_TIMEOUT_MAX)
		{
			turn_pre_state = 0;  // 超时清零
			pre_state_timeout = 0;
		}
	}
	
	// 如果当前处于转弯状态
	if(turning_state != 0)
	{
		// 检查是否转到中心位置（中间传感器检测到白线）
		if(x4 == 1 || x5 == 1)  // 中心传感器检测到白线
		{
			// 转到中心位置，退出转弯状态
			turning_state = 0;
			turn_pre_state = 0;
			all_white_counter = 0;
			pre_state_timeout = 0;
			// 不立即返回，继续执行后续的巡线逻辑，让过渡更平滑
		}
		else
		{
			// 继续转弯（原地转向）
			if(turning_state == 1)  // 左转
			{
				Motion_Set_Speed(-TURN_SPEED, TURN_SPEED);  // 左轮反转，右轮正转
			}
			else if(turning_state == 2)  // 右转
			{
				Motion_Set_Speed(TURN_SPEED, -TURN_SPEED);  // 左轮正转，右轮反转
			}
			return;
		}
	}
	
	// 转弯预检测：记录检测到左侧白或右侧白
	if(left_white)
	{
		turn_pre_state = 1;
		all_white_counter = 0;
		pre_state_timeout = 0;
		// 预判断转弯，提前减速并预转
		Motion_Car_Control(IRR_SPEED * 0.75, 0, -200);  // 轻微减速，加大左转力矩
		return;
	}
	else if(right_white)
	{
		turn_pre_state = 2;
		all_white_counter = 0;
		pre_state_timeout = 0;
		// 预判断转弯，提前减速并预转
		Motion_Car_Control(IRR_SPEED * 0.75, 0, 200);  // 轻微减速，加大右转力矩
		return;
	}
	
	// 处理全黑检测（可能是转弯）
	if(all_black)
	{
		all_white_counter++;
		
		// 连续检测到全白达到阈值，判定为真正的转弯
		if(all_white_counter >= ALL_WHITE_THRESHOLD)
		{
			// 根据之前检测到的状态判断转向
			if(turn_pre_state == 1)
			{
				// 左转：原地转向
				turning_state = 1;
				turn_pre_state = 0;
				Motion_Set_Speed(-TURN_SPEED, TURN_SPEED);  // 左轮反转，右轮正转
				return;
			}
			else if(turn_pre_state == 2)
			{
				// 右转：原地转向
				turning_state = 2;
				turn_pre_state = 0;
				Motion_Set_Speed(TURN_SPEED, -TURN_SPEED);  // 左轮正转，右轮反转
				return;
			}
			else
			{
				// 没有预检测状态，可能是丢线，保持直行
				Motion_Set_Speed(IRR_SPEED, IRR_SPEED);
				return;
			}
		}
		// 未达到阈值，保持当前状态继续前进
		return;
	}
	else
	{
		// 不是全黑，重置全黑计数器（但保留预检测状态）
		all_white_counter = 0;
	}
		
	if(x3 == 1 &&  x4 == 1  && x5 == 1 && x6 == 1 )
	{
		err = 0;
		if(trun_flag == 1)
		{
			trun_flag = 0;
		}
	}
	
 else if(x1 == 1 &&  x3 == 1 && x4 == 1 && x5 == 1 && x8 == 1 )
	{
		err = 0;
	}
	else if(x1 == 0 && x2 == 0 &&x3 == 0 &&  x4 == 0  && x5 == 0 && x6  == 0 && x7 == 0 && x8 == 0 )
	{
		if(trun_flag == 0)
		{
			err = 0; 
			trun_flag = 1;
		}
	}
	
	// 转弯判断已在函数开头处理
	else if(x1 == 0 && x2 == 0  && x3 == 1 && x4 == 1 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0)
	{
		err = -30;
	}
	else if(x1 == 0 && x2 == 0  && x3 == 1 && x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0)
	{
		err = -45;
	}
	
	else if(x1 == 0 && x2 == 1  && x3 == 1 && x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0)
	{
		err = -60;
	}
	else if(x1 == 0 && x2 == 1  && x3 == 0 && x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0)
	{
		err = -75;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 0 && x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0)
	{
		err = -90;
	}

	else if(x1 == 0 && x2 == 0  && x3 == 0 && x4 == 0 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0)
	{
		err = 30;
	}
	else if(x1 == 0 && x2 == 0  && x3 == 0 && x4 == 0 && x5 == 0 && x6 == 1  && x7 == 0 && x8 == 0)
	{
		err =45;
	}
	else if(x1 == 0 && x2 == 0  && x3 == 0 && x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 0)
	{
		err = 60;
	}
	else if(x1 == 0 && x2 == 0  && x3 == 0 && x4 == 0 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 0)
	{
		err = 75;
	}
	else if(x1 == 0 && x2 == 0  && x3 == 0 && x4 == 0 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1)
	{
		err = 90;
	}
	
 
	else if(x1 == 0 &&x2 == 0 &&x3 == 0 && (x4 == 1 || x5 == 1) && x6 == 0 && x7 == 0&& x8 == 0)
	{
		err = 0;
	}
	
	
	
	//剩下的就保持上一个状态	The rest will remain in the previous state
	pid_output_IRR = (int)(APP_IR_PID_Calc(err));
	
	// 限幅
	if(pid_output_IRR > (MAX_SPEED - MOTOR_DEAD_ZONE))
		pid_output_IRR = (MAX_SPEED - MOTOR_DEAD_ZONE);
	if(pid_output_IRR < (MOTOR_DEAD_ZONE - MAX_SPEED))
		pid_output_IRR = (MOTOR_DEAD_ZONE - MAX_SPEED);

	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);
	
	// 计算并保存本次的左右电机速度（根据差速转向公式）
	// speed_L = Vx + Vz * APB, speed_R = Vx - Vz * APB
	// 这里简化计算：Vx = IRR_SPEED, Vz对应的旋转分量通过pid_output_IRR体现
	float robot_APB = Motion_Get_APB();
	float speed_spin = (pid_output_IRR / 1000.0f) * robot_APB;
	current_speed_L = IRR_SPEED + speed_spin;
	current_speed_R = IRR_SPEED - speed_spin;
	
	// 更新上一时刻的速度记录
	last_speed_L = current_speed_L;
	last_speed_R = current_speed_R;

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

