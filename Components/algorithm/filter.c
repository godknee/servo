#include "filter.h"
#include "drv_adc.h"

#define MY_FILTER_N 30
unsigned short my_filter_buf[MY_FILTER_N];
unsigned char my_index = 0;

unsigned short filter_average(void)
{
    unsigned short sum=0;
    
    my_filter_buf[my_index++] = get_adc_value(ADC1_CHANNEL_2);
    
    if(my_index==MY_FILTER_N){
        my_index=0;
    }
    
    for(unsigned char count=0;count<MY_FILTER_N;count++){
        sum += my_filter_buf[count];
    }
    
    return (unsigned short)(sum/MY_FILTER_N);
}

//#define FILTER_N 8
//#define FILTER_S_N 4
//unsigned short filter_buf[FILTER_N + 1];
//unsigned short filter_s_buf[FILTER_S_N + 1];
//
//unsigned short filter(void)
//{
//  unsigned char  i;
//  unsigned short  filter_sum = 0;
//  
//  filter_buf[FILTER_N] = get_adc_value(ADC1_CHANNEL_2);
//  for(i = 0; i < FILTER_N; i++){
//    filter_buf[i] = filter_buf[i + 1];
//    filter_sum += filter_buf[i];
//  }
//  
//  unsigned short res = (unsigned short)(filter_sum / FILTER_N);
//  
//  // clear the filter_sum
//  filter_sum = 0;
//  
//  // second filter
//  filter_s_buf[FILTER_S_N] = res;
//  for(i = 0; i < FILTER_S_N; i++){
//    filter_s_buf[i] = filter_s_buf[i + 1];
//    filter_sum += filter_s_buf[i];
//  }
//  
//  return (unsigned short)(filter_sum / FILTER_S_N);
//}


#define FILTER_N 8
unsigned short filter_buf[FILTER_N + 1];

unsigned short filter(void)
{
  unsigned char  i;
  unsigned short  filter_sum = 0;
  
  filter_buf[FILTER_N] = get_adc_value(ADC1_CHANNEL_2);
  for(i = 0; i < FILTER_N; i++){
    filter_buf[i] = filter_buf[i + 1];
    filter_sum += filter_buf[i];
  }
  
  return (unsigned short)(filter_sum / FILTER_N);
}

#define FILTER_AGAIN_N 8
unsigned short filter_again_buf[FILTER_AGAIN_N + 1];

unsigned short filter_again(unsigned short value)
{
  unsigned char  i;
  unsigned short  filter_sum = 0;
  
  filter_again_buf[FILTER_AGAIN_N] = value;
  for(i = 0; i < FILTER_AGAIN_N; i++){
    filter_again_buf[i] = filter_again_buf[i + 1];
    filter_sum += filter_again_buf[i];
  }
  
  return (unsigned short)(filter_sum / FILTER_AGAIN_N);
}

// middle value filter

#define M 11

unsigned short filter2(void)
{
    unsigned short value_buf[M];
    unsigned short count, i, j, temp;
    for( count = 0; count < M; count++ ){
        value_buf[count] = get_adc_value(ADC1_CHANNEL_2);
    }

    for( j = 0; j < M - 1; j++ ){
        for( i = 0; i < M - j - 1; i++ ){
            if( value_buf[i] > value_buf[i + 1] ){
                temp = value_buf[i];
                value_buf[i] = value_buf[i + 1];
                value_buf[i + 1] = temp;
            }
        }
    }
    return value_buf[(M-1)/2];
}

//unsigned short filter_clipping(unsigned short value, unsigned short new_value, 
//                               unsigned short amplitude_limit) 
//{ 
//    if((new_value-value)>amplitude_limit || (value-new_value)>amplitude_limit)
//    {
//        return value;
//    }
//    
//    return new_value;
//} 
