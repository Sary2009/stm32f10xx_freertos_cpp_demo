#include "led.h"

// ----------------------------------------------------------------------------

void
led::init ()
{

  // Enable GPIO Peripheral clock
  RCC_APB2PeriphClockCmd (init_struct.RCC_APB2Periph, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.GPIO_Pin = init_struct.GPIO_Pinx;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init (init_struct.GPIOx, &GPIO_InitStructure);

  // Start with led turned off
  turn_off ();
}

// ----------------------------------------------------------------------------

void
led::turn_off ()
{
  if (init_struct.active_level == LED_ACTIVE_LOW)
    GPIO_SetBits (init_struct.GPIOx, init_struct.GPIO_Pinx);
  else
    GPIO_ResetBits (init_struct.GPIOx, init_struct.GPIO_Pinx);
}

// ----------------------------------------------------------------------------

void
led::turn_on ()
{
  if (init_struct.active_level == LED_ACTIVE_LOW)
    GPIO_ResetBits (init_struct.GPIOx, init_struct.GPIO_Pinx);
  else
    GPIO_SetBits (init_struct.GPIOx, init_struct.GPIO_Pinx);
}

// ----------------------------------------------------------------------------

void
led::chg_st ()
{
  GPIO_WriteBit (
      init_struct.GPIOx,
      init_struct.GPIO_Pinx,
      (BitAction) (1
	  - GPIO_ReadOutputDataBit (init_struct.GPIOx,
				    init_struct.GPIO_Pinx)));
}

// ----------------------------------------------------------------------------
