#ifndef __UART_H
#define __UART_H
#include "stm8s.h"

typedef enum{
  UART_STATE_READY,
  UART_STATE_BUSY,
} uart_state_e;

typedef struct{
    volatile uart_state_e  tx_state;
    unsigned char *tx_buffer_ptr;
    volatile unsigned char size;
}uart_handle_t;

void uart1_init(uint32_t baud_rate);
char uart_transmit_it(unsigned char *buf,unsigned char size);

#endif