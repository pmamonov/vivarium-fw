#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "stm32f10x_gpio.h"
#include "stdlib.h"
#include "string.h"
#include "cdcio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "strtok.h"

#define SYS_LED_RCC RCC_APB2Periph_GPIOB
#define SYS_LED_GPIO GPIOB
#define SYS_LED_PIN GPIO_Pin_12

/*#define ADC_NCHAN 4
volatile int chan[ADC_NCHAN];
xSemaphoreHandle chanLock;*/

void vChatTask(void* vpars);
void vBlinkTask(void* vpars);
void status_err();
void status_ok();

int main(void){
  portBASE_TYPE err;
  char s[64];

//  SystemInit();
  Set_System();

  // ADC
 // adc_init();

  // Enable System LED
  RCC_APB2PeriphClockCmd(SYS_LED_RCC, ENABLE);
  GPIO_InitTypeDef sGPIOinit;
  sGPIOinit.GPIO_Mode = GPIO_Mode_Out_PP;
  sGPIOinit.GPIO_Speed = GPIO_Speed_10MHz;
  sGPIOinit.GPIO_Pin = SYS_LED_PIN;
  GPIO_Init(SYS_LED_GPIO, &sGPIOinit);
  GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN);

  // USB setup
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();


  err = xTaskCreate( vBlinkTask, "blink", 64, NULL, tskIDLE_PRIORITY+1, NULL );
  if ( err == pdPASS)
    cdc_write_buf(&cdc_out, "blink started\n", 0);
  else{
    sniprintf(s, sizeof(s),"blink failed %d", err);
    cdc_write_buf(&cdc_out, s, 0);
  }

  err = xTaskCreate( vChatTask, "chat", 1024, NULL, tskIDLE_PRIORITY+1, NULL );
  if (err == pdPASS)
    cdc_write_buf(&cdc_out, "chat started\n", 0);
  else{
    sniprintf(s, sizeof(s),"chat failed %d", err);
    cdc_write_buf(&cdc_out, s, 0);
  }

  vTaskStartScheduler();

  for (;1;);
}

void vADCTask(void* vpars){
  int i;
  while (1){
/*    xSemaphoreTake(chanLock, portMAX_DELAY);
    for (i=0; i<ADC_NCHAN; i++){
      ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
      ADC_RegularChannelConfig(ADC1, (uint8_t)i, 1, ADC_SampleTime_144Cycles);
// switch external MUX
//      vTaskDelay(5);
      ADC_SoftwareStartConv(ADC1);
      while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
      chan[i] = ADC_GetConversionValue(ADC1);
    }
    xSemaphoreGive(chanLock);*/
    vTaskDelay(100);
  }
}

void vChatTask(void *vpars){
  char s[64];
  char cmd[64];
  char *tk;
  int16_t i;

  while (1){
    cdc_gets(cmd, sizeof(cmd));
    cdc_write_buf(&cdc_out, cmd, strlen(cmd));
/*
    tk = _strtok(cmd, " \n");
      if (strcmp(tk, "get")==0){
        xSemaphoreTake(chanLock, portMAX_DELAY);
        blink();

//        sniprintf(s,sizeof(s),"%d:%d ", i, ts_get(i));
//        cdc_write_buf(&cdc_out, s, strlen(s));

        for (i=0; i<ADC_NCHAN; i++)
          iprintf(" %d", chan[i]);
        iprintf("\n");
        xSemaphoreGive(chanLock);
      }
      else{
//        iprintf("Unknown command\n");*/
      }
}

void vBlinkTask(void *vpars){
  volatile int i, c=0, x=0, d=1;
  while (1){
    if (GPIO_ReadOutputDataBit(SYS_LED_GPIO, SYS_LED_PIN)) GPIO_ResetBits(SYS_LED_GPIO, SYS_LED_PIN);
    else GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN);
    vTaskDelay(200);
  }
}

void vApplicationStackOverflowHook( xTaskHandle xTask, signed portCHAR *pcTaskName ){
  while(1);
}

void status_err(){
//  cdc_write_buf(&cdc_out, "\nERR\n", 5);
  return;
}

void status_ok(){
//  cdc_write_buf(&cdc_out, "\nOK\n", 4);
  return;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif
