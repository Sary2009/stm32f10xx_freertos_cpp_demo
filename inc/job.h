#ifndef JOB_H_
#define JOB_H_

#include <string.h>

/* Scheduler includes. */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

/* ST library includes. */
#include "cmsis_device.h"

/* User library includes. */
#include "led.h"
#include "rtc.h"
#include "adc.h"
#include "dwt.h"
#include "usart.h"

#define MAX_INPUT_LENGTH    serRX_QUEUE_LEN
#define MAX_OUTPUT_LENGTH   serTX_QUEUE_LEN


void
vRegisterCLICommands (void);

void
vCommandConsoleTask (void *pvParameters);

extern usart usart1;
extern QueueHandle_t usart1rxQueue;
extern QueueHandle_t cmdQueue;
extern dwt timer;
extern led led0, led1;
extern rtc stm32rtc;
extern adc adc_tmpr;

#endif /* JOB_H_ */
