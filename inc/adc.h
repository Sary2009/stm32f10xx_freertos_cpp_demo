
#ifndef ADC_H_
#define ADC_H_

/* ST library includes. */
#include "cmsis_device.h"
#include <stddef.h>
#include <stdlib.h>
class adc
{

public:
  struct
  {
    unsigned long int RCC_APB2Periph_ADCx;
    ADC_TypeDef* ADCx;
    uint8_t ADC_Channel_x;
    ADC_InitTypeDef ADC_InitStructure;
    uint8_t Rank;
    uint8_t ADC_SampleTime;
  } adc_init_struct;

public:
  void
  init (void);
  uint16_t
  get (void);
  void
  puts (char * string, size_t buf_len);
};

#endif /* ADC_H_ */
