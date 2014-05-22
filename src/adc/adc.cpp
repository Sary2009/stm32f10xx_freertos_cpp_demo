#include "adc.h"
#include "usart.h"

extern usart usart1;

void
adc::init (void)
{

  //enable ADCx clock
  RCC_APB2PeriphClockCmd (adc_init_struct.RCC_APB2Periph_ADCx, ENABLE);

  //load structure values to control and status registers
  ADC_Init (adc_init_struct.ADCx, &adc_init_struct.ADC_InitStructure);

  //wake up temperature sensor
  if (adc_init_struct.ADC_Channel_x == 16)
    ADC_TempSensorVrefintCmd (ENABLE);

  //ADCx channel x configuration
  ADC_RegularChannelConfig (adc_init_struct.ADCx, adc_init_struct.ADC_Channel_x,
			    adc_init_struct.Rank,
			    adc_init_struct.ADC_SampleTime);

  //Enable ADCx
  ADC_Cmd (adc_init_struct.ADCx, ENABLE);

  //Enable ADCx reset calibration register
  ADC_ResetCalibration (adc_init_struct.ADCx);

  //Check the end of ADCx reset calibration register
  while (ADC_GetResetCalibrationStatus (adc_init_struct.ADCx))
    ;
  //Start ADCx calibration
  ADC_StartCalibration (adc_init_struct.ADCx);

  //Check the end of ADCx calibration
  while (ADC_GetCalibrationStatus (adc_init_struct.ADCx))
    ;
}

uint16_t
adc::get (void)
{
  u16 AD_value;

  //Start ADCx Software Conversion
  ADC_SoftwareStartConvCmd (adc_init_struct.ADCx, ENABLE);
  //wait for conversion complete
  while (!ADC_GetFlagStatus (adc_init_struct.ADCx, ADC_FLAG_EOC))
    {
    }
  //read ADC value
  AD_value = ADC_GetConversionValue (adc_init_struct.ADCx);
  //clear EOC flag
  ADC_ClearFlag (adc_init_struct.ADCx, ADC_FLAG_EOC);

  return AD_value;
}

void
adc::puts (char * string, size_t buf_len)
{
  if (adc_init_struct.ADC_Channel_x == 16)
    {

      int tmpr;

      char tmp[4];

      /* for adc channel 16 temperature sensor */
      const uint16_t V25 = 1750; // when V25=1.41V at ref 3.3V
      const uint16_t Avg_Slope = 5; //when avg_slope=4.3mV/C at ref 3.3V

      tmpr = (V25 - get ()) * 10 / Avg_Slope + 25 * 10;

      if (buf_len < 30)
	return;

      strcpy (string, "The temperature of chip is: ");

      sprintf (tmp, "%d.", tmpr / 10);
      strcat (string, tmp);

      sprintf (tmp, "%d\r\n", tmpr % 10);
      strcat (string, tmp);
    }
}
