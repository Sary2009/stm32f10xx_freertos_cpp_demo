#include "dwt.h"

void
dwt::init (void)
{
  if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
    {
      CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
      DWT->CYCCNT = 0;
      DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

uint32_t
dwt::get (void)
{
  return DWT->CYCCNT;
}

__inline uint8_t
dwt::cmp (int32_t tp)
{
  return (((int32_t) get () - tp) < 0);
}

// microseconds, up to 59s
void
dwt::sleep_us (uint32_t us)
{
  int32_t tp = get () + us * (SystemCoreClock / 1000000);
  while (cmp (tp))
    ;
}

// milliseconds up to 59s
void
dwt::sleep_ms (uint32_t ms)
{
  int32_t tp = get () + ms * (SystemCoreClock / 1000);
  while (cmp (tp))
    ;
}

void
dwt::puts (void)
{
  usart1.printf ("%d us\r\n", ((en - st) / 72));
}
