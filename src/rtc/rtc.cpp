#include "rtc.h"

void
rtc::init (void)
{

  /* We are using the semaphore for synchronisation so we create a binary
   semaphore rather than a mutex.  We must make sure that the interrupt
   does not attempt to use the semaphore before it is created! */
  rtcSemaphore = xSemaphoreCreateCounting(20, 0);

#ifdef ENABLE_RTC_IRQ
  /* NVIC configuration */
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
  configLIBRARY_KERNEL_INTERRUPT_PRIORITY - 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init (&NVIC_InitStructure);
#endif

  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd ( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd (ENABLE);

  /* Enable LSE */
  RCC_LSEConfig ( RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus ( RCC_FLAG_LSERDY) == RESET)
    {
    }

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig ( RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd (ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro ();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask ();

#ifdef ENABLE_RTC_IRQ
  /* Enable the RTC Second */
  RTC_ITConfig ( RTC_IT_SEC, ENABLE);
#endif

  if (BKP_ReadBackupRegister ( BKP_DR1) != 0xA5A5)
    {

      /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask ();

      /* Set RTC prescaler: set RTC period to 1sec */
      RTC_SetPrescaler (32785); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) with modified */

      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask ();

      BKP_WriteBackupRegister ( BKP_DR1, 0xA5A5);

    }
  else
    {

//      /* Wait until last write operation on RTC registers has finished */
//      RTC_WaitForLastTask ();
//      RTC_SetCounter (1400550000);

      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask ();

    }

#ifdef RTCClockOutput_Enable
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
   functionality must be disabled */

  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

  /* Clear reset flags */
  RCC_ClearFlag ();
}

void
rtc::update (struct tm *t)
{

  uint32_t counter;

  counter = mktime (t);

  /* Prevent the RTOS kernel swapping out the task. */
  vTaskSuspendAll ();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask ();

  /* Adjust time by values entered by the user on the hyperterminal */
  RTC_SetCounter (counter);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask ();

  /* The operation is complete.  Restart the RTOS kernel. */
  xTaskResumeAll ();
}

void
rtc::puts (char * string, size_t buf_len)
{

  time_t now = RTC_GetCounter ();

  strftime (string, buf_len, "%Y-%m-%d %H:%M:%S %a\r\n", gmtime (&now));
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

//extern rtc stm32rtc;
//
//extern "C" void
//RTC_IRQHandler (void)
//{
//
//  static portBASE_TYPE xHigherPriorityTaskWoken;
//
//  if (RTC_GetITStatus (RTC_IT_SEC) != RESET)
//    {
//
//      /* Clear the RTC Second interrupt */
//      RTC_ClearITPendingBit (RTC_IT_SEC);
//
//      xSemaphoreGiveFromISR(stm32rtc.rtcSemaphore, &xHigherPriorityTaskWoken);
//
//      /* Wait until last write operation on RTC registers has finished */
//      RTC_WaitForLastTask ();
//
//    }
//  /* If xHigherPriorityTaskWoken was set to true you
//   we should yield.  The actual macro used here is
//   port specific. */
//  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//
//}

