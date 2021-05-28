#include "drv_uart.h"

uart_handle_t uart1;

uint8_t uart1_tx_buffer[50];

void uart1_init(uint32_t baud_rate)
{
//    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);
//  /* Reset corresponding bit to GPIO_Pin in CR2 register */
//  GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_5));
//  /* Set Output mode */
//  GPIOD->DDR |= (uint8_t)GPIO_PIN_5;
//  /* Float or Open-Drain */
//  GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_5));
//  /* No external interrupt or No slope control */
//  GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_5));

  /* Clear the word length bit */
  UART1->CR1 &= (uint8_t)(~UART1_CR1_M);  
  
  /* Set the word length bit according to UART1_WordLength value */
  UART1->CR1 |= (uint8_t)UART1_WORDLENGTH_8D;
  
  /* Clear the STOP bits */
  UART1->CR3 &= (uint8_t)(~UART1_CR3_STOP);  
  /* Set the STOP bits number according to UART1_StopBits value  */
  UART1->CR3 |= (uint8_t)UART1_STOPBITS_1;  
  
  /* Clear the Parity Control bit */
  UART1->CR1 &= (uint8_t)(~(UART1_CR1_PCEN | UART1_CR1_PS  ));  
  /* Set the Parity Control bit to UART1_Parity value */
  UART1->CR1 |= (uint8_t)UART1_PARITY_NO;  
  
  /* Clear the LSB mantissa of UART1DIV  */
  UART1->BRR1 &= (uint8_t)(~UART1_BRR1_DIVM);  
  /* Clear the MSB mantissa of UART1DIV  */
  UART1->BRR2 &= (uint8_t)(~UART1_BRR2_DIVM);  
  /* Clear the Fraction bits of UART1DIV */
  UART1->BRR2 &= (uint8_t)(~UART1_BRR2_DIVF);  
  
  /* Set the UART1 BaudRates in BRR1 and BRR2 registers according to UART1_BaudRate value */
  /* Set the fraction of UART1DIV  */
  UART1->BRR2 |= (uint8_t)((uint8_t)(((((HSI_VALUE * 100) / (baud_rate << 4)) - ((HSI_VALUE / (baud_rate << 4)) * 100)) << 4) / 100) & (uint8_t)0x0F); 
  /* Set the MSB mantissa of UART1DIV  */
  UART1->BRR2 |= (uint8_t)(((HSI_VALUE / (baud_rate << 4)) >> 4) & (uint8_t)0xF0); 
  /* Set the LSB mantissa of UART1DIV  */
  UART1->BRR1 |= (uint8_t)(HSI_VALUE / (baud_rate << 4));           
  
  /* Disable the Transmitter and Receiver before setting the LBCL, CPOL and CPHA bits */
  UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN); 
  /* Clear the Clock Polarity, lock Phase, Last Bit Clock pulse */
  UART1->CR3 &= (uint8_t)~(UART1_CR3_CPOL | UART1_CR3_CPHA | UART1_CR3_LBCL); 
  /* Set the Clock Polarity, lock Phase, Last Bit Clock pulse */
  UART1->CR3 |= (uint8_t)((uint8_t)UART1_SYNCMODE_CLOCK_DISABLE & (uint8_t)(UART1_CR3_CPOL | 
                                                        UART1_CR3_CPHA | UART1_CR3_LBCL));  

  /* Set the Transmitter Enable bit */
  UART1->CR2 |= (uint8_t)UART1_CR2_TEN;  

  /* Set the Receiver Enable bit */
  UART1->CR2 |= (uint8_t)UART1_CR2_REN;  

  /* Clear the Clock Enable bit */
  UART1->CR3 &= (uint8_t)(~UART1_CR3_CKEN); 
  
//  UART1_ITConfig(UART1_IT_RXNE, ENABLE);
  UART1->CR2 |= (uint8_t)(1 << 5);
  
  /* Enable UART1 Half Duplex Mode*/
  UART1->CR5 |= UART1_CR5_HDSEL;
//  UART1_HalfDuplexCmd(ENABLE);
  
  enableInterrupts();
  
  /* Init the tx buffer */
  uart1.tx_buffer_ptr = uart1_tx_buffer;
}


char uart_transmit_it(uint8_t *buf,uint8_t size)
{
//  if((UART1->SR & 0x40)==0){
//    return -1;
//  }else{
//     /* clear the TC flag */
////    UART1->SR &= (uint8_t)~(UART1_SR_TC);
//    
//    /* Disable the Receiver */
//    UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN); 
//    /* Enable Transmitter */
//    UART1->CR2 |= (uint8_t)UART1_CR2_TEN;
//    
//    for(uint8_t i=0;i<size;i++){
//        UART1_SendData8(buf[i]);
//    }
//    
//    /* Disable Transmitter */
//    UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN);  
//    /* Enable Receiver */
//    UART1->CR2 |= (uint8_t)UART1_CR2_REN; 
//    
//    return 0;
//  }
  
  if(uart1.tx_state == UART_STATE_READY){
    /* Disable the Receiver */
    UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN); 
    /* Enable Transmitter */
    UART1->CR2 |= (uint8_t)UART1_CR2_TEN;
    
    uart1.tx_state = UART_STATE_BUSY;
//    uart1.tx_buffer_ptr = buf;
    for(uint16_t i=0;i<size;i++){
        uart1.tx_buffer_ptr[i]=buf[i];
    }

    uart1.size = size;

    /* enable TXE interrupt */
    UART1->CR2 |= (uint8_t)(1 << 7);
    
    return 0;
  }else{
    return -1;
  }
}