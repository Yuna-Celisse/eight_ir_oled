#ifndef _APP_IRTRACKING_H_
#define _APP_IRTRACKING_H_


#include "AllHeader.h"

#define BLACK       1        //黑线black
#define WHITE       0        //白线white


#define u8 uint8_t
#define u16 uint16_t


void LineWalking(void);
int LineCheck(void);
u8 TurnControl(u8 x1, u8 x2, u8 x7, u8 x8);

#endif

