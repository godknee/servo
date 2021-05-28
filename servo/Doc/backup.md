
```c
//    /* it is the angle of potentiometer,degree */
//    current_position_degree = current_position_adc/POTENTIOMETER_EACH_DEGREE_REPRESENT_NUM;
//    
//    /* convert the angle of motor to the angle of potentiometer, degree */
//    target_position_degree = (uint16_t)( *((uint16_t *)(&mem_ctl_table[TARGET_POSITION_2BYTES]))
//                                          +(*((int16_t *)(&mem_ctl_table[OFFSET_2BYTES]))) );
//    
//    /* convert the angle of motor to the angle of potentiometer, ADC value */
//    target_position_adc = (uint16_t)(target_position_degree * POTENTIOMETER_EACH_DEGREE_REPRESENT_NUM);

//    if(abs(current_position_degree-target_position_degree)<=1){ // degree
//        temp_target_position = target_position_adc;
//        return;
//    }

//    step_time = (run_time/abs(target_position_degree-current_position_degree))*step_degree;

    /* 1 means that each step is 1 degree */
//    step_adc = (uint16_t)(POTENTIOMETER_EACH_DEGREE_REPRESENT_NUM*step_degree);
```

```c
   /* it is the angle of potentiometer,ADC value */
    current_position_adc = get_motor_current_position();
    
    target_position_interpolation();
  
    pwm_width = pid_calculate(&motor_pid, 
                                  current_position_adc, 
                                  temp_target_position);
//    if(pwm_width==0){
//        goto STOP;
//    }
    
    /* run at full speed if it is far from the target position, otherwise use pid */
    if(motor_pid.err<0){
//      if(motor_pid.err<-80){
//        motor_run_backward(800);
//      }else{
//        motor_run_backward(-pwm_width);
//      }
//      motor_run_backward(800);
      motor_run_backward(-pwm_width);
    }else if(motor_pid.err>0){
//      if(motor_pid.err>80){
//        motor_run_forward(800);
//      }else{
//        motor_run_forward(pwm_width);
//      } 
//      motor_run_forward(800);
      motor_run_forward(pwm_width);
    }else{
//STOP:
      motor_stop();
    }

```

```
/* Configure CH4 */
  /* Disable the Channel 4: Reset the CCE Bit */
  TIM1->CCER2 &= (uint8_t)(~(TIM1_CCER2_CC4E | TIM1_CCER2_CC4P));
  /* Set the Output State  &  the Output Polarity */
  TIM1->CCER2 |= (uint8_t)((uint8_t)(TIM1_OUTPUTSTATE_ENABLE & TIM1_CCER2_CC4E ) |  
                           (uint8_t)(TIM1_OCPOLARITY_HIGH  & TIM1_CCER2_CC4P ));
  
  /* Reset the Output Compare Bit  and Set the Output Compare Mode */
  TIM1->CCMR4 = (uint8_t)((uint8_t)(TIM1->CCMR4 & (uint8_t)(~TIM1_CCMR_OCM)) | 
                          TIM1_OCMODE_PWM1);
  
  /* Set the Output Idle state */
  TIM1->OISR &= (uint8_t)(~TIM1_OISR_OIS4);

  
  /* Set the Pulse value */
  TIM1->CCR4H = (uint8_t)(800 >> 8);
  TIM1->CCR4L = (uint8_t)(800);
```

```
//      min_adc = *((uint16_t *)(&mem_ctl_table[MIN_ADC_LIMIT_2BYTES]));
//      dir = *((int8_t *)(&mem_ctl_table[MOTOR_RUN_DIRECTION]));
//      mid_degree = *((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES]));
//      bottom_resolution = *((uint16_t *)(&mem_ctl_table[BOTTOM_RESOLUTION_2BYTES]))/100.0;
//      top_resolution = *((uint16_t *)(&mem_ctl_table[TOP_RESOLUTION_2BYTES]))/100.0;
//      
//      
//      target_position_adc = min_adc + dir*(mid_degree * bottom_resolution + (target_position_degree-mid_degree)*top_resolution+0.5);
      
      
//    adc_error = (float)abs(*((uint16_t *)(&mem_ctl_table[MIDDLE_ADC_VALUE_2BYTES])) - current_adc);
//    degree_error = (float)abs(*((uint16_t *)(&mem_ctl_table[MID_ANGLE_2BYTES])) - *((uint16_t *)(&mem_ctl_table[MIN_ANGLE_LIMIT_2BYTES])));


//static void servo_reg_write(protocal_handle_t *protocal_handle) 
//{
//  uint8_t write_addr    = 0;
//  uint8_t write_len     = 0;
//  uint8_t return_len    = 0;
//  uint8_t return_status = 0;
// 
//  write_addr = protocal_handle->param[0];
//  write_len  = protocal_handle->len-3;
//  
//  return_status = 0;
//  
//  if(write_addr >= MEM_CTL_TABLE_SIZE) {
//    return_status |= (1<<4);
//  }
//  
//  if((write_addr > MEM_CTL_TABLE_ROM_END_ADDR) && (write_addr < MEM_CTL_TABLE_RAM_START_ADDR)) {
//    return_status |= (1<<4);
//  }
//  
//  if(write_len >= (MEM_CTL_TABLE_SIZE-write_addr)) {
//    return_status |= (1<<3);
//  }
//  
//  protocal_handle->result_buf[return_len++] = return_status;
//  
//  protocal_handle->result_len = return_len;
//  
//  if(return_status) {
//    return 0;
//  }
//  
//  memcpy(&mem_ctl_table_cache[write_addr],&(protocal_handle->param[1]),write_len);
//  
//  return 0;
//}
//
//static void servo_action(protocal_handle_t *protocal_handle) 
//{
//  memcpy(mem_ctl_table,mem_ctl_table_cache,MEM_CTL_TABLE_SIZE);
//  memset(mem_ctl_table_cache,0,MEM_CTL_TABLE_SIZE);
//  return 0;
//}

```

