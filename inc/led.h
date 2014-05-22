#ifndef LED_H_
#define LED_H_

/* ST Library includes. */
#include "cmsis_device.h"

// ----------------------------------------------------------------------------

typedef enum
{
  LED_ACTIVE_LOW = 0, LED_ACTIVE_HIGH = !LED_ACTIVE_LOW
} LED_ACTIVE;

class led
{

public:

  struct
  {
    unsigned long int RCC_APB2Periph;
    GPIO_TypeDef* GPIOx;
    uint32_t GPIO_Pinx;
    LED_ACTIVE active_level;
  } init_struct;

public:
  void
  init ();

  void
  turn_off ();

  void
  turn_on ();

  void
  chg_st ();
};

// ----------------------------------------------------------------------------

#endif /* LED_H_ */
