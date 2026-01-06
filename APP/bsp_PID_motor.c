#include "bsp_PID_motor.h"

int8_t SPEED = 24;

PID SPEED_PID_L = {
    .L_or_R = true,
    .KP = 5,
    .KI = 0.4,
    .KD = 0,
    .output = 0,
    .error = 0,
    .sum_error = 0,
    .pre_error = 0,
    .limit_sum_error = 100,
    .limit_output = 68
};

PID SPEED_PID_R = {
	.L_or_R = false,
    .KP = 5,
    .KI = 0.4,
    .KD = 0,
    .output = 0,
    .error = 0,
    .sum_error = 0,
    .pre_error = 0,
    .limit_sum_error = 100,
    .limit_output = 68
};


PID TRACK_PID = {
    .KP = 0.015,
    .KI = 0.001,
    .KD = 0,
	  .output = 0,
	  .error = 0,
	  .sum_error = 0,
    .pre_error = 0,
    .limit_sum_error = 50,
    .limit_output = 100
};
