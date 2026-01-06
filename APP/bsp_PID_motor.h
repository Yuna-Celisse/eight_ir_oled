#ifndef __BSP_PID_MOTOR_H
#define __BSP_PID_MOTOR_H

#include "AllHeader.h"

typedef struct {
		bool L_or_R;
		float KP;
		float KI;
		float KD;
		int output;
		int error;
		int sum_error;
		int pre_error;
		int limit_sum_error;
		int limit_output;
}PID;

void pid_control_speed(int current, float target, PID *pid);
float pid_control_track(int track_error, PID *pid);

extern PID SPEED_PID_L;
extern PID SPEED_PID_R;
extern PID TRACK_PID;
extern int8_t SPEED;

#endif
