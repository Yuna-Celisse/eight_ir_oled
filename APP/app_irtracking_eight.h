#ifndef _APP_IRTRACKING_H_
#define _APP_IRTRACKING_H_


#include "AllHeader.h"

#define LINE_BLACK       1        //黑线black
#define LINE_WHITE       0        //白线white

typedef struct {
	int now;
	int last;
	int sum;
}pid;	

void LineWalking(void);
int LineCheck(void);
void deal_IRdata(u8 *x1,u8 *x2,u8 *x3,u8 *x4,u8 *x5,u8 *x6,u8 *x7,u8 *x8);
uint8_t Direct_Read(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8);
void setspeed_pid(int *left_speed, int *right_speed);
//int16_t track_read(u8 x1, u8 x2, u8 x3, u8 x4, u8 x5, u8 x6, u8 x7, u8 x8);
//uint8_t Middle_TrackRead(u8 x4, u8 x5);


#endif
