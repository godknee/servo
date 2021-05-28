#include "motor.h"
#include "drv_tim.h"
#include "drv_adc.h"
#include "filter.h"
#include "mem_ctl_table.h"

//int8_t motor_install_dir = 1;

void motor_init(void)
{
    timer1_init(); // PWM
    adc_init();    // for position measurement and motor current measurement

    /* Init the filter buffer */
    for(uint8_t i=0;i<20;i++){
        filter_again(filter());
    }
}

void motor_run_forward(short pwm_width)
{
  if(*((uint8_t *)(&mem_ctl_table[MOTOR_INSTALL_DIRECTION]))==0){
    TIM1_SetCompare2(800-pwm_width);
    TIM1_SetCompare1(800);
  }else{
    TIM1_SetCompare2(800);
    TIM1_SetCompare1(800-pwm_width);
  }
  
  /* right direction for factory 2020-12-29 */
//  TIM1_SetCompare2(800-pwm_width);
//  TIM1_SetCompare1(800);
}

void motor_run_backward(short pwm_width)
{
  if(*((uint8_t *)(&mem_ctl_table[MOTOR_INSTALL_DIRECTION]))==0){
    TIM1_SetCompare2(800);
    TIM1_SetCompare1(800-pwm_width);
  }else{
    TIM1_SetCompare2(800-pwm_width);
    TIM1_SetCompare1(800);
  }
  
    /* right direction for factory 2020-12-29 */
//    TIM1_SetCompare2(800);
//    TIM1_SetCompare1(800-pwm_width);
}

void motor_stop(void)
{
  TIM1_SetCompare2(800);
  TIM1_SetCompare1(800);
}

//uint16_t get_motor_now_position(void)
//{
//    static uint16_t filter_buff[8];
//    static uint8_t index=0;
//    
//    uint16_t a = filter_average(void);
//    
//    static uint16_t filter_buff_a[4];
//    static uint8_t index_a=0;
//
//    return filter_average(filter_buff_a,4,&index_a,a);
////    return get_adc_value(ADC1_CHANNEL_2);
//}

//uint16_t get_motor_current(void)
//{
//    static uint16_t filter_buff[10];
//    static uint8_t index=0;
//    
//    return filter_average(filter_buff,10,&index,get_adc_value(ADC1_CHANNEL_3));
//    
////    return get_adc_value(ADC1_CHANNEL_3);
//}
