#include "drv_tim.h"

/**
  * @brief  Configure TIM1 CH2 & CH4 PWM Out High
  * @param  None
  * @retval None
  */
void timer1_init(void)
{
  /* Set the Autoreload value */
  TIM1->ARRH = (uint8_t)(800-1 >> 8);
  TIM1->ARRL = (uint8_t)(800-1);
  
  /* Set the Prescaler value */
  TIM1->PSCRH = 0;
  TIM1->PSCRL = 0;
  
  /* Configure CH1 */
  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State , 
  the Output N State, the Output Polarity & the Output N Polarity*/
  TIM1->CCER1 &= (uint8_t)(~( TIM1_CCER1_CC1E | TIM1_CCER1_CC1NE 
                             | TIM1_CCER1_CC1P | TIM1_CCER1_CC1NP));
  /* Set the Output State & Set the Output N State & Set the Output Polarity &
  Set the Output N Polarity */
  TIM1->CCER1 |= (uint8_t)((uint8_t)((uint8_t)(TIM1_OUTPUTSTATE_ENABLE & TIM1_CCER1_CC1E)
                                     | (uint8_t)(TIM1_OUTPUTNSTATE_DISABLE & TIM1_CCER1_CC1NE))
                           | (uint8_t)( (uint8_t)(TIM1_OCPOLARITY_HIGH  & TIM1_CCER1_CC1P)
                                       | (uint8_t)(TIM1_OCNPOLARITY_HIGH & TIM1_CCER1_CC1NP)));
  
  /* Reset the Output Compare Bits & Set the Output Compare Mode */
  TIM1->CCMR1 = (uint8_t)((uint8_t)(TIM1->CCMR1 & (uint8_t)(~TIM1_CCMR_OCM)) | 
                          (uint8_t)TIM1_OCMODE_PWM1);
  
  /* Reset the Output Idle state & the Output N Idle state bits */
  TIM1->OISR &= (uint8_t)(~(TIM1_OISR_OIS1 | TIM1_OISR_OIS1N));
  /* Set the Output Idle state & the Output N Idle state configuration */
  TIM1->OISR |= (uint8_t)((uint8_t)( TIM1_OCIDLESTATE_RESET & TIM1_OISR_OIS1 ) | 
                          (uint8_t)( TIM1_OCNIDLESTATE_RESET & TIM1_OISR_OIS1N ));
  
  /* Set the Pulse value */
  TIM1->CCR1H = (uint8_t)(800 >> 8);
  TIM1->CCR1L = (uint8_t)(800);
  
  /* Configure CH2 */
  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State , 
  the Output N State, the Output Polarity & the Output N Polarity*/
  TIM1->CCER1 &= (uint8_t)(~( TIM1_CCER1_CC2E | TIM1_CCER1_CC2NE | 
                             TIM1_CCER1_CC2P | TIM1_CCER1_CC2NP));
  
  /* Set the Output State & Set the Output N State & Set the Output Polarity &
  Set the Output N Polarity */
  TIM1->CCER1 |= (uint8_t)((uint8_t)((uint8_t)(TIM1_OUTPUTSTATE_ENABLE & TIM1_CCER1_CC2E  ) | 
                                     (uint8_t)(TIM1_OUTPUTNSTATE_DISABLE & TIM1_CCER1_CC2NE )) | 
                           (uint8_t)((uint8_t)(TIM1_OCPOLARITY_HIGH  & TIM1_CCER1_CC2P  ) | 
                                     (uint8_t)(TIM1_OCNPOLARITY_HIGH & TIM1_CCER1_CC2NP )));
  
  /* Reset the Output Compare Bits & Set the Output Compare Mode */
  TIM1->CCMR2 = (uint8_t)((uint8_t)(TIM1->CCMR2 & (uint8_t)(~TIM1_CCMR_OCM)) | 
                          (uint8_t)TIM1_OCMODE_PWM1);
  
  /* Reset the Output Idle state & the Output N Idle state bits */
  TIM1->OISR &= (uint8_t)(~(TIM1_OISR_OIS2 | TIM1_OISR_OIS2N));
  /* Set the Output Idle state & the Output N Idle state configuration */
  TIM1->OISR |= (uint8_t)((uint8_t)(TIM1_OISR_OIS2 & TIM1_OCIDLESTATE_RESET) | 
                          (uint8_t)(TIM1_OISR_OIS2N & TIM1_OCNIDLESTATE_RESET));
  
  /* Set the Pulse value */
  TIM1->CCR2H = (uint8_t)(800 >> 8);
  TIM1->CCR2L = (uint8_t)(800);
  
  
  /* TIM1 counter enable */
  TIM1->CR1 |= TIM1_CR1_CEN;

  /* TIM1 Main Output Enable */
  TIM1->BKR |= TIM1_BKR_MOE;
}

/**
  * @brief  Configure TIM2 to generate an update interrupt each 1ms(TIM2->ARR=1000-1) 
  * @param  None
  * @retval None
  */
void timer2_init(void)
{
  /* Set the Prescaler value */
  TIM2->PSCR = (uint8_t)(TIM2_PRESCALER_16);
  /* Set the Autoreload value */
  TIM2->ARRH = (uint8_t)((100-1) >> 8);
  TIM2->ARRL = (uint8_t)(100-1);
  
  /* Clear TIM2 update flag */
  TIM2->SR1 = (uint8_t)(~((uint8_t)(TIM2_FLAG_UPDATE)));
  TIM2->SR2 = (uint8_t)(~((uint8_t)((uint8_t)TIM2_FLAG_UPDATE >> 8)));

  /* Enable the Interrupt sources */
  TIM2->IER |= (uint8_t)TIM2_IT_UPDATE;
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM2 */
//  TIM2->CR1 |= (uint8_t)TIM2_CR1_CEN;
}