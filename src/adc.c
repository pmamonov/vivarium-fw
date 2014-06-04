#include "adc.h"

void adc_init(void){
/*  sGPIOinit.GPIO_Mode = GPIO_Mode_AN;
  sGPIOinit.GPIO_OType = GPIO_OType_OD;
  sGPIOinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  for (i=0; i<ADC_NCHAN; i++){
    sGPIOinit.GPIO_Pin = 1<<i;
    GPIO_Init(GPIOA, &sGPIOinit);
  }

  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

//  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_Init(&NVIC_InitStructure);
//  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  ADC_Cmd(ADC1, ENABLE);*/
  return;
}

void adc_get(int i){
  return 0;
}
