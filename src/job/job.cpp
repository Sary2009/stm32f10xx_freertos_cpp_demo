#include "job.h"

/* use to get parameters */
char CmdLStr[MAX_INPUT_LENGTH];

static portBASE_TYPE
prvDateCommand (char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString)
{
  (void) pcCommandString;
  xWriteBufferLen = MAX_OUTPUT_LENGTH;

  stm32rtc.puts (pcWriteBuffer, xWriteBufferLen);

  /* There is no more data to return after this single string, so return
   pdFALSE. */
  return pdFALSE;
}

static const CLI_Command_Definition_t xDate =
  { "date", /* The command string to type. */
  "date:\r\n show date\r\n\r\n", prvDateCommand, /* The function to run. */
  0 /* No parameters are expected. */
  };

static portBASE_TYPE
prvGetTmprCommand (char *pcWriteBuffer, size_t xWriteBufferLen,
		   const char *pcCommandString)
{
  (void) pcCommandString;
  xWriteBufferLen = MAX_OUTPUT_LENGTH;

  adc_tmpr.puts (pcWriteBuffer, xWriteBufferLen);

  /* There is no more data to return after this single string, so return
   pdFALSE. */
  return pdFALSE;
}

static const CLI_Command_Definition_t xGetTmpr =
  { "gettmpr", /* The command string to type. */
  "gettmpr:\r\n show stm32's temperature \r\n\r\n", prvGetTmprCommand, /* The function to run. */
  0 /* No parameters are expected. */
  };

static portBASE_TYPE
prvClearCommand (char *pcWriteBuffer, size_t xWriteBufferLen,
		 const char *pcCommandString)
{
  (void) pcCommandString;
  (void) xWriteBufferLen;

  strcpy (pcWriteBuffer, "\r\n\r\n\r\n\r\n\r\n"
	  "\r\n\r\n\r\n\r\n\r\n"
	  "\r\n\r\n\r\n\r\n\r\n"
	  "\r\n\r\n\r\n\r\n\r\n"
	  "\r\n\r\n\r\n\r\n");

  /* There is no more data to return after this single string, so return
   pdFALSE. */
  return pdFALSE;
}

/* Structure that defines the "clear" command line command.
 * This will clear terminal screen
 *
 */
static const CLI_Command_Definition_t xClear =
  { "clear", /* The command string to type. */
  "clear:\r\n clear screen \r\n\r\n", prvClearCommand, /* The function to run. */
  0 /* No parameters are expected. */
  };

static portBASE_TYPE
prvResetCommand (char *pcWriteBuffer, size_t xWriteBufferLen,
		 const char *pcCommandString)
{

  /* Remove compile time warnings about unused parameters, and check the
   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
   write buffer length is adequate, so does not check for buffer overflows. */
  (void) pcCommandString;
  (void) xWriteBufferLen;
  (void) pcWriteBuffer;

  usart1.puts ("System reset now...\r\n");

  /* wait for complete */
  timer.sleep_ms (5);

  /* system reset */
  NVIC_SystemReset ();

  /* There is no more data to return after this single string, so return
   pdFALSE. */
  return pdFALSE;
}

/* Structure that defines the "reset" command line command.
 * This will reset or reboot stm32
 *
 */
static const CLI_Command_Definition_t xReset =
  { "reset", /* The command string to type. */
  "reset:\r\n reset system\r\n\r\n", prvResetCommand, /* The function to run. */
  0 /* No parameters are expected. */
  };

/*-----------------------------------------------------------*/

static portBASE_TYPE
prvSayHelloCommand (char *pcWriteBuffer, size_t xWriteBufferLen,
		    const char *pcCommandString)
{

  /* Remove compile time warnings about unused parameters, and check the
   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
   write buffer length is adequate, so does not check for buffer overflows. */
  (void) pcCommandString;
  (void) xWriteBufferLen;
  xWriteBufferLen = MAX_OUTPUT_LENGTH;

  strcpy (pcWriteBuffer, "Hello world!\r\n");

  /* There is no more data to return after this single string, so return
   pdFALSE. */
  return pdFALSE;
}

/* Structure that defines the "hello" command line command.
 * This will print "Hello world!"
 *
 */
static const CLI_Command_Definition_t xHello =
  { "hello", /* The command string to type. */
  "hello:\r\n print Hello world!\r\n\r\n", prvSayHelloCommand, /* The function to run. */
  0 /* No parameters are expected. */
  };

/*-----------------------------------------------------------*/
/* control led on or off
 * led 0|1 on|off
 *
 */
static BaseType_t
prvLedCommand (char *pcWriteBuffer, size_t xWriteBufferLen,
	       const char *pcCommandString)
{

  char *pcParameter1, *pcParameter2;
  BaseType_t xResult;

  (void) xWriteBufferLen;

  memset (CmdLStr, '\0', MAX_INPUT_LENGTH);
  strcpy (CmdLStr, pcCommandString);

  /* with FreeRTOS_CLIGetParameter, C++ has some problems,
   * complete it with own way.
   */
  /* get command itself */
  strtok (CmdLStr, " ,.-");

  /* Obtain first parameter */
  pcParameter1 = strtok (NULL, " ,.-");

  /* Obtain second parameter */
  pcParameter2 = strtok (NULL, " ,.-");

  /* check parameter1 */
  if (strcmp (pcParameter1, "0") != 0 && strcmp (pcParameter1, "1") != 0)
    {
      xResult = pdFALSE;
      goto finish;
    }

  /* check parameter1 */
  if (strcmp (pcParameter2, "on") != 0 && strcmp (pcParameter2, "off") != 0)
    {
      xResult = pdFALSE;
      goto finish;
    }

  switch (pcParameter1[0])
    {
    case '0':
      if (strcmp (pcParameter2, "on") == 0)
	led0.turn_on ();
      else
	led0.turn_off ();
      break;
    case '1':
      if (strcmp (pcParameter2, "on") == 0)
	led1.turn_on ();
      else
	led1.turn_off ();
      break;
    }

  xResult = pdPASS;

  goto finish;

  finish: if (xResult == pdPASS)
    {

      strcpy (pcWriteBuffer, "it done.\r\n");
    }
  else
    {
      /* The copy was not successful.  Inform the users. */
      strcpy (pcWriteBuffer, "led <0|1> <on|off>\r\n");
    }

  /* There is only a single line of output produced in all cases.  pdFALSE is
   returned because there is no more output to be generated. */
  return pdFALSE;
}

/* Structure that defines the "led" command line command.
 * This will turn on or off the led on stm32 board
 *
 */
static const CLI_Command_Definition_t xLed =
  { "led", /* The command string to type. */
  "led <0|1> <on|off>:\r\n turn on or off led0|1\r\n\r\n", prvLedCommand, /* The function to run. */
  2 /* 2 parameters are expected. */
  };

/*-----------------------------------------------------------*/

void
vRegisterCLICommands (void)
{
  /* Register all the command line commands defined immediately above. */
  FreeRTOS_CLIRegisterCommand (&xGetTmpr);
  FreeRTOS_CLIRegisterCommand (&xDate);
  FreeRTOS_CLIRegisterCommand (&xReset);
  FreeRTOS_CLIRegisterCommand (&xHello);
  FreeRTOS_CLIRegisterCommand (&xLed);
  FreeRTOS_CLIRegisterCommand (&xClear);

#if configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1
  FreeRTOS_CLIRegisterCommand( & xStartStopTrace );
#endif
}

void
vCommandConsoleTask (void *pvParameters)
{

  (void) pvParameters;

  BaseType_t xMoreDataToFollow;
  /* The input and output buffers are declared static to keep them off the stack. */
  static char pcOutputString[ MAX_OUTPUT_LENGTH],
      pcInputString[ MAX_INPUT_LENGTH];

  char cChar;

  char *p_message = pcInputString;

#ifdef COMMAND_ECHO
  static const char * const pcWelcomeMessage =
      "\r\nSystem begins to work...\r\n> ";
  /* Send a welcome message to the user knows they are connected. */
  usart1.puts (pcWelcomeMessage);
#endif

  for (;;)
    {
      if (xQueueReceive(cmdQueue, &cChar, portMAX_DELAY) == pdTRUE) /* wait for the cmdQueue, or it will be blocked */
	{
	  /* Take xSemaphoreMutex, and do comnands */
	  if ( xSemaphoreTake(usart1.xSemaphoreMutex,
	      (TickType_t ) 10) == pdTRUE)
	    {
	      /* Clear the input the output buffer ready to receive the command. */
	      memset (pcInputString, '\0', serRX_QUEUE_LEN);
	      memset (pcOutputString, '\0', MAX_OUTPUT_LENGTH);

	      /* receive the command string */
	      p_message = pcInputString;
	      while (xQueueReceive(usart1.xUsartRxedChars, &cChar, 0) == pdTRUE)
		{
		  *p_message++ = cChar;
		}

	      /* The command interpreter is called repeatedly until it returns
	       pdFALSE.  See the "Implementing a command" documentation for an
	       exaplanation of why this is. */
	      do
		{
		  /* Send the command string to the command interpreter.  Any
		   output generated by the command interpreter will be placed in the
		   pcOutputString buffer. */
		  xMoreDataToFollow = FreeRTOS_CLIProcessCommand (
		      pcInputString, /* The command string.*/
		      pcOutputString, /* The output buffer. */
		      MAX_OUTPUT_LENGTH/* The size of the output buffer. */
		      );

		  /* print command running output */
		  usart1.puts (pcOutputString);

		  /* wait usart to finish */
		  timer.sleep_ms (USART_DELAY_MS);
		}
	      while (xMoreDataToFollow != pdFALSE);

#ifdef COMMAND_ECHO
	      usart1.puts ("> ");
#endif
	      /* We have finished jobs.
	       * so release the semaphore.
	       */
	      xSemaphoreGive(usart1.xSemaphoreMutex);
	    }
	}
    }
}

/*-----------------------------------------------------------*/

