#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm8s.h"

void motor_init(void);
void motor_run_forward(short pwm_width);
void motor_run_backward(short pwm_width);
void motor_stop(void);
uint16_t get_motor_now_position(void);
uint16_t get_motor_current(void);


#endif