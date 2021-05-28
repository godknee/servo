#ifndef __ADC_H
#define __ADC_H

#include "stm8s.h"

void adc_init();
uint16_t get_adc_value(ADC1_Channel_TypeDef ADC1_Channel);

#endif