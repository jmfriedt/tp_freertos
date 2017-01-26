#include <stm32/gpio.h>
#include <stm32/adc.h>

ADC_InitTypeDef ADC_InitStructure;

unsigned short readADC1(unsigned char channel)                    // jmf convert ADC1
{ ADC_RegularChannelConfig(ADC1,channel,1,ADC_SampleTime_239Cycles5);
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
  while (ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET); // EOC is set @ end of cv
  return(ADC_GetConversionValue(ADC1));
}

void initADC1(void)
{ ADC_DeInit (ADC1) ;
  ADC_InitStructure.ADC_Mode=ADC_Mode_Independent ;
  ADC_InitStructure.ADC_ScanConvMode=DISABLE ;
  ADC_InitStructure.ADC_ContinuousConvMode=DISABLE ;
  ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None ;
  ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right ;
  ADC_InitStructure.ADC_NbrOfChannel=1;
  ADC_Init (ADC1, &ADC_InitStructure ) ;
  ADC_Cmd(ADC1,ENABLE) ;
  ADC_TempSensorVrefintCmd (ENABLE) ;
  ADC_ResetCalibration (ADC1) ;

  while ( ADC_GetResetCalibrationStatus (ADC1) ) ;
  ADC_StartCalibration (ADC1) ;
  while ( ADC_GetCalibrationStatus (ADC1) ) ;
}

