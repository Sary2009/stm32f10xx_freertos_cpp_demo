#include "usart.h"

void
usart::init (void)
{

  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* The common (not port dependent) part of the initialization. */
  USART_InitStructure.USART_BaudRate = init_struct.ulWantedBaud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl =
  USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
  configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  /* Create the queue of chars that are waiting to be sent. */
  xCharsForUsartTx = xQueueCreate(serTX_QUEUE_LEN, sizeof(char));

  /* Create the queue used to hold characters received. */
  xUsartRxedChars = xQueueCreate(sizeof(usart_recv), sizeof(char));

  xSemaphoreMutex = xSemaphoreCreateMutex();

  if (init_struct.USARTx == USART1)
    {

      /* Enable clock - the ST libraries start numbering from UART1. */
      RCC_APB2PeriphClockCmd (
      RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,
			      ENABLE);

      /* Configure USART1 Rx (PA10) as input floating */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init ( GPIOA, &GPIO_InitStructure);

      /* Configure USART1 Tx (PA9) as alternate function push-pull */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init ( GPIOA, &GPIO_InitStructure);

      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
      NVIC_Init (&NVIC_InitStructure);
    }
  else if (init_struct.USARTx == USART2)
    {
      /* Enable clock - the ST libraries start numbering from UART2. */
      RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
      RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE);

      /* Configure USART2 Rx (PA3) as input floating */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init ( GPIOA, &GPIO_InitStructure);

      /* Configure USART2 Tx (PA2) as alternate function push-pull */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init ( GPIOA, &GPIO_InitStructure);

      NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
      NVIC_Init (&NVIC_InitStructure);
    }
  else if (init_struct.USARTx == USART3)
    {
      /* Enable clock - the ST libraries start numbering from UART3. */
      RCC_APB1PeriphClockCmd ( RCC_APB1Periph_USART3, ENABLE);
      RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);

      /* Configure USART3 Rx (PB11) as input floating */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init ( GPIOB, &GPIO_InitStructure);

      /* Configure USART3 Tx (PB10) as alternate function push-pull */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init ( GPIOB, &GPIO_InitStructure);

      NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
      NVIC_Init (&NVIC_InitStructure);
    }
  USART_Init (init_struct.USARTx, &USART_InitStructure);
  USART_ITConfig (init_struct.USARTx, USART_IT_RXNE, ENABLE);
  USART_Cmd (init_struct.USARTx, ENABLE);
}

signed long
usart::get_c (signed char *pcRxedChar, TickType_t xBlockTime)
{
  long lReturn = pdFAIL;

  if ( xQueueReceive(xUsartRxedChars, pcRxedChar,
      xBlockTime) == pdPASS)
    {
      lReturn = pdPASS;
    }
  return lReturn;
}

/*-----------------------------------------------------------*/

long
usart::puts (const char * pcString)
{

  long lReturn = pdPASS;

  while (*pcString)
    {
      if ( xQueueSend(xCharsForUsartTx, pcString++,
	  serPUT_STRING_CHAR_DELAY) != pdPASS)
	{
	  /* Cannot fit any more in the queue.  Try turning the Tx on to clear some space. */

	  USART_ITConfig (USART1, USART_IT_TXE, ENABLE);

	  vTaskDelay ( serPUT_STRING_CHAR_DELAY);

	  /* Back to precious char */
	  pcString--;
	}
    }
  USART_ITConfig (USART1, USART_IT_TXE, ENABLE);

  return lReturn;
}
/*-----------------------------------------------------------*/

signed long
usart::put_c (signed char cOutChar, TickType_t xBlockTime)
{

  long lReturn = pdFAIL;

  if ( xQueueSend( xCharsForUsartTx, &cOutChar, xBlockTime ) == pdPASS)
    {
      lReturn = pdPASS;

      USART_ITConfig (USART1, USART_IT_TXE, ENABLE);
    }
  else
    {
      lReturn = pdFAIL;
    }
  return lReturn;
}

int
usart::printf (const char* format, ...)
{

  int ret = 1;
  char printf_buf[USART_TX_BUF_SIZE];

  va_list ap;
  va_start(ap, format);

  ret = vsnprintf (printf_buf, USART_TX_BUF_SIZE, format, ap);
  if (ret > 0)
    {
      ret = puts (printf_buf);
    }
  va_end(ap);

  return ret;
}

/*-----------------------------------------------------------*/

extern usart usart1;
extern QueueHandle_t usart1rxQueue;
extern led led1;

extern "C" void
USART1_IRQHandler (void)
{
  static long xHigherPriorityTaskWoken = pdFALSE;

  char cChar;

  if (USART_GetITStatus ( USART1, USART_IT_TXE) == SET)
    {
      /* The interrupt was caused by the THR becoming empty.  Are there any
       more characters to transmit? */
      if (xQueueReceiveFromISR (usart1.xCharsForUsartTx, &cChar,
				&xHigherPriorityTaskWoken))
	{
	  /* A character was retrieved from the buffer so can be sent to the
	   THR now. */
	  USART_SendData ( USART1, cChar);
	}
      else
	{
	  USART_ITConfig ( USART1, USART_IT_TXE, DISABLE);
	}
    }

  if (USART_GetITStatus ( USART1, USART_IT_RXNE) == SET)
    {
      /* read udart rx buffer */
      cChar = USART_ReceiveData ( USART1);

      if (xQueueIsQueueFullFromISR (usart1.xUsartRxedChars) != pdFALSE)
	{ /* Queue full*/
	  /* resets a xUsartRxedChars queue to empty state */
	  xQueueReset(usart1.xUsartRxedChars);
	  cChar = TOOLONG;

	  xQueueSendFromISR(usart1rxQueue, &cChar, &xHigherPriorityTaskWoken);
	}
      else if (cChar > 31 && cChar < 127)
	{ /* normal characters receive */
	  /* send to xUsartRxedChars */
	  xQueueSendFromISR(usart1.xUsartRxedChars, &cChar,
			    &xHigherPriorityTaskWoken);
#ifdef COMMAND_ECHO
	  usart1.put_c (cChar, serPUT_STRING_CHAR_DELAY);
#endif
	}
      else if (cChar == '\r')
	{
	  /* Queue not full but Enter pressed */
	  cChar = NORMAL;
	  xQueueSendFromISR(usart1rxQueue, &cChar, &xHigherPriorityTaskWoken);
	}
      else if (cChar == 3)
	{
	  /* when Control + C pressed */
	  /* resets a xUsartRxedChars queue to empty state */
	  xQueueReset(usart1.xUsartRxedChars);
	  cChar = CTRL_C;

	  xQueueSendFromISR(usart1rxQueue, &cChar, &xHigherPriorityTaskWoken);
	}
      else if (cChar == 12) /* when Control + C pressed */
	{
	  /* resets a xUsartRxedChars queue to empty state */
	  xQueueReset(usart1.xUsartRxedChars);
	  cChar = CTRL_L;
	  xQueueSendFromISR(usart1rxQueue, &cChar, &xHigherPriorityTaskWoken);
	}
      else if (cChar == 127 || cChar == 8) /* when Control + h | Backspace pressed */
	{
	  cChar = CTRL_H;
	  xQueueSendFromISR(usart1rxQueue, &cChar, &xHigherPriorityTaskWoken);
	}
    }

  /* Now we can switch context if necessary. */
  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

}
/*-----------------------------------------------------------*/

extern "C" void
USART2_IRQHandler (void)
{
  static long xHigherPriorityTaskWoken = pdFALSE;
//  char cChar;

  if (USART_GetITStatus ( USART2, USART_IT_TXE) == SET)
    {

    }

  if (USART_GetITStatus ( USART2, USART_IT_RXNE) == SET)
    {
      USART_ReceiveData ( USART2);
    }

  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/*-----------------------------------------------------------*/

extern "C" void
USART3_IRQHandler (void)
{

  static long xHigherPriorityTaskWoken = pdFALSE;
//  char cChar;

  if (USART_GetITStatus ( USART3, USART_IT_TXE) == SET)
    {

    }

  if (USART_GetITStatus ( USART3, USART_IT_RXNE) == SET)
    {
      USART_ReceiveData ( USART3);
    }

  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

