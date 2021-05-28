#include <stdlib.h>
#include <string.h>

#include "command_interpreter.h"
#include "protocal_process.h"
#include "mem_ctl_table.h"
#include "drv_eeprom.h"
#include "drv_uart.h"
#include "pid.h"
#include "drv_adc.h"
#include "filter.h"

static void servo_ping(protocal_handle_t *protocal_handle);
static void servo_read_data(protocal_handle_t *protocal_handle); 
static void servo_write_data(protocal_handle_t *protocal_handle); 
//static void servo_reg_write(protocal_handle_t *protocal_handle); 
//static void servo_action(protocal_handle_t *protocal_handle); 
//static void servo_reset(protocal_handle_t *protocal_handle); 
//static void read_current_value(protocal_handle_t *protocal_handle); 
static void servo_sync_write(protocal_handle_t *protocal_handle);
static void run_to_middle(protocal_handle_t *protocal_handle);

struct cmd_table_t cmd_table[] = {
  {PING, servo_ping},        
  {READ_DATA, servo_read_data},   
  {WRITE_DATA, servo_write_data},  
//  {REG_WRITE, servo_reg_write},   
//  {ACTION, servo_action},      
//  {RESET, servo_reset}, 
  {RUN_TO_MIDDLE, run_to_middle},
  {SYNC_WRITE, servo_sync_write},  
};

#define CMD_TBL_LEN (sizeof(cmd_table))/(sizeof(cmd_table[0]))

void cmd_execute(protocal_handle_t *protocal_handle)
{
    for(uint8_t i=0;i<CMD_TBL_LEN;i++){
      if(protocal_handle->cmd==cmd_table[i].cmd){
        cmd_table[i].callback(protocal_handle);
        break;
      }
    }

    /* if it is broadcast or ack data len is zero, return,  otherwise send ack*/
    if(protocal_handle->id == 0xFE) { // 0xFE is broadcast addr
        return;
    }

    if(protocal_handle->result_len == 0) {
        return;
    }

    /* get ack package */
    protocal_encode_process(mem_ctl_table[ID], protocal_handle->result_buf, protocal_handle->result_len, 
                        protocal_handle->send_frame_buf, &protocal_handle->send_frame_len);

    /* send ack package */
    protocal_send(protocal_handle->send_frame_buf, protocal_handle->send_frame_len);
    
    /* clear */
    protocal_handle->id = 0x00; // default ID is 0
    protocal_handle->result_len = 0;
    protocal_handle->send_frame_len = 0;
}

static void servo_ping(protocal_handle_t *protocal_handle) 
{
  protocal_handle->result_buf[0] = 0;
  protocal_handle->result_len = 1;
}

static void servo_read_data(protocal_handle_t *protocal_handle) 
{
  uint8_t read_addr     = 0;
  uint8_t read_len      = 0;
  uint8_t return_len    = 0;
  uint8_t return_status = 0;
  
  read_addr = protocal_handle->param[0];
  read_len  = protocal_handle->param[1];
  
  return_status = 0;
  
  if(read_addr >= MEM_CTL_TABLE_SIZE ) {
    return_status |= (1<<4);
  }
  
  if((read_addr > MEM_CTL_TABLE_ROM_END_ADDR) && (read_addr < MEM_CTL_TABLE_RAM_START_ADDR)) {
    return_status |= (1<<4);
  }
  
  if(read_len >= (MEM_CTL_TABLE_SIZE-read_addr)) {
    return_status |= (1<<3);
  }
  
  protocal_handle->result_buf[return_len++] = return_status;
  
  /* if error, don't need return param */
  if(return_status) {
    protocal_handle->result_len = return_len;
    return;
  }

  /* get parameters */
  memcpy(&(protocal_handle->result_buf[return_len]),&mem_ctl_table[read_addr],read_len);

  protocal_handle->result_len = return_len+read_len;
}

extern struct pid motor_pid;
extern volatile uint16_t temp_target_position;
extern float target_position_degree;
extern uint16_t target_position_adc;
extern uint16_t begin_position_adc;
extern uint16_t run_time; //ms
extern uint16_t current_position_adc;
extern uint32_t fang_dou;
uint16_t current_adc = 0;


static void servo_write_data(protocal_handle_t *protocal_handle) 
{
  uint8_t write_addr    = 0;
  uint8_t write_len     = 0;
  uint8_t return_len    = 0;
  uint8_t return_status = 0;
  
  write_addr = protocal_handle->param[0];
  write_len  = protocal_handle->len-3;
  
  return_status = 0;
  
  if(write_addr >= MEM_CTL_TABLE_SIZE) {
    return_status |= (1<<4);
  }
  
  if(write_addr > MEM_CTL_TABLE_ROM_END_ADDR && write_addr < MEM_CTL_TABLE_RAM_START_ADDR) {
    return_status |= (1<<4);
  }
  
  if(write_len >= (MEM_CTL_TABLE_SIZE - write_addr)) {
    return_status |= (1<<3);
  }
  
  if(write_addr <= MEM_CTL_TABLE_ROM_END_ADDR) {
    if(eeprom_write_data(write_addr, &(protocal_handle->param[1]), write_len) < 0) {
      return_status |= (1<<1);
    }
  }
  
  protocal_handle->result_buf[return_len++] = return_status;
  
  protocal_handle->result_len = return_len;
  
  if(return_status) {
    return;
  }
  
  memcpy(&mem_ctl_table[write_addr],&(protocal_handle->param[1]),write_len);
  
  
  if(write_addr==TARGET_POSITION_2BYTES){
    /* disenable PID control*/
    motor_pid.enable = 0;
        
    fang_dou = 0;
    
//    temp_target_position = begin_position_adc = get_adc_value(ADC1_CHANNEL_2); // current adc value
    temp_target_position = begin_position_adc = filter_again(filter());
    
    target_position_degree = *((uint16_t *)(&mem_ctl_table[TARGET_POSITION_2BYTES]))/10.0;
    
    if(target_position_degree > *((uint16_t *)(&mem_ctl_table[MAX_ANGLE_LIMIT_2BYTES]))){
        target_position_degree = *((uint16_t *)(&mem_ctl_table[MAX_ANGLE_LIMIT_2BYTES]));
	}else if(target_position_degree < *((uint16_t *)(&mem_ctl_table[MIN_ANGLE_LIMIT_2BYTES]))){
		target_position_degree = *((uint16_t *)(&mem_ctl_table[MIN_ANGLE_LIMIT_2BYTES]));
	}
    
    /* convert the angle of motor to the angle of potentiometer, ADC value */
    if(target_position_degree > *((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES]))){ 
        // min_adc + dir*(mid_degree * bottom_resolution + (degree-mid_degree)*top_resolution)
        target_position_adc = (uint16_t)( *((uint16_t *)(&mem_ctl_table[MIN_ADC_LIMIT_2BYTES]))
                              + *((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION]))
                                *((float)(*((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES])) * (*((uint16_t *)(&mem_ctl_table[BOTTOM_RESOLUTION_2BYTES]))/100.0))
                                  + (float)((target_position_degree - *((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES]))) * (*((uint16_t *)(&mem_ctl_table[TOP_RESOLUTION_2BYTES]))/100.0))) );
    }else{  
      // min_adc + dir*(degree * bottom_resolution)
        target_position_adc = (uint16_t)(*((uint16_t *)(&mem_ctl_table[MIN_ADC_LIMIT_2BYTES])) 
                              + *((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION]))
                                *(float)(target_position_degree * (*((uint16_t *)(&mem_ctl_table[BOTTOM_RESOLUTION_2BYTES]))/100.0)) );
    }
    
    /* calculate run time */
    run_time = (uint16_t)((float)(*(uint16_t *)(&mem_ctl_table[RUN_TIME_2BYTES]))/(float)abs(target_position_adc-temp_target_position)*15.0+0.5);
//    if(run_time==0){
//        motor_pid.param.p=30;
//    }else{
//        motor_pid.param.p=10;
//    }

    /* enable PID control*/
    motor_pid.enable = 1;
  }else if(write_addr==MOTOR_STOP){
    temp_target_position = target_position_adc = get_adc_value(ADC1_CHANNEL_2);
    /* disable PID control*/
    motor_pid.enable = 0;
  }else if(write_addr==MOTOR_BREAK){  
    temp_target_position = target_position_adc = get_adc_value(ADC1_CHANNEL_2);
    /* enable PID control*/
    motor_pid.enable = 1;
  }else if(write_addr==MIN_ANGLE_LIMIT_2BYTES){
    /*
     * min_adc = current_adc
     * bottom_resolution =  (middle_adc - min_adc) / (middle_angle - min_angle)
     * write bottom_resolution to eeprom
     */
    current_adc = get_adc_value(ADC1_CHANNEL_2);
    
    *((uint16_t *)(&mem_ctl_table[MIN_ADC_LIMIT_2BYTES])) = current_adc;
       
    eeprom_write_data(MIN_ADC_LIMIT_2BYTES, &mem_ctl_table[MIN_ADC_LIMIT_2BYTES], 2);
    
    *((uint16_t *)(&mem_ctl_table[BOTTOM_RESOLUTION_2BYTES])) = (uint16_t)((float)abs(*((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES])) - current_adc) 
                                                                / (float)abs(*((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES])) - *((uint16_t *)(&mem_ctl_table[MIN_ANGLE_LIMIT_2BYTES])))
                                                                * 100.0 );
    
    eeprom_write_data(BOTTOM_RESOLUTION_2BYTES, &mem_ctl_table[BOTTOM_RESOLUTION_2BYTES], 2);
    
    if(current_adc > *((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES]))){
        *((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION])) = -1;
    }else{
        *((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION])) = 1;
    }
    eeprom_write_data(MOTOR_RUN_DIRECTION, &mem_ctl_table[MOTOR_RUN_DIRECTION], 1);

  }else if(write_addr==MAX_ANGLE_LIMIT_2BYTES){
    /*
     * max_adc = current_adc
     * top_resolution = (max_adc - middle_adc) / (max_angle - middle_angle)
     * write top_resolution to eeprom
     */
    current_adc = get_adc_value(ADC1_CHANNEL_2);
    
    *((uint16_t *)(&mem_ctl_table[MAX_ADC_LIMIT_2BYTES])) = current_adc;
       
    eeprom_write_data(MAX_ADC_LIMIT_2BYTES, &mem_ctl_table[MAX_ADC_LIMIT_2BYTES], 2);

    
    *((uint16_t *)(&mem_ctl_table[TOP_RESOLUTION_2BYTES])) = (uint16_t)((float)abs(current_adc - *((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES])))
                                                             / (float)abs(*((uint16_t *)(&mem_ctl_table[MAX_ANGLE_LIMIT_2BYTES])) - *((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES]))) 
                                                             * 100.0);
    eeprom_write_data(TOP_RESOLUTION_2BYTES, &mem_ctl_table[TOP_RESOLUTION_2BYTES], 2);
    
  }else if(write_addr==MID_ANGLE_2BYTES){
    *((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES])) = get_adc_value(ADC1_CHANNEL_2);
    eeprom_write_data(MIDDLE_ADC_VALUE_2BYTES, &mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES], 2);
  }
}

static void run_to_middle(protocal_handle_t *protocal_handle)
{
    run_time = 30;

    temp_target_position = get_adc_value(ADC1_CHANNEL_2);
    target_position_adc = 512;
    
    /* enable PID control*/
    motor_pid.enable = 1;
}

//static void servo_reset(protocal_handle_t *protocal_handle) 
//{
////  uint8_t return_len    = 0;
////  uint8_t return_status = 0;
////  
////  /* TODO: reset */
////  
////  return_status = 0;
////  
////  if(eeprom_write_data(0, mem_ctl_table, MEM_CTL_TABLE_ROM_END_ADDR+1) < 0) {
////    return_status |= (1<<1); 
////  }
////  
////  protocal_handle->result_buf[return_len++] = return_status;
////  protocal_handle->result_len = return_len;
//}

static void servo_sync_write(protocal_handle_t *protocal_handle) 
{
  uint8_t write_addr    = 0;
  uint8_t write_len     = 0;
  uint8_t return_len    = 0;
  uint8_t return_status = 0;
  uint8_t i = 0;
  uint8_t servo_num     = 0;
  
  write_addr = protocal_handle->param[0];
  write_len  = protocal_handle->param[1];
  
  /* 
     write_addr is param[0]
     write_len is param[1]

     total_len = len-write_addr-write_len-checksum
     item_len = write_len + 1(id)
     
     servo_num = total_len/item_len
  */
  servo_num = (protocal_handle->len-4)/(write_len+1);
  
  for(i=0; i<servo_num; i++) {
    /* offset is 2 */
    if(mem_ctl_table[ID] == protocal_handle->param[(write_len+1)*i+2]) {
      break;
    } 
  }
  
  /* no own id, so return */
  if(i == servo_num) {
     protocal_handle->result_len = return_len;
     return;
  }
  
  return_status = 0;
  
  if(write_addr >= MEM_CTL_TABLE_SIZE) {
    return_status |= (1<<4);
  }
  
  if((write_addr > MEM_CTL_TABLE_ROM_END_ADDR) && (write_addr < MEM_CTL_TABLE_RAM_START_ADDR)) {
    return_status |= (1<<4);
  }

  if(write_len >= (MEM_CTL_TABLE_SIZE-write_addr)) {
    return_status |= (1<<3);
  }
  
  if(return_status) {
    return;
  }
  
  if(write_addr <= MEM_CTL_TABLE_ROM_END_ADDR) {
    /* offset = write_addr + write_len + id = 1+1+1 = 3 */
    if(eeprom_write_data(write_addr, protocal_handle->param+3+(write_len+1)*i, write_len) < 0) {
      return_status |= (1<<1);
    }
  }
  
  protocal_handle->result_buf[return_len++] = return_status;
  
  memcpy(&mem_ctl_table[write_addr],protocal_handle->param+3+(write_len+1)*i,write_len);
  
  if(write_addr==TARGET_POSITION_2BYTES){
    /* disenable PID control*/
    motor_pid.enable = 0;
    
    fang_dou = 0;
    
//    temp_target_position = begin_position_adc = get_adc_value(ADC1_CHANNEL_2);
    temp_target_position = begin_position_adc = filter_again(filter());

    target_position_degree = *((uint16_t *)(&mem_ctl_table[TARGET_POSITION_2BYTES]))/10.0;
    
    if(target_position_degree > *((uint16_t *)(&mem_ctl_table[MAX_ANGLE_LIMIT_2BYTES]))){
        target_position_degree = *((uint16_t *)(&mem_ctl_table[MAX_ANGLE_LIMIT_2BYTES]));
	}else if(target_position_degree < *((uint16_t *)(&mem_ctl_table[MIN_ANGLE_LIMIT_2BYTES]))){
		target_position_degree = *((uint16_t *)(&mem_ctl_table[MIN_ANGLE_LIMIT_2BYTES]));
	}    
    
    /* convert the angle of motor to the angle of potentiometer, ADC value */
    if(target_position_degree > *((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES]))){ 
        // min_adc + dir*(mid_degree * bottom_resolution + (degree-mid_degree)*top_resolution)
        target_position_adc = (uint16_t)( *((uint16_t *)(&mem_ctl_table[MIN_ADC_LIMIT_2BYTES]))
                              + *((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION]))
                                *((float)(*((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES])) * (*((uint16_t *)(&mem_ctl_table[BOTTOM_RESOLUTION_2BYTES]))/100.0))
                                  + (float)((target_position_degree - *((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES]))) * (*((uint16_t *)(&mem_ctl_table[TOP_RESOLUTION_2BYTES]))/100.0))) );
    }else{  
      // min_adc + dir*(degree * bottom_resolution)
        target_position_adc = (uint16_t)(*((uint16_t *)(&mem_ctl_table[MIN_ADC_LIMIT_2BYTES])) 
                              + *((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION]))
                                *(float)(target_position_degree * (*((uint16_t *)(&mem_ctl_table[BOTTOM_RESOLUTION_2BYTES]))/100.0)) );
    }
    
    /* calculate value time */
    run_time = (uint16_t)((float)(*(uint16_t *)(&mem_ctl_table[RUN_TIME_2BYTES]))/(float)abs(target_position_adc-temp_target_position)*15.0+0.5);
//    if(run_time==0){
//        motor_pid.param.p=30;
//    }else{
//        motor_pid.param.p=10;
//    }
    
    /* enable PID control*/
    motor_pid.enable = 1;
  }else if(write_addr==MOTOR_STOP){
    temp_target_position = target_position_adc = get_adc_value(ADC1_CHANNEL_2);
    /* disenable PID control*/
    motor_pid.enable = 0;
  }else if(write_addr==MOTOR_BREAK){
    temp_target_position = target_position_adc = get_adc_value(ADC1_CHANNEL_2);
    /* enable PID control*/
    motor_pid.enable = 1;
  }

  protocal_handle->result_len = return_len;
}
