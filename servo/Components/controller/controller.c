#include <stdlib.h>
#include <string.h>

#include "controller.h"
#include "drv_adc.h"
#include "drv_tim.h"
#include "pid.h"
#include "motor.h"
#include "mem_ctl_table.h"
#include "filter.h"

/*
 * Angle=1, L=25cm : 0.44cm
 * Angle=2, L=25cm : 0.87cm
 * Angle=3, L=25cm : 1.30cm
 * Angle=4, L=25cm : 1.74cm
 * Angle=5, L=25cm : 2.18cm
 */

#define MOTOR_FULL_ANGLE 200
#define POTENTIOMETER_FULL_ANGLE 300
#define OFFSET ((POTENTIOMETER_FULL_ANGLE-MOTOR_FULL_ANGLE)/2)
#define POTENTIOMETER_EACH_DEGREE_REPRESENT_NUM (1024.0/POTENTIOMETER_FULL_ANGLE)

struct pid motor_pid;

volatile uint16_t temp_target_position = 0;
uint16_t current_position_degree = 0;
uint16_t current_position_adc = 0;
uint16_t begin_position_adc = 0;
float target_position_degree = 0;
uint16_t target_position_adc = 0;
uint16_t run_time=0; //ms
volatile uint16_t step_time=10; //ms
int pwm_width = 0;
uint16_t timer_cnt = 0;
uint16_t err = 0;
volatile uint16_t last_position_adc = 0;
uint16_t max_stall_cnt=0;
//uint32_t my_i=0;

int16_t err_raw = 0;
uint8_t vibrate_cnt = 0;
int8_t vibrate_dir = 0;

static void target_position_interpolation(void);

void controller_init(void)
{
    motor_init();
//    timer2_init(); // for callback
    pid_struct_init(&motor_pid,800, 0, 
                    30/*Kp 12 20 30 40*/, 
                    0 /*Ki*/, 
                    0 /*Kd*/);
    
    last_position_adc = target_position_adc = get_adc_value(ADC1_CHANNEL_2);
}

uint32_t fang_dou = 0;

void controller_run(void)
{
    /* it is the angle of potentiometer,ADC value */
      current_position_adc = filter_again(filter());
    
//    *((uint16_t *)(&mem_ctl_table[CURRENT_SPEED_2BYTES])) = current_position_adc - last_position_adc;
//    
//    last_position_adc = current_position_adc;
    
    err_raw = current_position_adc-target_position_adc;
    
    err = abs(err_raw);
    
    if(fang_dou++>50000){
      fang_dou=50001;
        if((err_raw>20)||(err_raw<-20)){
            vibrate_cnt = 0;
            vibrate_dir = 0;
        }else if((err_raw>2) && (err_raw<=20) && (vibrate_dir != 1)){
            vibrate_cnt++;
            vibrate_dir = 1;
        }else if((err_raw<-2) && (err_raw>=-20) && (vibrate_dir != -1)){
            vibrate_cnt++;
            vibrate_dir = -1;
        }
        
        if(vibrate_cnt>3){
//            motor_pid.param.p=10;
            goto STOP;
        }
    }
    
    
//    if(my_i++ > 50000){
//      my_i = 0;
//        // stall protect
//        if((abs(current_position_adc - last_position_adc)<=1) && (err>=20)){
//          motor_pid.enable=0;
//        }
//        last_position_adc = current_position_adc;
//    }
    

    if(err<=10){
        motor_pid.param.p=12;
    }else if(err==11){
        motor_pid.param.p=15;
    }else if(err==12){
        motor_pid.param.p=19;
    }else if(err==13){
        motor_pid.param.p=24;
    }else{
        motor_pid.param.p=30;
    }
    
    if(run_time>0){
        target_position_interpolation();
        pwm_width = pid_calculate(&motor_pid, 
                              current_position_adc, 
                              temp_target_position);
    }else{
        pwm_width = pid_calculate(&motor_pid, 
                              current_position_adc, 
                              target_position_adc);
    }
    
    if(pwm_width<-10){
      motor_run_backward(-pwm_width);
    }else if(pwm_width>10){
      motor_run_forward(pwm_width);
    }
    else{
STOP:
//      my_i=0;
      motor_stop();
    }
}

#define step_adc 2
void target_position_interpolation(void)
{
    if(timer_cnt++ >= run_time){
        timer_cnt=0;

        /*if(current_position_adc < target_position_adc){
            if(target_position_adc > (current_position_adc+step_adc)){
                temp_target_position += step_adc;
                
                 if(temp_target_position>1022){
                    temp_target_position=1023;
                }
            }else{
                temp_target_position = target_position_adc;
            }
        }else if(current_position_adc > target_position_adc){
            if(target_position_adc < (current_position_adc-step_adc)){
                temp_target_position -= step_adc;
                
                if(temp_target_position<5){
                    temp_target_position=5;
                }
                
            }else{
                temp_target_position = target_position_adc;
            }
        }else{
            temp_target_position = target_position_adc;
        }*/
        
        if(begin_position_adc < target_position_adc){
            temp_target_position += step_adc;
                
            if(temp_target_position>target_position_adc){
                temp_target_position=target_position_adc;
            }
        }else if(begin_position_adc > target_position_adc){
            temp_target_position -= step_adc;
                
            if(temp_target_position<target_position_adc){
                temp_target_position=target_position_adc;
            }
        }
        
    }
}

