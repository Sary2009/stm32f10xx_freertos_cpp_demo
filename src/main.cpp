/*
 * (C) Copyright 2014
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* ST library includes. */
#include "cmsis_device.h"

/* User library includes. */
#include "led.h"
#include "rtc.h"
#include "adc.h"
#include "dwt.h"
#include "usart.h"
#include "job.h"

/* Task priorities. */
#define LED_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define USART1RX_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define CMDCSLTask_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )

/* Task stack size */
#define LED_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE )
#define USART1RX_TASK_STACK_SIZE	( configMINIMAL_STACK_SIZE + 256 )
#define CMDCSLTask_TASK_STACK_SIZE	( configMINIMAL_STACK_SIZE + 256 )

// ----- main() ---------------------------------------------------------------
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void
prvSetupHardware (void);
static void
led_task (void *pvParameters);
static void
usart1rx_task (void *pvParameters);

#define USART1TX_QUEUE_LEN 1u

QueueHandle_t usart1rxQueue;
QueueHandle_t cmdQueue;

/* class declearation */
rtc stm32rtc;

usart usart1;
led led0, led1;
adc adc_tmpr;
dwt timer;

int
main (void)
{
  /* Initalize hardware */
  prvSetupHardware ();

  /* Create a queue capable of containing USART1RX_QUEUE_LEN char values*/
  usart1rxQueue = xQueueCreate(USART1TX_QUEUE_LEN, sizeof(char));

  /* usart1rx_task send to vCommandConsoleTask */
  cmdQueue = xQueueCreate(USART1TX_QUEUE_LEN, sizeof(char));

  xTaskCreate(led_task, "led_task", LED_TASK_STACK_SIZE, NULL,
	      LED_TASK_PRIORITY, NULL);
  xTaskCreate(usart1rx_task, "usart1rx_task", USART1RX_TASK_STACK_SIZE, NULL,
	      USART1RX_TASK_PRIORITY, NULL);
  xTaskCreate(vCommandConsoleTask, "cmdcsl_task", CMDCSLTask_TASK_STACK_SIZE,
	      NULL, CMDCSLTask_TASK_PRIORITY, NULL);

  vRegisterCLICommands ();

  /* it may receive bytes during system start,
   * so we need resets a xUsartRxedChars queue
   * to empty state
   */
  xQueueReset(usart1.xUsartRxedChars);

  /* Start the scheduler. */
  vTaskStartScheduler ();

  /* Will only get here if there was insufficient memory to create the idle
   task.  The idle task is created within vTaskStartScheduler(). */
  for (;;)
    ;
  return 0;
}

/*-----------------------------------------------------------*/

/* led flash task */
void
led_task (void *pvParameters)
{
  portTickType xLastWakeTime;
  /* The xLastWakeTime variable needs to be initialized with the current tick
   count. Note that this is the only time the variable is explicitly written to.
   After this xLastWakeTime is managed automatically by the vTaskDelayUntil()
   API function. */
  xLastWakeTime = xTaskGetTickCount ();
  /* As per most tasks, this task is implemented in an infinite loop. */
  for (;;)
    {
      led0.chg_st ();
      /* The task should execute every 500 milliseconds exactly. */
      vTaskDelayUntil (&xLastWakeTime, (500 / portTICK_RATE_MS));
    }
}

/*-----------------------------------------------------------*/

void
usart1rx_task (void *pvParameters)
{

  /* this task is used to deal with usart incoming message */
  char cChar = 0xA;

  extern char CmdLStr[MAX_INPUT_LENGTH];

  char *p_message = NULL;

  /* As per most tasks, this task is implemented in an infinite loop. */
  for (;;)
    {

      xQueueReceive(usart1rxQueue, &cChar, portMAX_DELAY);

      /* A newline character was received, so the input command stirng is
       complete and can be processed.  Transmit a line separator, just to
       make the output easier to read. */

      /* message type */
      switch (cChar)
	{
	case TOOLONG:
#ifdef COMMAND_ECHO
	  usart1.puts ("Command too long!\r\n");
#endif
	  break;
	case CTRL_C: /* user pressed Ctrl + C */
#ifdef COMMAND_ECHO
	  usart1.puts ("\r\n> ");
#endif
	  break;
	case CTRL_H: /* when Control + H | Backspace pressed */

	  if (uxQueueMessagesWaiting (usart1.xUsartRxedChars) == 0)
	    break;
	  else
	    {
	      memset (CmdLStr, '\0', MAX_INPUT_LENGTH);
	      usart1.puts ("\r> ");
	      /* receive the command string */
	      p_message = CmdLStr;
	      while (xQueueReceive(usart1.xUsartRxedChars, &cChar, 0) == pdTRUE)
		{
		  *p_message++ = cChar;
		  usart1.puts (" ");
		}

	      /* remove last char */
	      *(p_message - 1) = '\0';

	      /* send remain chars to usart1.xUsartRxedChars */
	      p_message = CmdLStr;
	      while (*p_message)
		{
		  xQueueSend(usart1.xUsartRxedChars, p_message++, 0);
		}

	      /* print content to screen */
	      usart1.puts ("\r> ");
	      /* wait precious one to finish */
	      timer.sleep_ms (USART_DELAY_MS);

	      /* send remain chars to screen */
	      usart1.puts (CmdLStr);

	      memset (CmdLStr, '\0', MAX_INPUT_LENGTH);

	      break;
	    }
	case CTRL_L: /* clear screen */
	  usart1.puts ("\r\n\r\n\r\n\r\n\r\n"
		       "\r\n\r\n\r\n\r\n\r\n"
		       "\r\n\r\n\r\n\r\n\r\n"
		       "\r\n\r\n\r\n\r\n\r\n"
		       "\r\n\r\n\r\n\r\n");
	  /* wait precious one to finish */
	  timer.sleep_ms (USART_DELAY_MS);
#ifdef COMMAND_ECHO
	  usart1.puts ("> ");
#endif
	  break;

	case NORMAL:
	  if (uxQueueMessagesWaiting (usart1.xUsartRxedChars) == 0) /* when user just press Enter */
	    {
#ifdef COMMAND_ECHO
	      usart1.puts ("\r\n> ");
#endif
	      break;
	    }
	  else if ( xSemaphoreTake(usart1.xSemaphoreMutex,
	      (TickType_t ) 10) == pdTRUE) /* if precious job has been finished, we will obtain the mutex */
	    {
	      xQueueSend(cmdQueue, &cChar, (TickType_t ) 10);
#ifdef COMMAND_ECHO
	      usart1.puts ("\r\n");
#endif
	      // We have finished jobs.  Release the
	      // semaphore.
	      xSemaphoreGive(usart1.xSemaphoreMutex);
	    }
	  else /* or this job will be discarded */
	    {
	      /* resets a xUsartRxedChars queue to empty state */
	      xQueueReset(usart1.xUsartRxedChars);
#ifdef COMMAND_ECHO
	      usart1.puts ("\r\nSystem is busy, wait for last to finish...\r\n> ");
#endif
	    }
	  break;
	}
    }
}

/*-----------------------------------------------------------*/

void
prvSetupHardware (void)
{
  /* Initialize DWT */
  timer.init ();

  /* Initialize usart1 */
  usart1.init_struct =
    { USART1, BAUDRATE};
  usart1.init ();

  /* led0 PA8, active low */
  led0.init_struct=
    { RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_8, LED_ACTIVE_LOW};
  led0.init ();
  /* led1 PD2, active low */
  led1.init_struct=
    { RCC_APB2Periph_GPIOD, GPIOD, GPIO_Pin_2, LED_ACTIVE_LOW};
  led1.init ();

  /* Initialize rtc */
  stm32rtc.init ();

  /* Initialize temperature sensor */
  adc_tmpr.adc_init_struct =
    { RCC_APB2Periph_ADC1, ADC1, ADC_Channel_16,
	{ ADC_Mode_Independent, DISABLE, DISABLE, ADC_ExternalTrigConv_None, ADC_DataAlign_Right, 1},
      1, ADC_SampleTime_41Cycles5};
  adc_tmpr.init ();
}

/*-----------------------------------------------------------*/

extern "C" void
vApplicationIdleHook (void)
{
  /* enter sleep mode to save power when idle, ZZZ... */
  __WFI ();
}

/*-----------------------------------------------------------*/

extern "C" void
vApplicationStackOverflowHook (xTaskHandle *pxTask, signed char *pcTaskName)
{
  led0.turn_on ();
  led1.turn_on ();
  usart1.printf ("%s: statck overflow!\r\n", pcTaskName);
  for (;;)
    ;
}

/*-----------------------------------------------------------*/
#pragma GCC diagnostic pop
// ----------------------------------------------------------------------------
