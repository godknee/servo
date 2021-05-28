#include <stdlib.h>

#include "cascade_controller.h"
#include "motor.h"
#include "drv_tim.h"
#include "pid.h"
#include "mem_ctl_table.h"


struct pid motor_position_pid;
struct pid motor_speed_pid;

void cascade_controller_init(void)
{
    motor_init();
    timer2_init(); // for callback
    
    /* position */
    pid_struct_init(&motor_position_pid,400, 0, 
                    2 /*Kp*/, 
                    0 /*Ki*/, 
                    0 /*Kd*/);
    
    /* speed */
    pid_struct_init(&motor_speed_pid,400, 0, 
                    2 /*Kp*/, 
                    0 /*Ki*/, 
                    0 /*Kd*/);
}

void cascade_controller_run(void)
{
  
    static short last_position;
    short position = get_motor_current_position();
    
    /* position */
    short speed = pid_calculate(&motor_position_pid, 
                              position, 
                              (uint16_t)(&mem_ctl_table[TARGET_POSITION_2BYTES]));
    /* speed */
    short pwm_width = pid_calculate(&motor_speed_pid,
                            position-last_position,
                            speed);
    motor_run(pwm_width);
    
    last_position = position;
}