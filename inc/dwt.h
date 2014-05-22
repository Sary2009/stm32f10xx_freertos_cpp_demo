#ifndef DWT_H_
#define DWT_H_

/* ST library includes. */
#include "cmsis_device.h"
#include "usart.h"

extern usart usart1;

class dwt
{

public:
  /* used to measure time */
  uint32_t st, en;

public:
  void
  init (void);

  uint32_t
  get (void);__inline
  uint8_t
  cmp (int32_t tp);

  void
  sleep_us (uint32_t us);

  void
  sleep_ms (uint32_t ms);

  void
  puts (void);
};

#endif /* DWT_H_ */
