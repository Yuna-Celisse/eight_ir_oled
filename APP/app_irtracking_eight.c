#include "app_irtracking_eight.h"

int Velocity_PWM1,Velocity_PWM2;
int basicspeed=2500;
double Kp = 3.5, Ki = 0,Kd = 0;
pid right  = {0,0,0};
pid left    = {0,0,0};

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
int8_t Direct_Read(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8)
{
	int turn_direact = 0;
	turn_direact = x1  + x2 + x3  + x4 + x5 + x6 + x7 + x8;
	return turn_direact;
}

//int16_t track_read(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8)
//{
//	int16_t err;
//	err = 25*x1 + 10*x2 + 1*x3 + 0.5*x4 - 0.5*x5 - 1*x6 - 10*x7 - 25*x8;
//	return err;
//}

//uint8_t Middle_TrackRead(u8 x4, u8 x5)
//{
//	int middle_trackRead = 0;
//	middle_trackRead = x4 + x5;
//	return middle_trackRead;
//}

void error_get(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8)
{
//	int error0 = 50, 
	int error1=50,error2=150,error3=500;	
//	if( x5 == 1 )
//	{
//		left.now    -= error0;
//    right.now   += error0;
//	}
//	if( x4 == 1 )
//	{
//		left.now    += error0;
//    right.now   -= error0;
//	}
	if( x6 == 1 )
	{
		    left.now    -= error1;
        right.now   += error1;
	}
	if( x3== 1 )
	{
        left.now   += error1;
        right.now  -= error1;
	}
	if( x7 == 1 )
	{
        left.now    -= error2;
        right.now   += error2;
	}
	if( x2 == 1 )
	{
        left.now    += error2;
        right.now   -= error2;
		
	}
	if( x8 == 1 )
	{
		    left.now    -= error3;
        right.now   += error3;
	}
	if( x1 == 1 )
	{
        left.now   += error3;
        right.now  -= error3;
	}
}

void setspeed_pid(int *left_speed, int *right_speed)
{
	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
	error_get(x1,x2,x3,x4,x5,x6,x7,x8);
	if(left.sum >  1000000)left.sum=   10000;
	if(left.sum < -1000000)left.sum=  -10000;

	if(right.sum >   1000000)right.sum=  10000;
	if(right.sum <  -1000000)right.sum= -10000;
		
    Velocity_PWM1 = basicspeed + Kp * right.now + Ki * right.sum + Kd * (right.now - right.last);
    Velocity_PWM2 = basicspeed + Kp * left.now  + Ki * left.sum 	+ Kd * (left.now - left.last);	
	
//	encoder_A=Read_Encoder(4);
//	encoder_B=Read_Encoder(2);  
//	PWM1=Velocity_A(Velocity_PWM1,encoder_A);
//	PWM2=Velocity_B(Velocity_PWM2,encoder_B);
//	OLED_ShowSignedNum(8,  0,encoder_A,3,OLED_8X16);
//	OLED_ShowSignedNum(8, 16,encoder_B, 3,OLED_8X16);
//	OLED_ShowSignedNum(8, 32,PWM1, 6,OLED_8X16);
//	OLED_ShowSignedNum(8, 48,PWM2, 6,OLED_8X16);
//	OLED_Update();
	*left_speed = Velocity_PWM1;
	*right_speed = Velocity_PWM2;

    left.last = left.now;
    right.last = right.now;
    left.now    = 0;
    right.now = 0;
}

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
		  return LINE_WHITE;
	}
	else
	{
		  return LINE_BLACK;
	}
}

