#include "app_irtracking_eight.h"

double Velocity_PWM1,Velocity_PWM2;
double basicspeed=2500;
double Kp = 1.65, Ki = 0,Kd = 0.05;
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

int8_t Direct_Read(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8)
{
	int turn_direact = 0;
	turn_direact = x1  + x2 + x3  + x4 + x5 + x6 + x7 + x8;
	return turn_direact;
}

void error_get(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8)
{
	int error1=50,error2=150,error3=500;	
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
	
	*left_speed = (int)Velocity_PWM1;
	*right_speed = (int)Velocity_PWM2;

    left.last = left.now;
    right.last = right.now;
    left.now    = 0;
    right.now = 0;
}

int* LineCheck(void)
{
    static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
	
	x1 = x1 > 1 ? 1 : x1;
	x2 = x2 > 1 ? 1 : x2;
	x3 = x3 > 1 ? 1 : x3;
	x4 = x4 > 1 ? 1 : x4;
	x5 = x5 > 1 ? 1 : x5;
	x6 = x6 > 1 ? 1 : x6;
	x7 = x7 > 1 ? 1 : x7;
	x8 = x8 > 1 ? 1 : x8;

	int left = x1 + x2 + x3 + x4;
	int right = x5 + x6 + x7 + x8;
	static int dir[2] = {0, 0};
	dir[0] = left;
	dir[1] = right;
	return dir;
}

