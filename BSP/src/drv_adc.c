#include "drv_adc.h"

#define ADC_SCAN

uint16_t raw_current_adc=0;

/* single scan mode */
void adc_init()
{
  /* Input floating, no external interrupt */
  GPIOD->DDR &= (uint8_t)(~(GPIO_PIN_2));
  GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_2));
  GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
  
  GPIOC->DDR &= (uint8_t)(~(GPIO_PIN_4));
  GPIOC->CR1 &= (uint8_t)(~(GPIO_PIN_4));
  GPIOC->CR2 &= (uint8_t)(~(GPIO_PIN_4));
  
  /* Clear the align bit */
  ADC1->CR2 &= (uint8_t)(~ADC1_CR2_ALIGN);
  /* Configure the data alignment */
  ADC1->CR2 |= (uint8_t)(ADC1_ALIGN_RIGHT);

#ifndef ADC_SCAN
  /* Set the single conversion mode */
  ADC1->CR1 &= (uint8_t)(~ADC1_CR1_CONT);
 
#else
  
  /* Set the continuous conversion mode */
//  ADC1->CR1 |= ADC1_CR1_CONT; // we will use single scan model
  
  /* Clear the ADC1 channels */
  ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
  /* Select the ADC1 channel */
  ADC1->CSR |= (uint8_t)(ADC1_CHANNEL_3); // from AIN0 ~ AIN3
  
#endif
  
  /* Clear the SPSEL bits */
  ADC1->CR1 &= (uint8_t)(~ADC1_CR1_SPSEL);
  /* Select the prescaler division factor according to ADC1_PrescalerSelection values */
  ADC1->CR1 |= (uint8_t)(ADC1_PRESSEL_FCPU_D2);
  
  // DISABLE
  ADC1->TDRL |= (uint8_t)((uint8_t)0x01 << (uint8_t)ADC1_SCHMITTTRIG_CHANNEL2);
  ADC1->TDRL |= (uint8_t)((uint8_t)0x01 << (uint8_t)ADC1_SCHMITTTRIG_CHANNEL3);
  
   /* Enable the ADC1 peripheral */
  ADC1->CR1 |= ADC1_CR1_ADON;
  
#ifdef ADC_SCAN
  /* Enables the ADC1 scan mode */
  ADC1->CR2 |= ADC1_CR2_SCAN;
  
  /* Enables the ADC1 data store into the Data Buffer registers rather than in the Data Register */
  ADC1->CR3 |= ADC1_CR3_DBUF;

  /* Start ADC1 conversion */
//  ADC1->CR1 |= ADC1_CR1_ADON; // we use single scan model, so we don't start ADC conversion here
#endif
}

#ifndef ADC_SCAN
uint16_t get_adc_value(ADC1_Channel_TypeDef ADC1_Channel)
{
    uint16_t temph = 0;
    uint8_t templ = 0;
    
    /* Clear the ADC1 channels */
    ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
    /* Select the ADC1 channel */
    ADC1->CSR |= (uint8_t)(ADC1_Channel);
    
    /* Start ADC1 conversion */
    ADC1->CR1 |= ADC1_CR1_ADON;
    
    /* Wait for conversion finish */
    while((ADC1->CSR & ADC1_FLAG_EOC)==0);
    /* clear the flag */
    ADC1->CSR &= (uint8_t)~(ADC1_FLAG_EOC);
    
    /* Read LSB first */
    templ = ADC1->DRL;
    /* Then read MSB */
    temph = ADC1->DRH;
    temph = (uint16_t)(templ | (uint16_t)(temph << (uint8_t)8));
    return temph;
}
#else
uint16_t get_adc_value(ADC1_Channel_TypeDef ADC1_Channel)
{
    /* Start ADC1 conversion */
    ADC1->CR1 |= ADC1_CR1_ADON;
    
    /* Wait for conversion finish */
    while((ADC1->CSR & ADC1_FLAG_EOC)==0);
    /* clear the flag */
    ADC1->CSR &= (uint8_t)~(ADC1_FLAG_EOC);

    /* read current */
    raw_current_adc = (uint16_t)(*((uint8_t*)0x53E7) | (uint16_t)(*((uint8_t*)0x53E6) << (uint8_t)8));
    
    /* read position */
    return (uint16_t)(*((uint8_t*)0x53E5) | (uint16_t)(*((uint8_t*)0x53E4) << (uint8_t)8));
}
#endif
