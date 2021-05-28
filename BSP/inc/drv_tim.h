#ifndef __TIM_H
#define __TIM_H

#include "stm8s.h"

/* The timer clock is 16MHZ, the pwm frequency is TIM_CLK/(800) */

void timer1_init(void);
void timer2_init(void);

#endif