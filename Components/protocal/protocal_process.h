#ifndef __PROTOCAL_PROCESS_H
#define __PROTOCAL_PROCESS_H

typedef enum{
  FIND_HEAD_FIRST_FF,
  FIND_HEAD_SECOND_FF,
  GET_ID,
  GET_LENGTH,
  GET_CMD,
  GET_PARAM,
  GET_CHECKSUM,
}step_e;

typedef struct{
    unsigned char ch;
    unsigned char id;
    unsigned char len;
    unsigned char cmd;
    unsigned char param[30];
    unsigned char clc_sum;
    unsigned char index;
    step_e step;
    unsigned char result_buf[15];
    unsigned char result_len;
    unsigned char send_frame_buf[15];
    unsigned char send_frame_len;
}protocal_handle_t;

void protocal_process_init(void);
void protocal_decode_process(void);
void protocal_encode_process(uint8_t id, uint8_t *data, uint8_t len, uint8_t *result, uint8_t *result_len); 
void protocal_send(uint8_t *buf,uint8_t size);

#endif