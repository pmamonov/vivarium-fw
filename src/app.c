#define __APP

/* Includes ------------------------------------------------------------------*/ 

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include <stdio.h>
//#include "newlib_stubs.h"
#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"

//#include "app.h"

void vChatTask(void* vpars);
void vADCTask(void* vpars);

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

#define ADC_NCHAN 4
volatile int chan[ADC_NCHAN];

int main(void)
{
  int i;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef sGPIOinit;
  sGPIOinit.GPIO_Pin = 1<<15;
  sGPIOinit.GPIO_Mode = GPIO_Mode_OUT;
  sGPIOinit.GPIO_Speed = GPIO_Speed_25MHz;
  sGPIOinit.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOD, &sGPIOinit);

  sGPIOinit.GPIO_Mode = GPIO_Mode_AN;
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
/*
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
*/  
//  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  ADC_Cmd(ADC1, ENABLE);


  USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);

// disable stdio buffering
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  xTaskCreate( vChatTask, "chat", 1024, NULL, tskIDLE_PRIORITY+1, NULL );
  xTaskCreate( vADCTask, "adc", 1024, NULL, tskIDLE_PRIORITY+1, NULL );
  vTaskStartScheduler();
} 

volatile buff_ready=0;

void vChatTask(void* vpars){
#define CMDBUFLEN 100
  volatile char cmdbuf[CMDBUFLEN];
  char *tk;
  int i;
  while (1)
  {
      fgets(cmdbuf, CMDBUFLEN, stdin);
      if (GPIO_ReadOutputDataBit(GPIOD, 1<<15)) GPIO_ResetBits(GPIOD, 1<<15);
      else GPIO_SetBits(GPIOD, 1<<15);
      tk = strtok(cmdbuf, " \n");
      if (strcmp(tk, "get")==0){
        for (i=0; i<ADC_NCHAN; i++)
          iprintf(" %d", chan[i]);
        iprintf("\n");
      }
//      else if(strcmp(tk, "cmd2")==0){
//        iprintf("rpl2\n");
//      }
      else{
        iprintf("Unknown command\n");
      }
  }
}

void vADCTask(void* vpars){
  int i;
  while (1){
    for (i=0; i<ADC_NCHAN; i++){
      ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
      ADC_RegularChannelConfig(ADC1, (uint8_t)i, 1, ADC_SampleTime_3Cycles);
// switch external MUX
      ADC_SoftwareStartConv(ADC1);
      while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
      chan[i] = ADC_GetConversionValue(ADC1);
    }
    vTaskDelay(100);
  }
}

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

