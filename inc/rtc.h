#ifndef RTC_H_
#define RTC_H_

#include <ctime>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define ENABLE_RTC_IRQ

class rtc
{

public:
  xSemaphoreHandle rtcSemaphore = NULL;

public:

  void
  init (void);

  void
  update (struct tm *t);

  void
  puts (char * string, size_t buf_len);

};

#endif /* RTC_H_ */
