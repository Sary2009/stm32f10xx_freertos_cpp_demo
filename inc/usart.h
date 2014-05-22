#ifndef STM_32_SERIAL_COMMS_H
#define STM_32_SERIAL_COMMS_H

#ifdef __cplusplus
extern "C"
{
#endif

  /* standard includes */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

  /* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

  /* ST library includes. */
#include "cmsis_device.h"

#include "led.h"

  typedef enum
  {
    NORMAL = 0, TOOLONG, CTRL_C, BUSY, CTRL_L, CTRL_H
  } usart_cmd_rx_st;

  /* Queues are used to hold characters that are waiting to be transmitted.  This
   constant sets the maximum number of characters that can be contained in such a
   queue at any one time. */
#define serTX_QUEUE_LEN					( 100 )

  /* Queues are used to hold characters that have been received but not yet
   processed.  This constant sets the maximum number of characters that can be
   contained in such a queue. */
#define serRX_QUEUE_LEN					( 50 )

#define USART_TX_BUF_SIZE			serTX_QUEUE_LEN
#define USART_RX_BUF_SIZE			serRX_QUEUE_LEN

  /* whether to enable command echo */
#define COMMAND_ECHO

  /* stm32 transmit and receive baud rates up to 4.5 MBits/s,
   * but my PL2303 just only to 460800,
   * ^_^ */
#define BAUDRATE 460800u
#define USART_DELAY_MS				(serTX_QUEUE_LEN * 8 * 1000 / BAUDRATE + 1)
  /* The maximum amount of time that calls to lSerialPutString() should wait for
   there to be space to post each character to the queue of characters waiting
   transmission.  NOTE!  This is the time to wait per character - not the time to
   wait for the entire string. */
#define serPUT_STRING_CHAR_DELAY		( USART_DELAY_MS / portTICK_RATE_MS )

  /* usart class declearation */

  class usart
  {

  public:

    char usart_recv[serRX_QUEUE_LEN];

    /* Usart Queues used to hold characters waiting to be transmitted. */
    QueueHandle_t xCharsForUsartTx;

    /* Usart Queues holding received characters. */
    QueueHandle_t xUsartRxedChars;

    /* Usart Queue to keep sources */
    SemaphoreHandle_t xSemaphoreMutex;

    struct
    {
      USART_TypeDef* USARTx;

      /* USART baud rate */
      uint32_t ulWantedBaud;

    } init_struct;

  private:

    signed long
    get_c (signed char *pcRxedChar, TickType_t xBlockTime);

  public:

    void
    init (void);

    signed long
    put_c (signed char cOutChar, TickType_t xBlockTime);

    int
    printf (const char *format, ...);

    long
    puts (const char * pcString);

  };

#ifdef __cplusplus
}
#endif

#endif
