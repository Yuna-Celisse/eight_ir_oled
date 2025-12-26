#include "app_irtracking_eight.h"

#define IRTrack_Trun_KP (4)
#define IRTrack_Trun_KI (0) 
#define IRTrack_Trun_KD (1.5) 

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

// 转弯控制函数 Turn control function
// 返回值：1表示正在转弯，0表示不需要转弯
uint8_t Direct_Read(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8)
{
	int turn_direact = 0;
	turn_direact = x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8;
	return turn_direact;
}

int16_t track_read(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8)
{
	int16_t err;
	err = 30*x1 + 15*x2 + 5*x3 + 0.2*x4 + 0.2*x5 - 5*x6 - 15*x7 - 30*x8;
	return err;
}


//检测现在位于黑线还是在白线上	Detection is now on the black line or on the white line
int LineCheck(void)
{
	int if_have = 0;
    static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
	//Traversing the grayscale sensor
	if(!x1)  // 修改：当传感器为1时认为检测到黑线
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
		  return WHITE;
	}
	else
	{
		  return BLACK;
	}
}

