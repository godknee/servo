 #include <string.h>

#include "app.h"
#include "drv_uart.h"
#include "controller.h"
#include "protocal_process.h"
#include "mem_ctl_table.h"
#include "pid.h"

static void clock_init(void);

uint16_t get_adc_value(ADC1_Channel_TypeDef ADC1_Channel);
void adc_init();
void timer1_init(void);
uint16_t get_motor_now_position(void);
uint16_t get_motor_current(void);

uint32_t i=50;
uint32_t max_current_limit_cnt=0;
uint16_t filter_adc = 0;
uint16_t raw_adc = 0;
unsigned short filter(void);
extern struct pid motor_pid;
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    clock_init();
    protocal_process_init();
    servo_mem_ctl_table_init();
    controller_init(); 
    /* for test */
//    adc_init();
//    timer1_init();
    /* for test */

    while (1)
    {
        protocal_decode_process();
        
        controller_run();
        
//        filter_adc = filter();
//        protocal_send((uint8_t *)(&filter_adc),2); 
//        raw_adc = get_adc_value(ADC1_CHANNEL_2);
//        protocal_send((uint8_t *)(&raw_adc),2); 
        
        // about 1K
//        if(i-- == 0){
//            i=50;
//            *((uint16_t *)(&mem_ctl_table[THE_CURRENT_2BYTES])) = get_adc_value(ADC1_CHANNEL_3);
////            protocal_send((uint8_t *)(&mem_ctl_table[THE_CURRENT_2BYTES]),2);           
//        }
        
        // current limit 
//        if(*((uint16_t *)(&mem_ctl_table[THE_CURRENT_2BYTES]))>*((uint16_t *)(&mem_ctl_table[MAX_CURRENT_LIMIT_2BYTES]))){
//          if(max_current_limit_cnt++>10000){ //0x0300;  >1.9A about 4~6s 2021-01-23
//            max_current_limit_cnt = 0;
//            motor_pid.enable = 0;
//          }  
//        }else{
//          if(max_current_limit_cnt>0){
//            max_current_limit_cnt -= 1;
//          }
//        }
        
        
    } 
}

static void clock_init(void)
{
  /* Clear High speed internal clock prescaler */
  CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
  /* Set High speed internal clock prescaler */
  CLK->CKDIVR |= (uint8_t)CLK_PRESCALER_HSIDIV1;
}
