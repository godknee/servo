#include <stdlib.h>
#include <string.h>

#include "drv_uart.h"
#include "protocal_process.h"
#include "command_interpreter.h"
#include "cyclic_queue.h"
#include "mem_ctl_table.h"

static void upload_current_value(void);

protocal_handle_t protocal_handle;

void protocal_process_init(void)
{
  rr_queue_init(&queue,300);
  uart1_init(115200);
}

void protocal_decode_process(void)
{
    while(!queue_is_empty(&queue)){
        protocal_handle.ch = queue_out(&queue);
        switch(protocal_handle.step){
            case FIND_HEAD_FIRST_FF:
                  if(protocal_handle.ch==0xFF){
                    protocal_handle.step = FIND_HEAD_SECOND_FF;
                  }
              break;
              
            case FIND_HEAD_SECOND_FF:
                  if(protocal_handle.ch==0xFF){
                    protocal_handle.step = GET_ID;
                  }else{
                    protocal_handle.step = FIND_HEAD_FIRST_FF;
                  }
              break;

            case GET_ID:
              if((protocal_handle.ch<0x04) || (protocal_handle.ch==0xFE)){ // 0xFE is broadcast addr
                  protocal_handle.id=protocal_handle.ch;
                  protocal_handle.step = GET_LENGTH;              
              }else{
                  protocal_handle.step = FIND_HEAD_FIRST_FF;
              }
              break;
              
           case GET_LENGTH:
                 if(protocal_handle.ch<=30){
                    protocal_handle.len=protocal_handle.ch;
                    protocal_handle.step = GET_CMD;
                 }else{
                    protocal_handle.step = FIND_HEAD_FIRST_FF;
                 }
              break;
              
           case GET_CMD:
                protocal_handle.cmd=protocal_handle.ch;
                if(protocal_handle.len-2 > 0){
                    protocal_handle.step = GET_PARAM;
                }else{
                    protocal_handle.step = GET_CHECKSUM; 
                }
              break;
              
            case GET_PARAM:
                protocal_handle.param[protocal_handle.index++]=protocal_handle.ch;
                if(protocal_handle.index == protocal_handle.len-2){
                    protocal_handle.step = GET_CHECKSUM;
                }
              break;
              
            case GET_CHECKSUM:
                  protocal_handle.clc_sum = protocal_handle.id+protocal_handle.len+protocal_handle.cmd;
                  
                  /* No CMD byte and sum byte, so len-2 */
                  for(uint8_t i=0;i<protocal_handle.len-2;i++){
                        protocal_handle.clc_sum += protocal_handle.param[i];
                  }
                  
                  uint8_t clc_sum = ~protocal_handle.clc_sum;
                  
                  if(protocal_handle.ch == clc_sum){
                    /*  */
                    if(protocal_handle.cmd==READ_CURRENT_VALUE){
                      if((mem_ctl_table[ID]==0x00)&&(protocal_handle.id==0x00)){
                          upload_current_value();
                      }else if(mem_ctl_table[ID]==0x01){
                          if(protocal_handle.id==0x01){
                              upload_current_value();
                          }
                      }else if(mem_ctl_table[ID]==0x02){
                          if(protocal_handle.id==0x02){
                              upload_current_value();
                          }
                      }
                    }else{
                      if((protocal_handle.id==mem_ctl_table[ID]) || (protocal_handle.id==0xFE)){
                        cmd_execute(&protocal_handle);
                      }
                    }
                  }else{
                  protocal_handle.index = 0;
                  protocal_handle.step = FIND_HEAD_FIRST_FF;
                  }
                  
                  protocal_handle.index = 0;
                  protocal_handle.step = FIND_HEAD_FIRST_FF;
              break;
        }
    }
}


void protocal_encode_process(uint8_t id, uint8_t *data, uint8_t len, uint8_t *result, uint8_t *result_len) 
{
  uint8_t i = 0;
  uint8_t check_sum = 0;
  
  result[0] = 0xFF;
  result[1] = 0xF5;
  result[2] = id;
  result[3] = len+1;
  memcpy(result+4, data, len);
  for(i=0; i<(len+2); i++) {
    check_sum += result[i+2]; 
  }
  result[len+4] = (~check_sum);
  
  *result_len = len+5;
}

uint16_t get_motor_current(void);
uint16_t get_adc_value(ADC1_Channel_TypeDef ADC1_Channel);

/*
	if(dir==1){
		if(adc<middle_adc){
			the_degree = min_degree + abs(adc-min_degree)/bottom_resolution
		}else{
			the_degree = mid_degree + abs(adc-middle_adc)/top_resolution
		}
	}else if(dir==-1){
		if(adc>middle_adc){
			the_degree = min_degree + abs(adc-min_degree)/bottom_resolution
		}else{
			the_degree = mid_degree + abs(adc-middle_adc)/top_resolution
		}
	}
*/
extern uint16_t current_position_adc;
uint16_t get_now_angle(void)
{
    uint16_t the_degree = 0;
  
//    uint16_t adc = get_adc_value(ADC1_CHANNEL_2);
    uint16_t adc = current_position_adc;
    
	if(*((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION]))==1){
		if(adc<*((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES]))){
			the_degree = (uint16_t)((*((uint16_t *)(&mem_ctl_table[MIN_ANGLE_LIMIT_2BYTES])) + abs(adc-*((uint16_t *)(&mem_ctl_table[MIN_ADC_LIMIT_2BYTES])))/(*((uint16_t *)(&mem_ctl_table[BOTTOM_RESOLUTION_2BYTES]))/100.0))*10);
		}else{
			the_degree = (uint16_t)((*((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES])) + abs(adc-*((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES])))/(*((uint16_t *)(&mem_ctl_table[TOP_RESOLUTION_2BYTES]))/100.0))*10);
		}
	}else if(*((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION]))==-1){
		if(adc>*((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES]))){
			the_degree = (uint16_t)((*((uint16_t *)(&mem_ctl_table[MIN_ANGLE_LIMIT_2BYTES])) + abs(adc-*((uint16_t *)(&mem_ctl_table[MIN_ADC_LIMIT_2BYTES])))/(*((uint16_t *)(&mem_ctl_table[BOTTOM_RESOLUTION_2BYTES]))/100.0))*10);
		}else{
			the_degree = (uint16_t)((*((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES])) + abs(adc-*((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES])))/(*((uint16_t *)(&mem_ctl_table[TOP_RESOLUTION_2BYTES]))/100.0))*10);
		}
	}
    
    return the_degree;
}


extern uint16_t raw_current_adc;

uint8_t upload_current_buff[12];
uint8_t servo_test_cnt = 0;
static void upload_current_value(void)
{
    uint8_t i = 0;
    uint8_t check_sum = 0;
//    uint16_t current = get_motor_current();
//    uint16_t current = get_adc_value(ADC1_CHANNEL_3);
//    uint16_t current =  0x5555;
//    uint16_t current = (uint16_t)(*((uint16_t *)(&mem_ctl_table[THE_CURRENT_2BYTES]))*0.66); // adc/76/0.02
    uint16_t angle = get_now_angle();
//    uint16_t angle = 0xAAAA;
    
    upload_current_buff[0]=0xFF;
    upload_current_buff[1]=0xFF;
    upload_current_buff[2]=mem_ctl_table[ID]+1;
    upload_current_buff[3]=8;
    upload_current_buff[4]=0x07; // cmd
    upload_current_buff[5]=(uint8_t)((angle>>8)&0xFF);   // hight byte
    upload_current_buff[6]=(uint8_t)(angle&0xFF);        // low byte
    upload_current_buff[7]=(uint8_t)((raw_current_adc>>8)&0xFF); // hight byte
    upload_current_buff[8]=(uint8_t)(raw_current_adc&0xFF);      // low byte
    upload_current_buff[9]=(uint8_t)((servo_test_cnt>>8)&0xFF); // hight byte
    upload_current_buff[10]=(uint8_t)(servo_test_cnt++&0xFF);      // low byte
    
    for(i=2; i<11; i++) {
        check_sum += upload_current_buff[i]; 
    }
    upload_current_buff[11] = (~check_sum);
    
    protocal_send(upload_current_buff,12); 
}

void protocal_send(uint8_t *buf,uint8_t size)
{
  while(uart_transmit_it(buf,size)!=0);
}